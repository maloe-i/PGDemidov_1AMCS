#pragma once

#include <iostream>

// Константа базы для длинной арифметики
const int BASE = 1000 * 1000 * 1000;

// Опережающее объявление класса для использования во friend-связях
class BigRational;

class BigInteger {
    friend class BigRational;
    friend BigInteger gcd(BigInteger a, BigInteger b);

    friend std::istream& operator>>(std::istream& is, BigRational& br);

private:
    int* digits;      // Динамический массив цифр
    size_t size_;     // Текущее количество цифр (ячеек)
    size_t capacity_; // Текущая вместимость массива
    bool is_negative; // Знак числа

    // Вспомогательные приватные методы (реализация будет в .cpp)
    void ensure_capacity(size_t min_capacity);
    void push_back(int val);
    void trim();
    void clear_memory();
    void copy_from(const BigInteger& other);
    int absCompare(const BigInteger& other) const;

public:
    // Конструкторы и деструктор
    BigInteger();
    BigInteger(long long n);
    BigInteger(const BigInteger& other);
    BigInteger& operator=(const BigInteger& other);
    ~BigInteger();

    // Приведение типов и конвертация
    explicit operator bool() const;
    char* toString() const;
    BigInteger abs() const;

    // Операторы сравнения
    bool operator==(const BigInteger& other) const;
    bool operator!=(const BigInteger& other) const;
    bool operator<(const BigInteger& other) const;
    bool operator>(const BigInteger& other) const;
    bool operator<=(const BigInteger& other) const;
    bool operator>=(const BigInteger& other) const;

    // Арифметика
    BigInteger operator-() const;
    BigInteger& operator+=(const BigInteger& other);
    BigInteger operator+(const BigInteger& other) const;
    BigInteger& operator-=(const BigInteger& other);
    BigInteger operator-(const BigInteger& other) const;
    BigInteger& operator*=(const BigInteger& other);
    BigInteger operator*(const BigInteger& other) const;
    BigInteger& operator/=(const BigInteger& other);
    BigInteger operator/(const BigInteger& other) const;
    BigInteger& operator%=(const BigInteger& other);
    BigInteger operator%(const BigInteger& other) const;

    // Инкременты и декременты
    BigInteger& operator++();   // Префиксный
    BigInteger operator++(int); // Постфиксный
    BigInteger& operator--();
    BigInteger operator--(int);

    // Ввод / Вывод
    friend std::ostream& operator<<(std::ostream& os, const BigInteger& bi);
    friend std::istream& operator>>(std::istream& is, BigInteger& bi);
};

// Функция НОД для BigInteger
BigInteger gcd(BigInteger a, BigInteger b);

class BigRational {
private:
    BigInteger numerator;   // Числитель
    BigInteger denominator; // Знаменатель

    // Сокращение дроби
    void simplify();

public:
    // Конструкторы
    BigRational();
    BigRational(int n);
    BigRational(BigInteger n);
    BigRational(BigInteger num, BigInteger den);

    // Присваивание и арифметика
    BigRational& operator+=(const BigRational& other);
    BigRational operator+(const BigRational& other) const;
    BigRational& operator-=(const BigRational& other);
    BigRational operator-(const BigRational& other) const;
    BigRational& operator*=(const BigRational& other);
    BigRational operator*(const BigRational& other) const;
    BigRational& operator/=(const BigRational& other);
    BigRational operator/(const BigRational& other) const;

    // Унарные операции
    BigRational operator-() const;
    BigRational& operator++();
    BigRational operator++(int);
    BigRational& operator--();
    BigRational operator--(int);

    // Операторы сравнения
    bool operator==(const BigRational& other) const;
    bool operator!=(const BigRational& other) const;
    bool operator<(const BigRational& other) const;
    bool operator>(const BigRational& other) const;
    bool operator<=(const BigRational& other) const;
    bool operator>=(const BigRational& other) const;

    // Конвертация
    char* toString() const;
    char* asDecimal(size_t precision = 0) const;
    explicit operator double() const;

    // Ввод / Вывод
    friend std::ostream& operator<<(std::ostream& os, const BigRational& br);
    friend std::istream& operator>>(std::istream& is, BigRational& br);
};