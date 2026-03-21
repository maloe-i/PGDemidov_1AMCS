#include <iostream>
#include <algorithm>
#include <windows.h>
#include "BigInteger.h"

// Класс для работы с перестановками в алгебраическом смысле.
// Перестановка представлена биекцией множества {0, 1, ..., n-1} на себя.
class Permutation {
private:
    unsigned int n;       // Размер перестановки (количество элементов)
    unsigned int* arr;    // Динамический массив для хранения элементов

    // Вспомогательный метод для вычисления факториала (k!).
    // Используется как базис для факториальной системы счисления
    // при вычислении лексикографических номеров.
    static BigInteger factorial(unsigned int k) {
        BigInteger res(1);
        for (unsigned int i = 2; i <= k; ++i) {
            res = res * BigInteger(i);
        }
        return res;
    }

public:
    // ЗАДАНИЕ 1

    // Создание тождественной перестановки: {0, 1, ..., n-1}
    Permutation(unsigned int size) : n(size) {
        arr = new unsigned int[n];
        for (unsigned int i = 0; i < n; ++i) {
            arr[i] = i;
        }
    }

    // Создание перестановки на основе существующего массива
    Permutation(unsigned int size, const unsigned int* input_arr) : n(size) {
        arr = new unsigned int[n];
        for (unsigned int i = 0; i < n; ++i) {
            arr[i] = input_arr[i];
        }
    }

    // Конструктор копирования
    Permutation(const Permutation& other) : n(other.n) {
        arr = new unsigned int[n];
        std::copy(other.arr, other.arr + n, arr);
    }

    // Оператор присваивания
    Permutation& operator=(const Permutation& other) {
        if (this != &other) {
            delete[] arr; // Очищаем старую память
            n = other.n;
            arr = new unsigned int[n];
            std::copy(other.arr, other.arr + n, arr);
        }
        return *this;
    }

    // Деструктор
    ~Permutation() {
        delete[] arr;
    }

    // ЗАДАНИЕ 2

    // Конструктор, генерирующий перестановку по её номеру.
    Permutation(unsigned int size, BigInteger lexNumber) : n(size) {
        arr = new unsigned int[n];
        bool* used = new bool[n]; // Флаги для уже задействованных элементов
        for (unsigned int i = 0; i < n; ++i) used[i] = false;

        BigInteger factN = factorial(n);

        // Защита от отрицательных номеров и нормализация по кольцу n!
        if (lexNumber < BigInteger(0)) lexNumber = BigInteger(0);
        lexNumber = lexNumber % factN;

        // Стартуем с веса старшего разряда: (n-1)!
        BigInteger currentFact = factorial(n > 0 ? n - 1 : 0);

        for (unsigned int i = 0; i < n; ++i) {
            BigInteger qBI(0);

            if (i < n - 1) {
                // Целая часть от деления — это количество свободных элементов,
                // которые нужно пропустить (индекс в пуле оставшихся).
                qBI = lexNumber / currentFact;
                lexNumber = lexNumber % currentFact; // Остаток уходит в следующие разряды

                // Понижаем порядок факториала для следующего шага
                if (n - 1 - i > 0) {
                    currentFact = currentFact / BigInteger(n - 1 - i);
                }
            } else {
                qBI = BigInteger(0); // Последнему элементу достается единственный оставшийся
            }

            // Переводим BigInteger в обычный счетчик
            unsigned int skip = 0;
            while (BigInteger(skip) < qBI) {
                skip++;
            }

            // Ищем нужный элемент с учетом тех, что уже забрали на предыдущих шагах
            int count_unused = -1;
            for (unsigned int j = 0; j < n; ++j) {
                if (!used[j]) {
                    count_unused++;
                }
                if (count_unused == (int)skip) {
                    arr[i] = j;
                    used[j] = true;
                    break;
                }
            }
        }
        delete[] used;
    }

    // Вычисление лексикографического номера перестановки (Ранжирование).
    // Подсчитывает количество инверсий правее каждого элемента.
    BigInteger getLexNumber() const {
        BigInteger lexNum(0);
        BigInteger currentFact(1);

        // Итерация идет с конца, чтобы накапливать факториал множителями, а не пересчитывать i! каждый раз заново.
        for (int i = (int)n - 1; i >= 0; --i) {
            unsigned int count_smaller = 0;

            // Считаем элементы правее arr[i], которые строго меньше него
            for (unsigned int j = i + 1; j < n; ++j) {
                if (arr[j] < arr[i]) {
                    count_smaller++;
                }
            }

            // Взвешиваем инверсии на факториал текущего разряда
            lexNum = lexNum + BigInteger(count_smaller) * currentFact;

            // Наращиваем факториал для следующего (более старшего) разряда
            if (i > 0) {
                currentFact = currentFact * BigInteger(n - i);
            }
        }
        return lexNum;
    }

    // Операторы сдвига

    // Сдвиг перестановки вперед на count позиций с изменением текущего объекта
    Permutation& operator+=(int count) {
        BigInteger currentLex = getLexNumber();
        BigInteger factN = factorial(n);
        BigInteger newLex;

        if (count >= 0) {
            newLex = (currentLex + BigInteger(count)) % factN;
        } else {
            // Обработка отрицательного сдвига с учетом арифметики по модулю n!
            BigInteger offset(-count);
            newLex = currentLex - offset;

            // Если ушли в минус, добавляем n! до тех пор, пока число не станет положительным
            if (newLex < BigInteger(0)) {
                BigInteger q = offset / factN + BigInteger(1);
                newLex = newLex + q * factN;
                newLex = newLex % factN;
            }
        }

        // Пересоздаем текущий объект с помощью конструктора по номеру
        *this = Permutation(n, newLex);
        return *this;
    }

    // Сдвиг перестановки назад
    Permutation& operator-=(int count) {
        return *this += (-count);
    }

    // Операторы + и - (возвращают новый объект, не меняя текущий)
    Permutation operator+(int count) const {
        Permutation result(*this);
        result += count;
        return result;
    }

    Permutation operator-(int count) const {
        Permutation result(*this);
        result -= count;
        return result;
    }

    // Алгебраические операции и операторы отношения

    // Суперпозиция (умножение) перестановок.
    // Эквивалентно применению перестановки other, а затем текущей (this).
    Permutation operator*(const Permutation& other) const {
        Permutation result(n);
        for (unsigned int i = 0; i < n; ++i) {
            result.arr[i] = arr[other.arr[i]];
        }
        return result;
    }

    // Инкремент и декремент
    Permutation& operator++() { std::next_permutation(arr, arr + n); return *this; }
    Permutation operator++(int) { Permutation temp(*this); ++(*this); return temp; }
    Permutation& operator--() { std::prev_permutation(arr, arr + n); return *this; }
    Permutation operator--(int) { Permutation temp(*this); --(*this); return temp; }

    // Получение соседних перестановок без изменения оригинала
    Permutation next() const { Permutation res(*this); ++res; return res; }
    Permutation previous() const { Permutation res(*this); --res; return res; }

    // Операторы сравнения для лексикографического сравнения массивов
    bool operator==(const Permutation& other) const {
        if (n != other.n) return false;
        return std::equal(arr, arr + n, other.arr);
    }
    bool operator!=(const Permutation& other) const { return !(*this == other); }

    bool operator<(const Permutation& other) const {
        return std::lexicographical_compare(arr, arr + n, other.arr, other.arr + n);
    }
    bool operator>(const Permutation& other) const { return other < *this; }
    bool operator<=(const Permutation& other) const { return !(other < *this); }
    bool operator>=(const Permutation& other) const { return !(*this < other); }

    // Вывод в стандартный поток
    void print() const {
        for (unsigned int i = 0; i < n; ++i) std::cout << arr[i] << " ";
        std::cout << "\n";
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Запуск тестов Задания 1\n";

    // 1. Тест конструкторов
    Permutation id_perm(4);
    std::cout << "Тождественная перестановка (n=4): ";
    id_perm.print(); // Ожидается: 0 1 2 3

    unsigned int arrA[] = {3, 2, 1, 0};
    unsigned int arrB[] = {1, 2, 0, 3};
    Permutation pA(4, arrA);
    Permutation pB(4, arrB);

    // 2. Тест умножения
    // По заданию: result[i] = pA.arr[ pB.arr[i] ]
    Permutation pMult = pA * pB;
    std::cout << "\nУмножение pA * pB:\n";
    std::cout << "pA: "; pA.print();
    std::cout << "pB: "; pB.print();
    std::cout << "Результат: "; pMult.print(); // Ожидается: 2 1 3 0


    // 3. Тест инкремента и декремента
    unsigned int arrC[] = {0, 1, 2};
    Permutation pC(3, arrC);

    std::cout << "\nТекущая перестановка pC: "; pC.print();
    Permutation pC_next = pC.previous();
    std::cout << "next() (без изменения pC): "; pC_next.print(); // Ожидается: 1 0 2

    ++pC;
    std::cout << "После ++pC (измененная pC): "; pC.print(); // Ожидается: 1 0 2
    --pC;
    std::cout << "После --pC (возврат назад): "; pC.print(); // Ожидается: 0 2 1

    // 4. Тест операторов сравнения
    std::cout << "\nСравнение перестановок:\n";
    std::cout << "pC < pC_next: " << (pC < pC_next ? "Истина" : "Ложь") << " (Ожидается: Истина)\n";
    std::cout << "pA == pB: " << (pA == pB ? "Истина" : "Ложь") << " (Ожидается: Ложь)\n";
    std::cout << "pA != pB: " << (pA != pB ? "Истина" : "Ложь") << " (Ожидается: Истина)\n";

    std::cout << "\nЗапуск тестов Задания 2\n";

    // 1. Тест метода getLexNumber()
    unsigned int arr1[] = {1, 2, 0};
    Permutation p1(3, arr1);
    BigInteger lex1 = p1.getLexNumber();

    std::cout << "Перестановка: ";
    p1.print();
    std::cout << "Ожидаемый номер: 3 | Фактический: " << lex1 << "\n\n";

    // 2. Тест конструктора по лексикографическому номеру
    BigInteger targetLex(4);
    Permutation p2(3, targetLex);

    std::cout << "Создание перестановки n=3 по номеру 4:\n";
    std::cout << "Ожидается: 2 0 1 | Фактически: ";
    p2.print();
    std::cout << "\n";

    // 3. Тест операторов + и - (без изменения состояния объекта)
    Permutation p_plus = p1 + 2;
    std::cout << "Тест оператора + (p1 + 2):\n";
    std::cout << "Ожидается: 2 1 0 | Фактически: ";
    p_plus.print();
    std::cout << "\n";

    // 4. Тест операторов += и -= (с изменением состояния)
    Permutation p3(3, arr1); // {1, 2, 0}, номер 3
    p3 -= 5;
    std::cout << "Тест оператора -= с переходом через ноль (номер 3 -= 5):\n";
    std::cout << "Ожидается: 2 1 0 | Фактически: ";
    p3.print();
    std::cout << "\n";

    return 0;
}
