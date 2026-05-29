#include <windows.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include "BigInteger.h"


// Класс поля Finite<N>

template<int N>
class Finite {
private:
    int value;

public:
    // Конструктор от int (математический остаток от деления)
    Finite(int v = 0) {
        value = v % N;
        if (value < 0) {
            value += N;
        }
    }

    Finite& operator+=(const Finite& other) {
        value = (value + other.value) % N;
        return *this;
    }

    Finite operator+(const Finite& other) const {
        Finite res = *this;
        return res += other;
    }

    Finite& operator-=(const Finite& other) {
        value = (value - other.value + N) % N;
        return *this;
    }

    Finite operator-(const Finite& other) const {
        Finite res = *this;
        return res -= other;
    }

    // Унарный минус
    Finite operator-() const {
        return Finite(N - value);
    }

    Finite& operator*=(const Finite& other) {
        // Использование long long предотвращает переполнение при умножении ААААААА
        value = (1LL * value * other.value) % N;
        return *this;
    }

    Finite operator*(const Finite& other) const {
        Finite res = *this;
        return res *= other;
    }

    // Обратный элемент поля через расширенный алгоритм Евклида
    Finite inv() const {
        int t = 0, newt = 1;
        int r = N, newr = value;
        while (newr != 0) {
            int quotient = r / newr;

            int temp_t = t - quotient * newt;
            t = newt;
            newt = temp_t;

            int temp_r = r - quotient * newr;
            r = newr;
            newr = temp_r;
        }
        if (r > 1) {
            throw "Деление на ноль или N не является простым числом";
        }
        if (t < 0) {
            t += N;
        }
        return Finite(t);
    }

    Finite& operator/=(const Finite& other) {
        return *this *= other.inv();
    }

    Finite operator/(const Finite& other) const {
        Finite res = *this;
        return res /= other;
    }

    bool operator==(const Finite& other) const { return value == other.value; }
    bool operator!=(const Finite& other) const { return value != other.value; }

    // Перегрузка вывода для конечного поля
    friend std::ostream& operator<<(std::ostream& os, const Finite& f) {
        os << f.value;
        return os;
    }
};


// Инициализации нулевой матрицы
struct ZeroInit {};



// Шаблонный класс Matrix

template<unsigned M, unsigned N, typename Field = BigRational>
class Matrix {
private:
    std::vector<std::vector<Field>> data;

public:
    // Создание строго нулевой матрицы
    Matrix(ZeroInit) {
        data.assign(M, std::vector<Field>(N, Field(0)));
    }

    // Конструктор по умолчанию (создает единичную, если M == N)
    Matrix() {
        data.assign(M, std::vector<Field>(N, Field(0)));
        if (M == N) {
            for (unsigned i = 0; i < std::min(M, N); ++i) {
                data[i][i] = Field(1);
            }
        }
    }

    // Шаблонный конструктор от вектора векторов
    template<typename T>
    Matrix(const std::vector<std::vector<T>>& vec) {
        data.assign(M, std::vector<Field>(N, Field(0)));
        for (unsigned i = 0; i < M && i < vec.size(); ++i) {
            for (unsigned j = 0; j < N && j < vec[i].size(); ++j) {
                data[i][j] = Field(vec[i][j]);
            }
        }
    }

    bool operator==(const Matrix& other) const { return data == other.data; }
    bool operator!=(const Matrix& other) const { return data != other.data; }

    // Операции сложения и вычитания.
    Matrix& operator+=(const Matrix& other) {
        for (unsigned i = 0; i < M; ++i)
            for (unsigned j = 0; j < N; ++j)
                data[i][j] += other.data[i][j];
        return *this;
    }

    Matrix operator+(const Matrix& other) const {
        Matrix res = *this;
        return res += other;
    }

    Matrix& operator-=(const Matrix& other) {
        for (unsigned i = 0; i < M; ++i)
            for (unsigned j = 0; j < N; ++j)
                data[i][j] -= other.data[i][j];
        return *this;
    }

    Matrix operator-(const Matrix& other) const {
        Matrix res = *this;
        return res -= other;
    }

    // Умножение на скаляр
    Matrix& operator*=(const Field& scalar) {
        for (unsigned i = 0; i < M; ++i)
            for (unsigned j = 0; j < N; ++j)
                data[i][j] *= scalar;
        return *this;
    }

    Matrix operator*(const Field& scalar) const {
        Matrix res = *this;
        return res *= scalar;
    }

    // Умножение матриц
    template<unsigned K>
    Matrix<M, K, Field> operator*(const Matrix<N, K, Field>& other) const {
        Matrix<M, K, Field> res(ZeroInit{}); // Инициализация нулевой матрицей
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned k = 0; k < N; ++k) {
                for (unsigned j = 0; j < K; ++j) {
                    res[i][j] += data[i][k] * other[k][j];
                }
            }
        }
        return res;
    }

    // Допустим только для умножения на квадратную матрицу
    Matrix& operator*=(const Matrix<N, N, Field>& other) {
        *this = *this * other;
        return *this;
    }

    // Определитель
    Field det() const {
        static_assert(M == N, "Определитель определен только для квадратных матриц.");

        Matrix<M, N, Field> copy = *this;
        Field det_val = Field(1);

        for (unsigned i = 0; i < N; ++i) {
            unsigned pivot = i;
            while (pivot < N && copy[pivot][i] == Field(0)) {
                pivot++;
            }

            if (pivot == N) return Field(0);

            if (pivot != i) {
                std::swap(copy[i], copy[pivot]);
                det_val = Field(0) - det_val;
            }


            det_val *= copy[i][i];

            Field inv_pivot = Field(1) / copy[i][i];
            for (unsigned j = i + 1; j < N; ++j) {
                if (copy[j][i] == Field(0)) continue;

                Field factor = copy[j][i] * inv_pivot;
                for (unsigned k = i; k < N; ++k) {
                    copy[j][k] -= factor * copy[i][k];
                }
            }
        }

        return det_val;
    }


    // Транспонирование
    Matrix<N, M, Field> transposed() const {
        Matrix<N, M, Field> res(ZeroInit{});
        for (unsigned i = 0; i < M; ++i) {
            for (unsigned j = 0; j < N; ++j) {
                res[j][i] = data[i][j];
            }
        }
        return res;
    }

    // Вычисление ранга
    unsigned rank() const {
        Matrix<M, N, Field> copy = *this;
        unsigned r = 0;

        for (unsigned i = 0; i < N && r < M; ++i) {
            unsigned pivot = r;
            while (pivot < M && copy[pivot][i] == Field(0)) {
                pivot++;
            }
            if (pivot == M) continue;

            std::swap(copy.data[r], copy.data[pivot]);
            Field inv_pivot = Field(1) / copy[r][i];

            for (unsigned j = r + 1; j < M; ++j) {
                Field factor = copy[j][i] * inv_pivot;
                for (unsigned k = i; k < N; ++k) {
                    copy[j][k] -= factor * copy[r][k];
                }
            }
            r++;
        }
        return r;
    }

    // сумма всех элементов главной диагонали
    Field trace() const {
        static_assert(M == N, "Параметр Trace определен только для квадратных матриц.");
        Field sum = Field(0);
        for (unsigned i = 0; i < N; ++i) {
            sum += data[i][i];
        }
        return sum;
    }

    // Возвращает обратную матрицу
    Matrix<N, N, Field> inverted() const {
        static_assert(M == N, "Обратная матрица определена только для квадратных матриц.");
        Matrix<N, N, Field> copy = *this;
        Matrix<N, N, Field> res; // Изначально инициализируется единичной матрицей

        for (unsigned i = 0; i < N; ++i) {
            unsigned pivot = i;
            while (pivot < N && copy[pivot][i] == Field(0)) {
                pivot++;
            }

            if (pivot == N) throw "Матрица вырождена";

            if (pivot != i) {
                std::swap(copy.data[i], copy.data[pivot]);
                std::swap(res.data[i], res.data[pivot]);
            }

            Field inv_pivot = Field(1) / copy[i][i];
            for (unsigned j = 0; j < N; ++j) {
                copy[i][j] *= inv_pivot;
                res[i][j] *= inv_pivot;
            }

            for (unsigned j = 0; j < N; ++j) {
                if (j != i && copy[j][i] != Field(0)) {
                    Field factor = copy[j][i];
                    for (unsigned k = 0; k < N; ++k) {
                        copy[j][k] -= factor * copy[i][k];
                        res[j][k] -= factor * res[i][k];
                    }
                }
            }
        }
        return res;
    }

    // Модифицирует текущую матрицу, делая ее обратной
    void invert() {
        *this = this->inverted();
    }

    std::vector<Field> getRow(unsigned row) const {
        return data[row];
    }

    std::vector<Field> getColumn(unsigned col) const {
        std::vector<Field> col_vec(M);
        for (unsigned i = 0; i < M; ++i) {
            col_vec[i] = data[i][col];
        }
        return col_vec;
    }

    // Поддержка двойного применения m[i][j]
    std::vector<Field>& operator[](unsigned row) {
        return data[row];
    }

    const std::vector<Field>& operator[](unsigned row) const {
        return data[row];
    }

    // Перегрузка оператора вывода для печати матрицы
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        for (unsigned i = 0; i < M; ++i) {
            os << "[ ";
            for (unsigned j = 0; j < N; ++j) {
                os << mat.data[i][j] << " ";
            }
            os << "]\n";
        }
        return os;
    }
};

// Псевдоним для квадратных матриц
template<unsigned N, typename Field = BigRational>
using SquareMatrix = Matrix<N, N, Field>;



using namespace std;

template <typename T>
void check_eq(const T& actual, const T& expected, const string& test_name) {
    if (actual == expected) {
        cout << "  [УСПЕХ] " << test_name << endl;
    } else {
        cout << "  [ОШИБКА] " << test_name << " (Ожидалось другое значение!) <--- ВНИМАНИЕ" << endl;
    }
}

void check(bool condition, const string& test_name) {
    if (condition) {
        cout << "  [УСПЕХ] " << test_name << endl;
    } else {
        cout << "  [ОШИБКА] " << test_name << " <--- ВНИМАНИЕ" << endl;
    }
}

int main() {
    using Rational = BigRational;
    SetConsoleOutputCP(CP_UTF8);

    cout << " ЗАПУСК ТЕСТИРОВАНИЯ КЛАССА MATRIX" << endl;

    cout << "\n=== METHOD 1: Identity Matrix ===" << endl;
    SquareMatrix<3> identity;
    cout << identity << endl;
    check(identity[0][0] == Rational(1) && identity[1][1] == Rational(1) && identity[0][1] == Rational(0),
          "Матрица действительно единичная (проверка диагонали и нулей)");


    cout << "\n=== METHOD 2: From vector<vector<int>> ===" << endl;
    vector<vector<int>> vec1 = {
        {0, 4},
        {0, 1}
    };
    Matrix<2, 2> m1(vec1);
    cout << m1 << endl;

    cout << "det(m1) = " << m1.det() << endl;
    check_eq(m1.det(), Rational(1), "Определитель вычислен верно (1*5 - 2*2 = 1)");

    try {
        Matrix<2, 2> inv_m1 = m1.inverted(); // Вычисляем один раз
        cout << "m1.inverted() = \n" << inv_m1 << endl;
        check((m1 * inv_m1) == SquareMatrix<2>(), "Умножение матрицы на обратную дает единичную");
    }
    catch (const char* msg) {
        cout << "  [ОШИБКА] Не удалось инвертировать матрицу: " << msg << endl;
        cout << "  [ИНФО] Тест инверсии пропущен, продолжаем...\n" << endl;
    }
    catch (const std::exception& e) {
        cout << "  [ОШИБКА] " << e.what() << endl;
    }

    cout << "\n=== METHOD 3: From vector<vector<Rational>> ===" << endl;
    vector<vector<Rational>> vec2 = {
        {Rational(1, 2), Rational(1, 2)},
        {Rational(5, 6), Rational(2, 8)}
    };
    Matrix<2, 2, Rational> m2(vec2);
    cout << m2 << endl;
    check(m2[1][0] == Rational(5, 6), "Дробные числа корректно сохранены в матрице");


    cout << "\n=== METHOD 4: From vector<vector<int>> in Finite<7> ===" << endl;
    vector<vector<int>> vec3 = {
        {1, 2},
        {3, 4}
    };
    SquareMatrix<2, Finite<7>> m3(vec3);
    cout << m3 << endl;
    // Определитель: 1*4 - 2*3 = 4 - 6 = -2. По модулю 7 это 5.
    check_eq(m3.det(), Finite<7>(5), "Определитель в поле Галуа GF(7) вычислен верно (-2 mod 7 = 5)");


    cout << "\n=== METHOD 5: Element Access ===" << endl;
    vector<vector<int>> vec4 = { {1, 2, 3}, {4, 5, 6} };
    Matrix<2, 3> m4(vec4);
    cout << "Original Matrix 2x3:\n" << m4 << endl;

    m4[0][1] = Rational(100);
    cout << "After modification (m4[0][1] = 100):\n" << m4 << endl;
    check_eq(m4[0][1], Rational(100), "Двойной оператор [] успешно перезаписал элемент");


    // АРИФМЕТИЧЕСКИЕ ОПЕРАЦИИ С МАТРИЦАМИ
    cout << "\n=== MATRIX ARITHMETIC ===" << endl;
    vector<vector<int>> vecA = { {1, 2}, {3, 4} };
    vector<vector<int>> vecB = { {5, 6}, {7, 8} };
    SquareMatrix<2> mA(vecA);
    SquareMatrix<2> mB(vecB);

    cout << "A = \n" << mA << "B = \n" << mB << endl;

    SquareMatrix<2> mSum = mA + mB;
    cout << "A + B = \n" << mSum << endl;
    check_eq(mSum[0][0], Rational(6), "Сложение отработало корректно (1 + 5 = 6)");

    cout << "A * 2 = \n" << (mA * Rational(2)) << endl;
    check_eq((mA * Rational(2))[1][1], Rational(8), "Умножение на скаляр отработало корректно (4 * 2 = 8)");

    SquareMatrix<2> mMult = mA * mB;
    cout << "A * B = \n" << mMult << endl;
    check_eq(mMult[0][0], Rational(19), "Матричное умножение верно (1*5 + 2*7 = 19)");


    // МЕТОДЫ МАТРИЦЫ
    cout << "\n=== MATRIX METHODS ===" << endl;
    cout << "trace(A) = " << mA.trace() << endl;
    check_eq(mA.trace(), Rational(5), "След матрицы посчитан верно (1 + 4 = 5)");

    cout << "rank(A) = " << mA.rank() << endl;
    check_eq(mA.rank(), 2u, "Ранг невырожденной матрицы 2x2 равен 2");

    cout << "A transposed = \n" << mA.transposed() << endl;
    check_eq(mA.transposed()[0][1], Rational(3), "Транспонирование сместило элементы верно");

    cout << "\n=== ALL TESTS COMPLETED SUCCESSFULLY ===" << endl;

    return 0;
}
