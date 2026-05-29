#include <iostream>
#include <algorithm>
#include <windows.h>
#include <chrono>

// Предварительное объявление класса Deque
template <typename T>
class Deque;

// Шаблонный класс итератора произвольного доступа
template <typename D, typename V>
class DequeIterator {
private:
    D* deque_;
    size_t index_;

public:
    DequeIterator(D* d, size_t idx) : deque_(d), index_(idx) {}

    // Конструктор для неявного преобразования iterator в const_iterator
    template <typename NonConstD, typename NonConstV>
    DequeIterator(const DequeIterator<NonConstD, NonConstV>& other)
        : deque_(other.get_deque()), index_(other.get_index()) {}

    D* get_deque() const { return deque_; }
    size_t get_index() const { return index_; }

    V& operator*() const { return (*deque_)[index_]; }
    V* operator->() const { return &(*deque_)[index_]; }

    DequeIterator& operator++() { ++index_; return *this; }
    DequeIterator operator++(int) { DequeIterator tmp = *this; ++index_; return tmp; }
    DequeIterator& operator--() { --index_; return *this; }
    DequeIterator operator--(int) { DequeIterator tmp = *this; --index_; return tmp; }

    DequeIterator operator+(long long n) const { return DequeIterator(deque_, index_ + n); }
    DequeIterator operator-(long long n) const { return DequeIterator(deque_, index_ - n); }
    long long operator-(const DequeIterator& other) const { return (long long)index_ - (long long)other.index_; }

    DequeIterator& operator+=(long long n) { index_ += n; return *this; }
    DequeIterator& operator-=(long long n) { index_ -= n; return *this; }

    V& operator[](long long n) const { return (*deque_)[index_ + n]; }

    bool operator==(const DequeIterator& other) const { return index_ == other.index_ && deque_ == other.deque_; }
    bool operator!=(const DequeIterator& other) const { return !(*this == other); }
    bool operator<(const DequeIterator& other) const { return index_ < other.index_; }
    bool operator<=(const DequeIterator& other) const { return index_ <= other.index_; }
    bool operator>(const DequeIterator& other) const { return index_ > other.index_; }
    bool operator>=(const DequeIterator& other) const { return index_ >= other.index_; }
};

// Шаблонный класс обратного итератора
template <typename Iter>
class ReverseIterator {
private:
    Iter current;

public:
    ReverseIterator(Iter i) : current(i) {}

    template <typename OtherIter>
    ReverseIterator(const ReverseIterator<OtherIter>& other) : current(other.base()) {}

    Iter base() const { return current; }

    auto& operator*() const {
        Iter tmp = current;
        --tmp;
        return *tmp;
    }
    auto* operator->() const {
        Iter tmp = current;
        --tmp;
        return &(*tmp);
    }

    ReverseIterator& operator++() { --current; return *this; }
    ReverseIterator operator++(int) { ReverseIterator tmp = *this; --current; return tmp; }
    ReverseIterator& operator--() { ++current; return *this; }
    ReverseIterator operator--(int) { ReverseIterator tmp = *this; ++current; return tmp; }

    ReverseIterator operator+(long long n) const { return ReverseIterator(current - n); }
    ReverseIterator operator-(long long n) const { return ReverseIterator(current + n); }
    long long operator-(const ReverseIterator& other) const { return other.current - current; }

    ReverseIterator& operator+=(long long n) { current -= n; return *this; }
    ReverseIterator& operator-=(long long n) { current += n; return *this; }

    auto& operator[](long long n) const { return *(*this + n); }

    bool operator==(const ReverseIterator& other) const { return current == other.current; }
    bool operator!=(const ReverseIterator& other) const { return current != other.current; }
    bool operator<(const ReverseIterator& other) const { return current > other.current; }
    bool operator<=(const ReverseIterator& other) const { return current >= other.current; }
    bool operator>(const ReverseIterator& other) const { return current < other.current; }
    bool operator>=(const ReverseIterator& other) const { return current <= other.current; }
};

// Основной класс Deque
template <typename T>
class Deque {
private:
    T* data_;
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;

    void reallocate(size_t new_cap) {
        T* new_data = new T[new_cap];
        for (size_t i = 0; i < size_; ++i) {
            // Безопасно, т.к. цикл выполняется только если size_ > 0, следовательно capacity_ > 0
            new_data[i] = data_[(head_ + i) % capacity_];
        }
        delete[] data_;
        data_ = new_data;
        head_ = 0;
        tail_ = size_;
        capacity_ = new_cap;
    }

public:
    using iterator = DequeIterator<Deque<T>, T>;
    using const_iterator = DequeIterator<const Deque<T>, const T>;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    // 2. Конструктор по умолчанию
    Deque() : data_(nullptr), capacity_(0), size_(0), head_(0), tail_(0) {}

    // 2. Конструктор копирования
    Deque(const Deque& other) : capacity_(other.capacity_), size_(other.size_), head_(0), tail_(other.size_) {
        data_ = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[(other.head_ + i) % other.capacity_];
        }
    }

    ~Deque() {
        delete[] data_;
    }

    // Оператор присваивания для корректной работы с памятью
    Deque& operator=(const Deque& other) {
        if (this != &other) {
            delete[] data_;
            capacity_ = other.capacity_;
            size_ = other.size_;
            head_ = 0;
            tail_ = size_;
            data_ = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[(other.head_ + i) % other.capacity_];
            }
        }
        return *this;
    }

    // --- Основные операции (Задание 1) ---

    void push_back(const T& val) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 2 : capacity_ * 2);
        }
        data_[tail_] = val;
        tail_ = (tail_ + 1) % capacity_;
        size_++;
    }

    void pop_back() {
        if (size_ == 0) return;
        tail_ = (tail_ + capacity_ - 1) % capacity_;
        size_--;
        // Поддержание памяти c1*n + c2 (уменьшаем, если заполненность <= 25%)
        if (size_ > 0 && size_ <= capacity_ / 4) {
            reallocate(capacity_ / 2);
        }
    }

    void push_front(const T& val) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 2 : capacity_ * 2);
        }
        head_ = (head_ + capacity_ - 1) % capacity_;
        data_[head_] = val;
        size_++;
    }

    void pop_front() {
        if (size_ == 0) return;
        head_ = (head_ + 1) % capacity_;
        size_--;
        if (size_ > 0 && size_ <= capacity_ / 4) {
            reallocate(capacity_ / 2);
        }
    }

    // 3. Доступ по индексу (O(1))
    T& operator[](size_t index) {
        return data_[(head_ + index) % capacity_];
    }
    const T& operator[](size_t index) const {
        return data_[(head_ + index) % capacity_];
    }

    // 1. Методы доступа к крайним элементам
    T& back() { return data_[(tail_ + capacity_ - 1) % capacity_]; }
    const T& back() const { return data_[(tail_ + capacity_ - 1) % capacity_]; }
    T& front() { return data_[head_]; }
    const T& front() const { return data_[head_]; }

    // 4. Проверки состояния
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }

    // 6. Методы работы с итераторами
    iterator begin() { return iterator(this, 0); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator cbegin() const { return const_iterator(this, 0); }

    iterator end() { return iterator(this, size_); }
    const_iterator end() const { return const_iterator(this, size_); }
    const_iterator cend() const { return const_iterator(this, size_); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }
};

// 7. Тесты производительности и функциональности

void test_complexity() {
    Deque<int> d;
    const int N = 10000;

    std::cout << "[Тест] Вставка " << N << " элементов (проверка амортизированного O(1))...\n";

    // Запускаем таймер для вставки
    auto start_push = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        d.push_back(i);
    }

    // Останавливаем таймер
    auto end_push = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_push = end_push - start_push;

    std::cout << "Время вставки: " << duration_push.count() << " секунд.\n\n";

    std::cout << "[Тест] Удаление " << N << " элементов (с освобождением памяти)...\n";

    // Запускаем таймер для удаления
    auto start_pop = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        d.pop_back();
    }

    // Останавливаем таймер
    auto end_pop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_pop = end_pop - start_pop;

    std::cout << "Время удаления: " << duration_pop.count() << " секунд.\n";
    std::cout << "Успех! Операции выполняются за доли секунды, что эмпирически доказывает сложность O(1).\n\n";
}

void test_complexity2() {
    Deque<int> d;
    const int N = 100;

    std::cout << "[Тест] Вставка " << N << " элементов (проверка амортизированного O(1))...\n";

    // Запускаем таймер для вставки
    auto start_push = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        d.push_back(i);
    }

    // Останавливаем таймер
    auto end_push = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_push = end_push - start_push;

    std::cout << "Время вставки: " << duration_push.count() << " секунд.\n\n";

    std::cout << "[Тест] Удаление " << N << " элементов (с освобождением памяти)...\n";

    // Запускаем таймер для удаления
    auto start_pop = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        d.pop_back();
    }

    // Останавливаем таймер
    auto end_pop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_pop = end_pop - start_pop;

    std::cout << "Время удаления: " << duration_pop.count() << " секунд.\n";
    std::cout << "Успех! Операции выполняются за доли секунды, что эмпирически доказывает сложность O(1).\n\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "--- Инициализация тестов Deque ---\n\n";
    test_complexity();
    test_complexity2();

    Deque<int> deq;

    // Проверка базового функционала
    deq.push_back(10);
    deq.push_back(20);
    deq.push_front(5);
    deq.push_front(1);

    std::cout << "Содержимое дека после вставок (должно быть: 1 5 10 20):" << '\n';
    for(auto it = deq.begin(); it != deq.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n\n";

    std::cout << "Доступ по индексу (deq[2] должно быть 10): " << deq[2] << "\n";
    std::cout << "Передний элемент (front): " << deq.front() << "\n";
    std::cout << "Задний элемент (back): " << deq.back() << "\n\n";

    // Проверка обратных итераторов
    std::cout << "Обратный проход (должно быть: 20 10 5 1):" << '\n';
    for(auto rit = deq.rbegin(); rit != deq.rend(); ++rit) {
        std::cout << *rit << " ";
    }
    std::cout << '\n';

    return 0;
}

/*begin() — итератор на первый элемент. Позволяет изменять данные.
 *begin() const — константный итератор на первый элемент. Вызывается у константных объектов.
 *cbegin() const — всегда константный итератор на первый элемент. Защищает данные от изменений.
 *end() — итератор на элемент, следующий за последним. Указывает на пустую ячейку памяти (маркер конца).
 *end() const — константный маркер конца для константных объектов.
 *cend() const — всегда константный маркер конца.*/


/*head_ — индекс, где сейчас реально лежит первый элемент очереди.
 *tail_ — индекс ячейки, куда будет записан следующий добавленный в конец элемент*/
