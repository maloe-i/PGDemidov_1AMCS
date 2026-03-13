// 1

// 0.0 5.0 -5.0 -5.0 5.0 -5.0 0.0 0.0

#include <iostream>
#include <algorithm>
#include "glut.h"
#include <clocale>

// Класс точки
class Point {
public:
    float x, y;
    Point(float _x = 0, float _y = 0) : x(_x), y(_y) {}
};

// Класс треугольника
class Triangle {
private:
    Point a, b, c;
public:
    Triangle(Point _a = Point(), Point _b = Point(), Point _c = Point()) : a(_a), b(_b), c(_c) {}

    // 0 - снаружи, 1 - внутри, 2 - на границе
    int checkPointPosition(const Point& p) const {
        float d1 = (a.x - p.x) * (b.y - a.y) - (b.x - a.x) * (a.y - p.y);
        float d2 = (b.x - p.x) * (c.y - b.y) - (c.x - b.x) * (b.y - p.y);
        float d3 = (c.x - p.x) * (a.y - c.y) - (a.x - c.x) * (c.y - p.y);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
        bool has_zero = (d1 == 0) || (d2 == 0) || (d3 == 0);

        if (has_neg && has_pos) return 0; // Разные знаки - точка снаружи
        if (has_zero) return 2;           // Лежит на границе
        return 1;                         // Строго внутри
    }

    // Отрисовка треугольника
    void draw() const {
        glColor3f(0.0f, 0.0f, 0.0f); // Черный контур
        glBegin(GL_LINE_LOOP);
        glVertex2f(a.x, a.y);
        glVertex2f(b.x, b.y);
        glVertex2f(c.x, c.y);
        glEnd();
    }
};

// Глобальные объекты для GLUT
Triangle globalTriangle;
Point globalPoint;
int pointStatus = 0;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Отрисовка треугольника
    globalTriangle.draw();

    // Установка цвета точки в зависимости от ее положения
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    if (pointStatus == 1) {
        glColor3f(0.0f, 1.0f, 0.0f); // Зеленый - внутри
    }
    else if (pointStatus == 2) {
        glColor3f(0.0f, 0.0f, 1.0f); // Синий - на границе
    }
    else {
        glColor3f(1.0f, 0.0f, 0.0f); // Красный - снаружи
    }
    glVertex2f(globalPoint.x, globalPoint.y);
    glEnd();

    glutSwapBuffers();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // Белый фон
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Настраиваем ортогональную проекцию (система координат от -10 до 10)
    // хз вроде надо
    gluOrtho2D(-10.0, 10.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    float tx1, ty1, tx2, ty2, tx3, ty3, px, py;
    
    if (std::cin >> tx1 >> ty1 >> tx2 >> ty2 >> tx3 >> ty3 >> px >> py) {
        
        // Проверка на треугольник
        
        float area2 = (tx2 - tx1) * (ty3 - ty1) - (tx3 - tx1) * (ty2 - ty1);
        
        if (area2 == 0) {
            std::cout << "Ошибка: Введенные координаты не образуют треугольник (точки лежат на одной прямой)." << std::endl;
            return 1; 
        }

        globalTriangle = Triangle(Point(tx1, ty1), Point(tx2, ty2), Point(tx3, ty3));
        globalPoint = Point(px, py);

        pointStatus = globalTriangle.checkPointPosition(globalPoint);

        std::cout << "Результат проверки: ";
        if (pointStatus == 1) std::cout << "Точка внутри треугольника\n";
        else if (pointStatus == 2) std::cout << "Точка на границе треугольника\n";
        else std::cout << "Точка снаружи треугольника\n";
    }
    else {
        std::cout << "Ошибка чтения данных.\n";
        return 1;
    }

    // Инициализация GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Triangle and Point Position");

    init();
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
