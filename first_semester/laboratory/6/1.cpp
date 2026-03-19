#include <iostream>
#include <algorithm>

const int BASE = 1000 * 1000 * 1000;

class BigInteger {
    friend class BigRational;
    friend BigInteger gcd(BigInteger a, BigInteger b);
private:
    int* digits;      // Динамический массив цифр
    size_t size_;     // Текущее количество цифр (ячеек)
    size_t capacity_; // Текущая вместимость массива
    bool is_negative; // Знак числа

    // Увеличение вместимости массива при необходимости
    void ensure_capacity(size_t min_capacity) {
        if (capacity_ >= min_capacity) return;

        size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
        while (new_capacity < min_capacity) new_capacity *= 2;

        int* new_digits = new int[new_capacity]();
        for (size_t i = 0; i < size_; ++i) {
            new_digits[i] = digits[i];
        }

        if (digits) delete[] digits;
        digits = new_digits;
        capacity_ = new_capacity;
    }

    // Добавление элемента в конец
    void push_back(int val) {
        ensure_capacity(size_ + 1);
        digits[size_++] = val;
    }

    // Удаление незначащих нулей
    void trim() {
        while (size_ > 1 && digits[size_ - 1] == 0) {
            size_--;
        }
        if (size_ == 1 && digits[0] == 0) {
            is_negative = false;
        }
    }

    // Очистка
    void clear_memory() {
        if (digits) {
            delete[] digits;
            digits = nullptr;
        }
        size_ = 0;
        capacity_ = 0;
    }

    // Копирование данных из другого объекта
    void copy_from(const BigInteger& other) {
        ensure_capacity(other.size_);
        size_ = other.size_;
        is_negative = other.is_negative;
        for (size_t i = 0; i < size_; ++i) {
            digits[i] = other.digits[i];
        }
    }

    // Сравнение модулей
    int absCompare(const BigInteger& other) const {
        if (size_ > other.size_) return 1;
        if (size_ < other.size_) return -1;
        for (long long i = (long long)size_ - 1; i >= 0; --i) {
            if (digits[i] > other.digits[i]) return 1;
            if (digits[i] < other.digits[i]) return -1;
        }
        return 0;
    }

public:
    BigInteger() : digits(nullptr), size_(0), capacity_(0), is_negative(false) {
        push_back(0);
    }

    BigInteger(long long n) : digits(nullptr), size_(0), capacity_(0), is_negative(false) {
        if (n < 0) {
            is_negative = true;
            n = -n;
        }
        if (n == 0) {
            push_back(0);
        }
        while (n > 0) {
            push_back(n % BASE);
            n /= BASE;
        }
    }

    // Конструктор копирования
    BigInteger(const BigInteger& other) : digits(nullptr), size_(0), capacity_(0), is_negative(false) {
        copy_from(other);
    }

    // Оператор присваивания
    BigInteger& operator=(const BigInteger& other) {
        if (this == &other) return *this;
        // Оптимизация.
        if (capacity_ < other.size_) {
            clear_memory();
        }
        copy_from(other);
        return *this;
    }

    // Деструктор
    ~BigInteger() {
        clear_memory();
    }

    // Неявное преобразование в bool.
    explicit operator bool() const {
        return !(size_ == 1 && digits[0] == 0);
    }

    // Метод toString().
    char* toString() const {
        if (size_ == 0) return nullptr;

        // Расчет максимальной длины: знак + (9 цифр * количество блоков) + терминирующий нуль
        size_t max_len = 1 + size_ * 10 + 1;
        char* buffer = new char[max_len];
        int pos = 0;

        if (is_negative && !(*this == BigInteger(0))) {
            buffer[pos++] = '-';
        }

        int top = digits[size_ - 1];
        // Ручная конвертация int в string
        if (top == 0) {
            buffer[pos++] = '0';
        } else {
            char temp[12];
            int t_pos = 0;
            while (top > 0) {
                temp[t_pos++] = '0' + (top % 10);
                top /= 10;
            }
            while (t_pos > 0) {
                buffer[pos++] = temp[--t_pos];
            }
        }

        // Печатаем остальные блоки
        for (long long i = (long long)size_ - 2; i >= 0; --i) {
            int val = digits[i];
            char temp[10]; // 9 цифр
            for (int k = 0; k < 9; ++k) {
                temp[k] = '0' + (val % 10);
                val /= 10;
            }
            for (int k = 8; k >= 0; --k) {
                buffer[pos++] = temp[k];
            }
        }

        buffer[pos] = '\0';
        return buffer;
    }

    // Операторы сравнения

    bool operator==(const BigInteger& other) const {
        if (is_negative != other.is_negative) return false;
        if (size_ != other.size_) return false;
        for (size_t i = 0; i < size_; ++i) {
            if (digits[i] != other.digits[i]) return false;
        }
        return true;
    }
    bool operator!=(const BigInteger& other) const { return !(*this == other); }
    bool operator<(const BigInteger& other) const {
        if (is_negative != other.is_negative) return is_negative;
        if (!is_negative) return absCompare(other) == -1;
        return absCompare(other) == 1;
    }
    bool operator>(const BigInteger& other) const { return other < *this; }
    bool operator<=(const BigInteger& other) const { return !(*this > other); }
    bool operator>=(const BigInteger& other) const { return !(*this < other); }

    // Арифметика

    BigInteger operator-() const {
        if (size_ == 1 && digits[0] == 0) return *this;
        BigInteger res = *this;
        res.is_negative = !res.is_negative;
        return res;
    }

    BigInteger& operator+=(const BigInteger& other) {
        if (is_negative == other.is_negative) {
            // Одинаковые знаки - сложение модулей
            size_t max_s = std::max(size_, other.size_);
            ensure_capacity(max_s + 1);
            long long carry = 0;
            for (size_t i = 0; i < max_s || carry; ++i) {
                long long current = carry;
                if (i < size_) current += digits[i];
                if (i < other.size_) current += other.digits[i];
                if (i >= size_) {
                    push_back(current % BASE);
                } else {
                    digits[i] = current % BASE;
                }
                carry = current / BASE;
            }
        } else {
            // Разные знаки - вычитание модулей
            int cmp = absCompare(other);
            if (cmp == 0) {
                // Модули равны - результат 0
                clear_memory();
                push_back(0);
                is_negative = false;
            } else if (cmp > 0) {
                // |this| > |other| - вычитаем из текущего
                long long borrow = 0;
                for (size_t i = 0; i < size_; ++i) {
                    long long current = digits[i] - borrow;
                    if (i < other.size_) current -= other.digits[i];
                    if (current < 0) {
                        current += BASE;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    digits[i] = current;
                }
                trim();
            } else {
                // |this| < |other| - меняем знак и вычитаем
                BigInteger temp = other;
                long long borrow = 0;
                // Вычитаем this из other
                for (size_t i = 0; i < temp.size_; ++i) {
                    long long current = temp.digits[i] - borrow;
                    if (i < size_) current -= digits[i];
                    if (current < 0) {
                        current += BASE;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    temp.digits[i] = current;
                }
                temp.trim();
                *this = temp;
            }
        }
        return *this;
    }

    BigInteger operator+(const BigInteger& other) const {
        BigInteger res = *this;
        res += other;
        return res;
    }

    BigInteger& operator-=(const BigInteger& other) {
        *this += (-other);
        return *this;
    }

    BigInteger operator-(const BigInteger& other) const {
        BigInteger res = *this;
        res -= other;
        return res;
    }

    BigInteger& operator*=(const BigInteger& other) {
        BigInteger res;
        // Подготовка размера
        size_t new_size = size_ + other.size_;
        res.ensure_capacity(new_size);
        // Заполняем нулями, так как ensure_capacity не инициализирует значения
        for (size_t i = 0; i < new_size; ++i) res.push_back(0);
        res.is_negative = is_negative != other.is_negative;

        for (size_t i = 0; i < size_; ++i) {
            long long carry = 0;
            for (size_t j = 0; j < other.size_ || carry; ++j) {
                long long current = res.digits[i + j] +
                                    (long long)digits[i] * (j < other.size_ ? other.digits[j] : 0) + carry;
                res.digits[i + j] = current % BASE;
                carry = current / BASE;
            }
        }
        res.trim();
        *this = res;
        return *this;
    }

    BigInteger operator*(const BigInteger& other) const {
        BigInteger res = *this;
        res *= other;
        return res;
    }

    BigInteger& operator/=(const BigInteger& other) {
        if (other == BigInteger(0)) {
            // Деление на 0
            std::cerr << "Error: Division by zero\n";
            clear_memory();
            push_back(0);
            return *this;
        }

        BigInteger absA = *this; absA.is_negative = false;
        BigInteger absB = other; absB.is_negative = false;

        if (absA < absB) {
            clear_memory();
            push_back(0);
            return *this;
        }

        BigInteger res;
        res.ensure_capacity(size_);
        for (size_t k = 0; k < size_; ++k) res.push_back(0);
        res.is_negative = is_negative != other.is_negative;

        BigInteger current;
        for (long long i = (long long)size_ - 1; i >= 0; --i) {
            // Сдвиг current влево: current = current * BASE
            if (!(current.size_ == 1 && current.digits[0] == 0)) {
                current.ensure_capacity(current.size_ + 1);
                for (long long k = current.size_; k > 0; --k) {
                    current.digits[k] = current.digits[k - 1];
                }
                current.digits[0] = 0;
                current.size_++;
            }
            
            // Добавляем текущую цифру A
            current.digits[0] = digits[i];

            // Бинарный поиск
            int l = 0, r = BASE;
            int x = 0;
            while (l <= r) {
                int m = (l + r) / 2;
                if (absB * BigInteger(m) <= current) {
                    x = m;
                    l = m + 1;
                } else {
                    r = m - 1;
                }
            }
            res.digits[i] = x;
            current -= absB * BigInteger(x);
        }
        res.trim();
        *this = res;
        return *this;
    }

    BigInteger operator/(const BigInteger& other) const {
        BigInteger res = *this;
        res /= other;
        return res;
    }

    BigInteger& operator%=(const BigInteger& other) {
        BigInteger div = *this / other;
        *this -= (div * other);
        return *this;
    }

    BigInteger operator%(const BigInteger& other) const {
        BigInteger res = *this;
        res %= other;
        return res;
    }

    // Инкременты
    BigInteger& operator++() { *this += 1; return *this; }
    BigInteger operator++(int) { BigInteger t = *this; ++(*this); return t; }
    BigInteger& operator--() { *this -= 1; return *this; }
    BigInteger operator--(int) { BigInteger t = *this; --(*this); return t; }

    // Ввод / Вывод
    friend std::ostream& operator<<(std::ostream& os, const BigInteger& bi) {
        char* s = bi.toString();
        os << s;
        delete[] s;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, BigInteger& bi) {
        char c;
        while (is.get(c) && isspace(c));
        if (!is) return is;

        // Буфер для чтения. Макс ввод 10000 символов.
        size_t buf_cap = 1024;
        size_t buf_len = 0;
        char* buffer = new char[buf_cap];

        buffer[buf_len++] = c;
        while (is.get(c) && !isspace(c)) {
            if (buf_len >= buf_cap) {
                // Расширяем буфер
                size_t new_cap = buf_cap * 2;
                char* new_buf = new char[new_cap];
                for (size_t k = 0; k < buf_len; ++k) new_buf[k] = buffer[k];
                delete[] buffer;
                buffer = new_buf;
                buf_cap = new_cap;
            }
            buffer[buf_len++] = c;
        }
        // Если считали лишний символ (пробел), вернем его в поток.
        if (is) is.unget();

        bi.clear_memory();
        bool neg = false;
        int start = 0;
        if (buffer[0] == '-') { neg = true; start = 1; }
        else if (buffer[0] == '+') { start = 1; }

        if (start == buf_len) {
            bi.push_back(0);
            delete[] buffer;
            return is;
        }

        // Сбор.
        for (int i = buf_len; i > start; i -= 9) {
            int chunk_len = (i - start < 9) ? (i - start) : 9;
            int chunk_start = i - chunk_len;
            int val = 0;
            for (int j = 0; j < chunk_len; ++j) {
                val = val * 10 + (buffer[chunk_start + j] - '0');
            }
            bi.push_back(val);
        }
        bi.is_negative = neg;
        bi.trim();
        delete[] buffer;
        return is;
    }

    // Возвращает абсолютное значение числа.
    BigInteger abs() const {
        BigInteger res = *this;
        res.is_negative = false;
        return res;
    }
};

// Функция НОД для BigInteger.
BigInteger gcd(BigInteger a, BigInteger b) {
    a = a.abs();
    b = b.abs();
    while ((bool)b) {
        a %= b;
        BigInteger temp = a; // temp хранит остаток
        a = b;               // a становится старым делителем
        b = temp;            // b становится остатком
    }
    return a;
}

class BigRational {
private:
    BigInteger numerator;   // Числитель
    BigInteger denominator; // Знаменатель

    // Сокращение дроби
    void simplify() {
        if (denominator == BigInteger(0)) {
             std::cerr << "Error: Denominator is zero.\n";
             numerator = 0;
             denominator = 1;
             return;
        }
        if (numerator == BigInteger(0)) {
            denominator = 1;
            return;
        }

        BigInteger common = gcd(numerator, denominator);
        numerator /= common;
        denominator /= common;

        // Знак всегда у числителя
        if (denominator < 0) {
            denominator = -denominator;
            numerator = -numerator;
        }
    }

public:
    // Конструкторы
    BigRational() : numerator(0), denominator(1) {}
    BigRational(int n) : numerator(n), denominator(1) {}
    BigRational(BigInteger n) : numerator(n), denominator(1) {}
    BigRational(BigInteger num, BigInteger den) : numerator(num), denominator(den) {
        simplify();
    }

    // Присваивание
    BigRational& operator+=(const BigRational& other) {
        numerator = numerator * other.denominator + other.numerator * denominator;
        denominator *= other.denominator;
        simplify();
        return *this;
    }
    // Арифметические операции
    BigRational operator+(const BigRational& other) const {
        // a/b + c/d = (ad + bc) / bd
        BigRational res = *this;
        res += other;
        return res;
    }

    BigRational& operator-=(const BigRational& other) {
        numerator = numerator * other.denominator - other.numerator * denominator;
        denominator *= other.denominator;
        simplify();
        return *this;
    }
    BigRational operator-(const BigRational& other) const {
        // a/b - c/d = (ad - bc) / bd
        BigRational res = *this;
        res -= other;
        return res;
    }

    BigRational& operator*=(const BigRational& other) {
        numerator *= other.numerator;
        denominator *= other.denominator;
        simplify();
        return *this;
    }
    BigRational operator*(const BigRational& other) const {
        BigRational res = *this;
        res *= other;
        return res;
    }

    BigRational& operator/=(const BigRational& other) {
        numerator *= other.denominator;
        denominator *= other.numerator;
        simplify();
        return *this;
    }
    BigRational operator/(const BigRational& other) const {
        // (a/b) / (c/d) = (ad) / (bc)
        BigRational res = *this;
        res /= other;
        return res;
    }

    // Унарный минус, инкремент, декремент
    BigRational operator-() const {
        return BigRational(-numerator, denominator);
    }

    BigRational& operator++() { *this += 1; return *this; }
    BigRational operator++(int) { BigRational temp = *this; ++(*this); return temp; }
    BigRational& operator--() { *this -= 1; return *this; }
    BigRational operator--(int) { BigRational temp = *this; --(*this); return temp; }

    // Операторы сравнения
    // a/b == c/d <=> ad == bc
    bool operator==(const BigRational& other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }
    bool operator!=(const BigRational& other) const { return !(*this == other); }
    bool operator<(const BigRational& other) const {
        // Приводим к общему знаменателю для сравнения.
        // Знаменатели всегда положительны, знак неравенства не меняется.
        return numerator * other.denominator < other.numerator * denominator;
    }
    bool operator>(const BigRational& other) const { return other < *this; }
    bool operator<=(const BigRational& other) const { return !(*this > other); }
    bool operator>=(const BigRational& other) const { return !(*this < other); }

    // Метод toString()
    char* toString() const {
        char* numStr = numerator.toString();

        if (denominator == BigInteger(1)) {
            return numStr;
        }

        char* denStr = denominator.toString();

        // Вычисляем длину: len(num) + 1 ('/') + len(den) + 1 ('\0')
        size_t lenNum = 0; while (numStr[lenNum]) lenNum++;
        size_t lenDen = 0; while (denStr[lenDen]) lenDen++;

        char* buffer = new char[lenNum + 1 + lenDen + 1];

        // Копируем
        size_t pos = 0;
        for (size_t i = 0; i < lenNum; ++i) buffer[pos++] = numStr[i];
        buffer[pos++] = '/';
        for (size_t i = 0; i < lenDen; ++i) buffer[pos++] = denStr[i];
        buffer[pos] = '\0';

        delete[] numStr;
        delete[] denStr;
        return buffer;
    }

    // Метод asDecimal(precision)
    char* asDecimal(size_t precision = 0) const {
        // Целая часть
        BigInteger integerPart = numerator / denominator;
        BigInteger remainder = numerator % denominator;
        if (remainder < 0) remainder = -remainder; // Остаток для дробной части берем по модулю

        // Получаем строку целой части
        char* intStr = integerPart.toString();
        if (precision == 0) return intStr;

        bool isNeg = (numerator < 0);
        // Вычисляем длину
        size_t lenInt = 0; while (intStr[lenInt]) lenInt++;
        bool needMinus = (isNeg && intStr[0] != '-');

        size_t totalLen = lenInt + (needMinus ? 1 : 0) + 1 + precision + 1;
        char* buffer = new char[totalLen];

        size_t pos = 0;
        if (needMinus) buffer[pos++] = '-';
        for (size_t i = 0; i < lenInt; ++i) buffer[pos++] = intStr[i];

        buffer[pos++] = '.';

        // Дробная часть: делим столбиком
        for (size_t i = 0; i < precision; ++i) {
            remainder *= 10;
            BigInteger digit = remainder / denominator;
            buffer[pos++] = '0' + (digit.size_ > 0 ? digit.digits[0] : 0);
            remainder %= denominator;
        }
        buffer[pos] = '\0';

        delete[] intStr;
        return buffer;
    }

    // Оператор приведения к double
    explicit operator double() const {
        char* s = asDecimal(18);
        double val = 0.0;
        bool neg = false;
        int i = 0;
        if (s[0] == '-') { neg = true; i++; }

        // Целая часть
        while (s[i] != '.' && s[i] != '\0') {
            val = val * 10.0 + (s[i] - '0');
            i++;
        }

        // Дробная часть
        if (s[i] == '.') {
            i++;
            double weight = 0.1;
            while (s[i] != '\0') {
                val += (s[i] - '0') * weight;
                weight *= 0.1;
                i++;
            }
        }

        if (neg) val = -val;
        delete[] s;
        return val;
    }

    // Ввод/Вывод
    friend std::ostream& operator<<(std::ostream& os, const BigRational& br) {
        char* s = br.asDecimal(10); // ПОСЛЕ ЗАПЯТОЙ
        os << s;
        delete[] s;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, BigRational& br) {
        char c;
        while (is.get(c) && isspace(c));
        if (!is) return is;

        size_t buf_cap = 1024;
        size_t buf_len = 0;
        char* buffer = new char[buf_cap];

        buffer[buf_len++] = c;
        while (is.get(c) && !isspace(c)) {
            if (buf_len >= buf_cap) {
                size_t new_cap = buf_cap * 2;
                char* new_buf = new char[new_cap];
                for (size_t k = 0; k < buf_len; ++k) new_buf[k] = buffer[k];
                delete[] buffer;
                buffer = new_buf;
                buf_cap = new_cap;
            }
            buffer[buf_len++] = c;
        }
        if (is) is.unget();

        // Ищем точку
        int dotPos = -1;
        for (int i = 0; i < buf_len; ++i) {
            if (buffer[i] == '.') { dotPos = i; break; }
        }

        char* num_buf = new char[buf_len];
        int num_len = 0;
        int den_pow = 0;
        for (int i = 0; i < buf_len; ++i) {
            if (i == dotPos) continue;
            num_buf[num_len++] = buffer[i];
            if (dotPos != -1 && i > dotPos) den_pow++;
        }

        bool neg = false;
        int start = 0;
        if (num_buf[0] == '-') { neg = true; start = 1; }
        else if (num_buf[0] == '+') { start = 1; }

        BigInteger num(0);
        num.clear_memory();
        if (start == num_len) {
            num.push_back(0);
        } else {
            // Просто целое или Дробное (оба случая покрываются чтением цифр без точки)
            for (int i = num_len; i > start; i -= 9) {
                int chunk_len = (i - start < 9) ? (i - start) : 9;
                int chunk_start = i - chunk_len;
                int val = 0;
                for (int j = 0; j < chunk_len; ++j) {
                    val = val * 10 + (num_buf[chunk_start + j] - '0');
                }
                num.push_back(val);
            }
            num.is_negative = neg;
        }
        num.trim();

        BigInteger den(1);
        int full_blocks = den_pow / 9;
        int rem = den_pow % 9;
        den.clear_memory();
        for (int i = 0; i < full_blocks; ++i) den.push_back(0);
        int rem_val = 1;
        for (int i = 0; i < rem; ++i) rem_val *= 10;
        den.push_back(rem_val);

        br = BigRational(num, den);
        delete[] num_buf;
        delete[] buffer;
        return is;
    }
};

int main() {
    BigInteger bi1, bi2;
    std::cout << "BigInteger Test Start\n";
    if (std::cin >> bi1 >> bi2) {
        std::cout << "Read BI1: " << bi1 << "\n";
        std::cout << "Read BI2: " << bi2 << "\n";

        std::cout << "Sum: " << (bi1 + bi2) << "\n";
        std::cout << "Sub: " << (bi1 - bi2) << "\n";
        std::cout << "Mul: " << (bi1 * bi2) << "\n";

        if ((bool)bi2) { // Проверка на ноль
            std::cout << "Div: " << (bi1 / bi2) << "\n";
            std::cout << "Mod: " << (bi1 % bi2) << "\n";
        } else {
            std::cout << "Div/Mod by zero skipped.\n";
        }

        std::cout << "(>): " << (bi1 > bi2) << "\n";
        std::cout << "(<): " << (bi1 < bi2) << "\n";
        std::cout << "(==): " << (bi1 == bi2) << "\n";

        // Проверка инкремента
        BigInteger temp = bi1;
        std::cout << "Pre-inc: " << ++temp << "\n";
        std::cout << "Post-inc: " << temp++ << " (now " << temp << ")\n";
    }

    std::cout << "\nBigRational Test Start\n";
    BigRational br1, br2;
    if (std::cin >> br1 >> br2) {
        // Тест toString (в виде дроби) vs operator<< (десятичный)
        char* s1 = br1.toString();
        char* s2 = br2.toString();
        std::cout << "Read BR1: " << s1 << " (decimal: " << br1 << ")\n";
        std::cout << "Read BR2: " << s2 << " (decimal: " << br2 << ")\n";
        delete[] s1; delete[] s2;

        BigRational sum = br1 + br2;
        BigRational sub = br1 - br2;
        BigRational mul = br1 * br2;

        // Для вывода в виде дроби используем toString
        char* sSum = sum.toString();
        std::cout << "Sum (frac): " << sSum << " (dec: " << sum << ")\n";
        delete[] sSum;

        std::cout << "Sub (dec): " << sub << "\n";
        std::cout << "Mul (dec): " << mul << "\n";

        // Проверка деления и исключения деления на ноль
        if (br2 != BigRational(0)) {
            std::cout << "Div (dec): " << (br1 / br2) << "\n";
        }

        std::cout << "(>): " << (br1 > br2) << "\n";
        // Тест double cast
        std::cout << "Double cast BR1: " << (double)br1 << "\n";
    }

    return 0;
}
