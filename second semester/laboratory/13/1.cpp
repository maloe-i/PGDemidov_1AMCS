#include <iostream>
#include <functional>
#include <stdexcept>
#include <utility>
#include <iterator>
#include <type_traits>
#include <string>
#include <cassert>
#include <memory>
#include <windows.h>

// Функтор по умолчанию для вычисления индекса (линейное пробирование)
struct LinearProbing {
    unsigned int operator()(unsigned int keyHash, unsigned int iterationNumber, unsigned int tableSize) const {
        return (keyHash + iterationNumber) % tableSize;
    }
};

template <
    size_t SIZE,
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename Equal = std::equal_to<Key>,
    typename GetIndex = LinearProbing
>
class HashMap {
public:
    using key_type = Key;
    using value_type = Value;

    // Предварительное объявление итератора
    template <bool IsConst>
    class Iterator;

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

private:
    enum class State { EMPTY, OCCUPIED, DELETED };

    // Узел хранит состояние, саму пару (размещенную через union для обхода инициализации)
    // и указатели для поддержания O(1) обхода итераторами.
    struct Node {
        using PairType = std::pair<const Key, Value>;

        union {
            PairType kv;
        };

        Node* prev = nullptr;
        Node* next = nullptr;
        State state = State::EMPTY;

        Node() : state(State::EMPTY) {}

        ~Node() {
            if (state == State::OCCUPIED) {
                std::destroy_at(&kv); // Заменили kv.~PairType()
            }
        }

        // Запрещаем случайное копирование узлов
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
    };

    Node* table;          // Массив размера SIZE + 1 (последний элемент — фиктивный sentinel)
    Node* sentinel;       // Фиктивный элемент для зацикливания списка (он же end())
    size_t elements_count;

    // Вспомогательная функция для вставки узла в конец двусвязного списка
    void link_node(Node* node) {
        node->prev = sentinel->prev;
        node->next = sentinel;
        sentinel->prev->next = node;
        sentinel->prev = node;
    }

    // Вспомогательная функция для удаления узла из двусвязного списка
    void unlink_node(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->prev = nullptr;
        node->next = nullptr;
    }

public:
    // 1. Конструктор, фиксирующий размер
    explicit HashMap() : elements_count(0) {
        // Выделяем SIZE + 1, чтобы индекс SIZE служил барьером (sentinel)
        table = new Node[SIZE + 1];
        sentinel = &table[SIZE];
        sentinel->prev = sentinel;
        sentinel->next = sentinel;
    }

    // 2. Конструктор копирования
    HashMap(const HashMap& other) : elements_count(other.elements_count) {
        table = new Node[SIZE + 1];
        sentinel = &table[SIZE];
        sentinel->prev = sentinel;
        sentinel->next = sentinel;

        // Копируем данные массива
        for (size_t i = 0; i < SIZE; ++i) {
            table[i].state = other.table[i].state;
            if (other.table[i].state == State::OCCUPIED) {
                // Размещающий new (placement new) для инициализации пары внутри union
                new (&table[i].kv) Node::PairType(other.table[i].kv);
            }
        }

        // Восстанавливаем связи двусвязного списка, вычисляя смещения в массиве (O(1) на узел)
        for (size_t i = 0; i <= SIZE; ++i) {
            if (other.table[i].prev != nullptr) {
                size_t prev_idx = other.table[i].prev - other.table;
                table[i].prev = &table[prev_idx];
            }
            if (other.table[i].next != nullptr) {
                size_t next_idx = other.table[i].next - other.table;
                table[i].next = &table[next_idx];
            }
        }
    }

    // 2. Оператор присваивания (Copy-and-Swap idiom)
    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            HashMap tmp(other);
            swap(tmp);
        }
        return *this;
    }

    ~HashMap() {
        clear();
        delete[] table;
    }

    // 3. Метод swap (работает за строгие O(1), меняя указатели)
    void swap(HashMap& other) noexcept {
        std::swap(table, other.table);
        std::swap(sentinel, other.sentinel);
        std::swap(elements_count, other.elements_count);
    }

    // 6. Методы empty, size, clear()
    bool empty() const { return elements_count == 0; }
    size_t size() const { return elements_count; }


    void clear() {
        for (size_t i = 0; i < SIZE; ++i) {
            if (table[i].state == State::OCCUPIED) {
                std::destroy_at(&table[i].kv); // Заменили table[i].kv.~PairType()
            }
            table[i].state = State::EMPTY;
            table[i].prev = nullptr;
            table[i].next = nullptr;
        }
        sentinel->prev = sentinel;
        sentinel->next = sentinel;
        elements_count = 0;
    }

    // 4. Оператор []
    Value& operator[](const Key& key) {
        unsigned int hash_val = Hash{}(key);
        unsigned int iteration = 0;
        GetIndex get_idx;
        Equal is_equal;

        unsigned int first_available = SIZE;

        while (iteration < SIZE) {
            unsigned int idx = get_idx(hash_val, iteration, SIZE);

            if (table[idx].state == State::EMPTY) {
                if (first_available == SIZE) first_available = idx;
                break;
            } else if (table[idx].state == State::OCCUPIED) {
                if (is_equal(table[idx].kv.first, key)) {
                    return table[idx].kv.second;
                }
            } else if (table[idx].state == State::DELETED) {
                if (first_available == SIZE) {
                    first_available = idx;
                }
            }
            iteration++;
        }

        if (first_available == SIZE) {
            throw std::out_of_range("HashMap is full");
        }

        Node& node = table[first_available];
        new (&node.kv) Node::PairType(key, Value());
        node.state = State::OCCUPIED;

        link_node(&node);
        ++elements_count;

        return node.kv.second;
    }

    // 5. Метод at
    Value& at(const Key& key) {
        iterator it = find(key);
        if (it == end()) throw std::out_of_range("Key not found");
        return it->second;
    }

    const Value& at(const Key& key) const {
        const_iterator it = find(key);
        if (it == end()) throw std::out_of_range("Key not found");
        return it->second;
    }

    // 7. Метод erase
    bool erase(const Key& key) {
        unsigned int hash_val = Hash{}(key);
        unsigned int iteration = 0;
        GetIndex get_idx;
        Equal is_equal;

        while (iteration < SIZE) {
            unsigned int idx = get_idx(hash_val, iteration, SIZE);

            if (table[idx].state == State::EMPTY) {
                return false;
            } else if (table[idx].state == State::OCCUPIED) {
                if (is_equal(table[idx].kv.first, key)) {
                    std::destroy_at(&table[idx].kv); // Заменили table[idx].kv.~PairType()
                    table[idx].state = State::DELETED;

                    unlink_node(&table[idx]);
                    --elements_count;
                    return true;
                }
            }
            iteration++;
        }
        return false;
    }

    // 11. Метод find
    iterator find(const Key& key) {
        unsigned int hash_val = Hash{}(key);
        unsigned int iteration = 0;
        GetIndex get_idx;
        Equal is_equal;

        while (iteration < SIZE) {
            unsigned int idx = get_idx(hash_val, iteration, SIZE);

            if (table[idx].state == State::EMPTY) {
                break;
            } else if (table[idx].state == State::OCCUPIED) {
                if (is_equal(table[idx].kv.first, key)) {
                    return iterator(&table[idx]);
                }
            }
            iteration++;
        }
        return end();
    }

    const_iterator find(const Key& key) const {
        unsigned int hash_val = Hash{}(key);
        unsigned int iteration = 0;
        GetIndex get_idx;
        Equal is_equal;

        while (iteration < SIZE) {
            unsigned int idx = get_idx(hash_val, iteration, SIZE);

            if (table[idx].state == State::EMPTY) {
                break;
            } else if (table[idx].state == State::OCCUPIED) {
                if (is_equal(table[idx].kv.first, key)) {
                    return const_iterator(&table[idx]);
                }
            }
            iteration++;
        }
        return end();
    }

    // 10. Методы begin, end
    iterator begin() { return iterator(sentinel->next); }
    const_iterator begin() const { return const_iterator(sentinel->next); }
    const_iterator cbegin() const { return const_iterator(sentinel->next); }

    iterator end() { return iterator(sentinel); }
    const_iterator end() const { return const_iterator(sentinel); }
    const_iterator cend() const { return const_iterator(sentinel); }

    // 8. Итератор
    template <bool IsConst>
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<const Key, Value>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
        using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
        using node_ptr = std::conditional_t<IsConst, const Node*, Node*>;

    private:
        node_ptr current;

        friend class HashMap;
        template <bool> friend class Iterator;

        explicit Iterator(node_ptr node) : current(node) {}

    public:
        Iterator() : current(nullptr) {}

        // Конструктор преобразования (позволяет неконстантному итератору стать константным)
        template <bool WasConst = IsConst, typename = std::enable_if_t<IsConst && !WasConst>>
        Iterator(const Iterator<false>& other) : current(other.current) {}

        reference operator*() const { return current->kv; }
        pointer operator->() const { return &current->kv; }

        Iterator& operator++() {
            current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            current = current->next;
            return tmp;
        }

        Iterator& operator--() {
            current = current->prev;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            current = current->prev;
            return tmp;
        }

        template <bool OtherConst>
        bool operator==(const Iterator<OtherConst>& other) const {
            return current == other.current;
        }

        template <bool OtherConst>
        bool operator!=(const Iterator<OtherConst>& other) const {
            return current != other.current;
        }
    };
};


int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "Начинаем тестирование HashMap..." << std::endl;

    // 1. Конструктор (размер фиксируется шаблоном)
    // 6. Методы empty, size
    HashMap<5, int, std::string> map;
    assert(map.empty());
    assert(map.size() == 0);

    // 4. Оператор [] (создание и доступ)
    map[1] = "Один";
    map[2] = "Два";
    map[6] = "Шесть (коллизия с 1, если хэш по модулю 5)";

    assert(map.size() == 3);
    assert(!map.empty());
    assert(map[1] == "Один");

    // 5. Метод at (с исключением)
    assert(map.at(2) == "Два");
    try {
        map.at(99);
        assert(false); // Сюда мы дойти не должны
    } catch (const std::out_of_range& e) {
        // Ожидаемое поведение
    }

    // 11. Метод find
    auto it_found = map.find(6);
    assert(it_found != map.end());
    assert(it_found->second == "Шесть (коллизия с 1, если хэш по модулю 5)");

    auto it_not_found = map.find(42);
    assert(it_not_found == map.end());

    // 8 & 10. Итераторы (bidirectional, *, ->, ++, --)
    std::cout << "Прямой обход итераторами:\n";
    size_t count = 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << "Ключ: " << it->first << ", Значение: " << it->second << "\n";
        count++;
    }
    assert(count == 3);

    std::cout << "Обратный обход итераторами:\n";
    if (!map.empty()) {
        auto rev_it = map.end();
        --rev_it;
        for (;;) {
            std::cout << "Ключ: " << rev_it->first << ", Значение: " << rev_it->second << "\n";
            if (rev_it == map.begin()) break;
            --rev_it;
        }
    }

    // 2. Конструктор копирования
    HashMap<5, int, std::string> map_copy = map;
    assert(map_copy.size() == 3);
    assert(map_copy.at(1) == "Один");

    // 2. Оператор присваивания
    HashMap<5, int, std::string> map_assigned;
    map_assigned = map;
    assert(map_assigned.size() == 3);
    assert(map_assigned.at(2) == "Два");

    // 3. Метод swap (за O(1))
    HashMap<5, int, std::string> map_swap;
    map_swap[99] = "Девяносто девять";
    map_swap.swap(map_assigned);

    assert(map_swap.size() == 3);
    assert(map_swap.at(6) == "Шесть (коллизия с 1, если хэш по модулю 5)");
    assert(map_assigned.size() == 1);
    assert(map_assigned.at(99) == "Девяносто девять");

    // 7. Метод erase
    bool erased = map.erase(2);
    assert(erased == true);
    assert(map.size() == 2);
    assert(map.find(2) == map.end());

    bool erased_not_exist = map.erase(999);
    assert(erased_not_exist == false);

    // 6. Метод clear
    map.clear();
    assert(map.empty());
    assert(map.size() == 0);
    assert(map.begin() == map.end()); // Итераторы указывают на sentinel

    std::cout << "Все тесты успешно пройдены, мой господин!" << std::endl;
    return 0;
}
