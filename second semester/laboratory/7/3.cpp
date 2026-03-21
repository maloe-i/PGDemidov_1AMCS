// 4

#include <iostream>
#include <algorithm>
#include "glut.h"

unsigned long int custom_seed = 987654321;
int my_rand() {
    custom_seed = (1103515245 * custom_seed + 12345) % 2147483648;
    return custom_seed % 32768;
}

// Генерация числа с плавающей точкой в диапазоне [min, max]
float getRandomFloat(float min, float max) {
    return min + (max - min) * ((float)my_rand() / 32767.0f);
}


struct PointData {
    float x, y;
    bool is_inside;
};

const int MAX_POINTS = 1000000;
const int POINTS_PER_FRAME = 2000; // Скорость отрисовки

PointData* points = nullptr;
int current_points = 0;
int points_inside = 0;

// Площадь ограничивающего квадрата (-2 < x < 2, -2 < y < 2)
const float S0 = 16.0f;

// Математическая модель (проверка системы неравенств)
bool checkConditions(float x, float y) {
    // Условие 1: -x^3 + y^5 < 2
    if (-x * x * x + y * y * y * y * y >= 2.0f) return false;

    // Условие 2: x - y < 1
    if (x - y >= 1.0f) return false;

    return true;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Отрисовка ограничивающего квадрата (рамки)
    glColor3f(0.0f, 0.0f, 0.0f); // Черный контур
    glBegin(GL_LINE_LOOP);
    glVertex2f(-2.0f, -2.0f);
    glVertex2f(2.0f, -2.0f);
    glVertex2f(2.0f, 2.0f);
    glVertex2f(-2.0f, 2.0f);
    glEnd();

    // Отрисовка сгенерированных точек
    glBegin(GL_POINTS);
    for (int i = 0; i < current_points; i++) {
        if (points[i].is_inside) {
            glColor3f(0.0f, 0.8f, 0.0f); // Зеленый - точка принадлежит фигуре
        }
        else {
            glColor3f(0.8f, 0.0f, 0.0f); // Красный - точка вне фигуры
        }
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();

    glutSwapBuffers();
}

void idle() {
    if (current_points < MAX_POINTS) {
        // Добавляем партию точек за один кадр
        for (int i = 0; i < POINTS_PER_FRAME && current_points < MAX_POINTS; i++) {
            float x = getRandomFloat(-2.0f, 2.0f);
            float y = getRandomFloat(-2.0f, 2.0f);
            bool inside = checkConditions(x, y);

            points[current_points].x = x;
            points[current_points].y = y;
            points[current_points].is_inside = inside;

            if (inside) points_inside++;
            current_points++;
        }

        // Вывод промежуточных результатов в консоль каждые 50000 точек
        if (current_points % 50000 == 0 || current_points == MAX_POINTS) {
            float estimated_area = S0 * ((float)points_inside / (float)current_points);
            std::cout << "Сгенерировано точек: " << current_points
                << " | Текущая площадь S ≈ " << estimated_area << "\n";
        }

        glutPostRedisplay();
    }
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Белый фон
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Ортогональная проекция с небольшим запасом для отображения рамки (-2.5 до 2.5)
    gluOrtho2D(-2.5, 2.5, -2.5, 2.5);
    glMatrixMode(GL_MODELVIEW);

    // Выделение памяти под точки
    points = new PointData[MAX_POINTS];
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    std::cout << "Вычисление площади методом Монте-Карло...\n";
    std::cout << "Площадь прямоугольника S0 = " << S0 << "\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600); // Квадратное окно сохраняет пропорции
    glutCreateWindow("Monte Carlo Area Calculation");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle); // Запускает фоновую генерацию

    glutMainLoop();

    delete[] points; // Очистка памяти при выходе
    return 0;
}
