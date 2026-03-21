// 10

#include <iostream>
#include <algorithm>
#include "glut.h"


unsigned long int custom_seed = 123456789;
int my_rand() {
    custom_seed = (1103515245 * custom_seed + 12345) % 2147483648;
    return custom_seed % 32768;
}


class Point {
public:
    float x, y;
    Point(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
};

const int TOTAL_POINTS = 1000000;
Point* point_sequence = nullptr;

// Вершины правильного квадрата
Point vertices[4] = {
    Point(100.0f, 100.0f),
    Point(-100.0f, 100.0f),
    Point(-100.0f, -100.0f),
    Point(100.0f, -100.0f)
};

// Переменные состояния для визуализации
int current_drawn_points = 0;
bool is_instant_mode = false;
float zoom_level = 1.0f;
float offset_x = 0.0f;
float offset_y = 0.0f;


// Логика вычислений
void calculateChaosGame() {
    point_sequence = new Point[TOTAL_POINTS];
    Point current(0.0f, 0.0f); // Стартовая позиция (x0, y0)

    for (int k = 0; k < TOTAL_POINTS; ++k) {
        // Имитация броска 10-гранного кубика (получаем от 1 до 10)
        int roll = (my_rand() % 10) + 1;
        int target_vertex = 0;

        
        if (roll <= 2) target_vertex = 0;
        else if (roll <= 4) target_vertex = 1;
        else if (roll <= 7) target_vertex = 2;
        else target_vertex = 3;

        // Вычисление смещения на пропорцию R = 0.50
        current.x = current.x + 0.50f * (vertices[target_vertex].x - current.x);
        current.y = current.y + 0.50f * (vertices[target_vertex].y - current.y);

        point_sequence[k] = current;
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Применяем масштабирование и перемещение
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(offset_x, offset_y, 0.0f);
    glScalef(zoom_level, zoom_level, 1.0f);

    // Отрисовка сгенерированных точек
    glColor3f(0.0f, 0.0f, 0.0f); // Черные точки фрактала
    glBegin(GL_POINTS);
    int limit = is_instant_mode ? TOTAL_POINTS : current_drawn_points;
    for (int i = 0; i < limit; ++i) {
        glVertex2f(point_sequence[i].x, point_sequence[i].y);
    }
    glEnd();

    // Отрисовка опорных вершин (красным цветом для наглядности)
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 4; ++i) {
        glVertex2f(vertices[i].x, vertices[i].y);
    }
    glEnd();
    glPointSize(1.0f); // Возврат размера точек в норму

    glutSwapBuffers();
}

void idle() {
    // В режиме реального времени
    if (!is_instant_mode && current_drawn_points < TOTAL_POINTS) {
        current_drawn_points += 100; // Увеличил шаг для более заметной анимации при 1 млн точек
        if (current_drawn_points > TOTAL_POINTS) {
            current_drawn_points = TOTAL_POINTS;
        }
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1': // Режим реального времени
        is_instant_mode = false;
        current_drawn_points = 0;
        break;
    case '2': // Мгновенный показ
        is_instant_mode = true;
        break;
    case '+': // Приблизить
    case '=':
        zoom_level *= 1.2f;
        break;
    case '-': // Отдалить
    case '_':
        zoom_level /= 1.2f;
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float step = 10.0f / zoom_level; // Шаг зависит от зума для удобства
    switch (key) {
    case GLUT_KEY_UP:    offset_y += step; break;
    case GLUT_KEY_DOWN:  offset_y -= step; break;
    case GLUT_KEY_LEFT:  offset_x -= step; break;
    case GLUT_KEY_RIGHT: offset_x += step; break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Белый фон
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ортогональная проекция с запасом по краям
    gluOrtho2D(-150.0, 150.0, -150.0, 150.0);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    std::cout << "Генерация 1.000.000 точек... Ожидайте.\n";

    // Предрасчет массива точек до запуска графики
    calculateChaosGame();

    std::cout << "Управление:\n";
    std::cout << "[ 1 ] - Анимация (построение в реальном времени)\n";
    std::cout << "[ 2 ] - Мгновенное отображение всех точек\n";
    std::cout << "[ + ] - Увеличить масштаб\n";
    std::cout << "[ - ] - Уменьшить масштаб\n";
    std::cout << "[ СТРЕЛКИ ] - Перемещение по графику\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Chaos Game: N=4, M=10, R=0.50");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle);         // Функция, работающая в фоне (для анимации)
    glutKeyboardFunc(keyboard); // Обработка обычных клавиш
    glutSpecialFunc(specialKeys); // Обработка стрелок

    glutMainLoop();

    return 0;
}
