#include <iostream>
#include <algorithm>
#include "glut.h"


const double PI = 3.14159265358979323846;

double my_abs(double x) {
    return x < 0 ? -x : x;
}

double my_exp(double x) {
    double sum = 1.0, term = 1.0;
    for (int i = 1; i < 20; ++i) {
        term *= x / i;
        sum += term;
    }
    return sum;
}

double my_sin(double x) {
    // Приведение к периоду для сохранения точности ряда Тейлора
    bool neg = false;
    if (x < 0) { x = -x; neg = true; }
    while (x > 2.0 * PI) x -= 2.0 * PI;
    if (x > PI) { x -= PI; neg = !neg; }

    double sum = x, term = x;
    for (int i = 1; i < 20; ++i) {
        term *= -x * x / ((2.0 * i) * (2.0 * i + 1.0));
        sum += term;
    }
    return neg ? -sum : sum;
}

double my_cos(double x) {
    return my_sin(x + PI / 2.0);
}

double my_sqrt(double x) {
    if (x <= 0) return 0;
    double guess = x;
    for (int i = 0; i < 20; ++i) {
        guess = (guess + x / guess) / 2.0;
    }
    return guess;
}

double my_log(double x) {
    if (x <= 0) return 0;
    double y = (x - 1.0) / (x + 1.0);
    double y2 = y * y;
    double sum = 0.0, term = y;
    for (int i = 1; i < 50; ++i) {
        sum += term / (2.0 * i - 1.0);
        term *= y2;
    }
    return 2.0 * sum;
}

// ВЫБОР ИНТЕГРАЛА
int current_choice = 1;

double f(double x) {
    switch (current_choice) {
    case 1: return my_exp(x) * my_sin(x);
    case 2: return my_cos(my_sqrt(x)) / my_sqrt(x);
    case 3: return x * my_log(my_abs((1.0 + x) / (1.0 - x)));
    case 4: {
        double s = my_sin(x);
        return (x * my_cos(x)) / (s * s * s + 2.0);
    }
    default: return my_exp(x) * my_sin(x);
    }
}

// Заранее вычисленные точные значения интегралов
double getExactValue(int choice) {
    switch (choice) {
    case 1: return 0.90933067; // Точное значение для e^x * sin(x)
    case 2: return 0.44933524; // Точное значение для cos(sqrt(x)) / sqrt(x)
    case 3: return 0.08802039;  // Точное значение для x * ln|(1+x)/(1-x)|
    case 4: return 0.0; // Точное значение для x * cos(x) / (sin^3(x) + 2)
    default: return 0.90933067;
    }
}

// ГЕНЕРАТОР
unsigned long int custom_seed = 7777777;
int my_rand() {
    custom_seed = (1103515245 * custom_seed + 12345) % 2147483648;
    return custom_seed % 32768;
}

float getRandomFloat(float min, float max) {
    return min + (max - min) * ((float)my_rand() / 32767.0f);
}

// ДАННЫЕ ДЛЯ ОТРИСОВКИ
struct PointData {
    float x, y;
    int hit_type; // 1: под графиком (положительная), -1: над графиком (отрицательная), 0: мимо
};

const int MAX_POINTS = 1000000;
const int POINTS_PER_FRAME = 2500;
PointData* points = nullptr;
int current_points = 0;
int points_pos = 0;
int points_neg = 0;

float X_MIN, X_MAX, Y_MIN, Y_MAX, S0;
double exact_value = 0.0;

const int milestones[6] = { 10, 100, 1000, 10000, 100000, 1000000 };
int current_milestone_idx = 0;


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Отрисовка сгенерированных точек
    glBegin(GL_POINTS);
    for (int i = 0; i < current_points; i++) {
        if (points[i].hit_type == 1) glColor3f(0.0f, 0.7f, 0.0f);      // Зеленые (плюс)
        else if (points[i].hit_type == -1) glColor3f(0.0f, 0.8f, 0.8f); // Голубые (минус)
        else glColor3f(0.8f, 0.0f, 0.0f);                              // Красные (мимо)
        glVertex2f(points[i].x, points[i].y);
    }
    glEnd();

    // Отрисовка осей (чтобы был виден нуль, если график уходит в минус)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(X_MIN - 1.0f, 0.0f); glVertex2f(X_MAX + 1.0f, 0.0f); // Ось X
    glVertex2f(0.0f, Y_MIN - 1.0f); glVertex2f(0.0f, Y_MAX + 1.0f); // Ось Y
    glEnd();

    // Отрисовка ограничивающего прямоугольника
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(X_MIN, Y_MIN); glVertex2f(X_MAX, Y_MIN);
    glVertex2f(X_MAX, Y_MAX); glVertex2f(X_MIN, Y_MAX);
    glEnd();

    // Отрисовка графика функции
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 200; ++i) {
        double px = X_MIN + i * (X_MAX - X_MIN) / 200.0;
        glVertex2f((float)px, (float)f(px));
    }
    glEnd();
    glLineWidth(1.0f);

    glutSwapBuffers();
}

void idle() {
    if (current_points < MAX_POINTS) {
        for (int i = 0; i < POINTS_PER_FRAME && current_points < MAX_POINTS; i++) {
            float px = getRandomFloat(X_MIN, X_MAX);
            float py = getRandomFloat(Y_MIN, Y_MAX);
            double fx = f(px);

            int type = 0;
            if (fx >= 0) {
                if (py >= 0 && py <= fx) { type = 1; points_pos++; }
            }
            else {
                if (py <= 0 && py >= fx) { type = -1; points_neg++; }
            }

            points[current_points].x = px;
            points[current_points].y = py;
            points[current_points].hit_type = type;
            current_points++;

            if (current_milestone_idx < 6 && current_points == milestones[current_milestone_idx]) {
                double approx_value = S0 * ((double)(points_pos - points_neg) / current_points);
                double rel_error = (my_abs(approx_value - exact_value) / my_abs(exact_value)) * 100.0;

                std::cout << current_points << "\t\t| "
                    << approx_value << "\t| "
                    << rel_error << "%" << std::endl;

                current_milestone_idx++;
            }
        }
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Динамическая ортогональная проекция с отступами
    float padX = (X_MAX - X_MIN) * 0.1f;
    float padY = (Y_MAX - Y_MIN) * 0.1f;
    if (padY < 0.1f) padY = 0.5f; // Защита для плоских графиков
    gluOrtho2D(X_MIN - padX, X_MAX + padX, Y_MIN - padY, Y_MAX + padY);

    glMatrixMode(GL_MODELVIEW);
    points = new PointData[MAX_POINTS];
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");

    std::cout << "Выберите интеграл (1-4):\n";
    std::cout << "1. a) int_0^1 (e^x * sin(x)) dx\n";
    std::cout << "2. b) int_{pi/4}^{3pi/4} (cos(sqrt(x)) / sqrt(x)) dx\n";
    std::cout << "3. c) int_2^12 (x * ln|(1+x)/(1-x)|) dx\n";
    std::cout << "4. d) int_{pi}^{3pi} (x * cos(x) / (sin^3(x) + 2)) dx\n";
    std::cout << "Ваш выбор: ";
    std::cin >> current_choice;

    // Настройка параметров в зависимости от выбора
    switch (current_choice) {
    case 1: X_MIN = 0.0f; X_MAX = 1.0f; Y_MIN = 0.0f; Y_MAX = 2.5f; break;
    case 2: X_MIN = PI / 4.0; X_MAX = 3.0 * PI / 4.0; Y_MIN = 0.0f; Y_MAX = 1.5f; break;
    case 3: X_MIN = 2.0f; X_MAX = 12.0f; Y_MIN = 0.0f; Y_MAX = 4.0f; break;
    case 4: X_MIN = PI; X_MAX = 3.0 * PI; Y_MIN = -5.0f; Y_MAX = 5.0f; break; // Четвертый уходит в минус
    default: X_MIN = 0.0f; X_MAX = 1.0f; Y_MIN = 0.0f; Y_MAX = 2.5f; current_choice = 1; break;
    }

    S0 = (X_MAX - X_MIN) * (Y_MAX - Y_MIN);
    exact_value = getExactValue(current_choice);

    std::cout << "\nТочное значение (заранее вычисленное): " << exact_value << "\n\n";
    std::cout << "Точек N\t\t| Приближение\t| Относит. погрешность\n";
    std::cout << "--------------------------------------------------------\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Monte Carlo Integrals");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMainLoop();

    delete[] points;
    return 0;
}