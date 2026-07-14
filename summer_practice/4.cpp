#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>

using namespace std;
using namespace std::chrono;

//ifstream fin("n1000000_one_types.txt");
ifstream fin("n1000000_two_types.txt");

// a - исходный массив
// pref - массив префиксных сумм
// n - размер массива
// q - количество запросов
void PREF(vector<int>& a, vector<int>& pref, int n, int q) {
    while (q--) {
        int type;
        fin >> type;
        if (type == 1) {
            int l, r;
            fin >> l >> r;
            int sum = pref[r] - pref[l - 1];
        }
        else {
            int id, x;
            fin >> id >> x;
            int df = x - a[id - 1];
            a[id - 1] = x;
            for (int i = id; i <= n; ++i) {
                pref[i] += df;
            }
        }
    }
}

// a - исходный массив
// n - размер массива
// q - количество запросов
void SQRT(vector<int>& a, int n, int q) {
    int block_size = sqrt(n);
    int max_blocks = block_size + 10;

    vector<long long> SQRT_arr(max_blocks, 0);
    vector<int> L(max_blocks, 0);
    vector<int> R(max_blocks, 0);

    int count = 0;
    int pos = 0;
    long long current_sum = 0;

    for (int i = 0; i < n; i++) {
        count++;
        if (count > block_size) {
            current_sum = 0;
            count = 1;
            pos++;
        }

        if (count == 1) {
            L[pos] = i;
        }

        current_sum += a[i];

        if (count == block_size) {
            R[pos] = i;
            SQRT_arr[pos] = current_sum;
        }
    }

    if (count != block_size && n > 0) {
        SQRT_arr[pos] = current_sum;
        R[pos] = n - 1;
    }

    for (int i = 0; i < q; i++) {
        int type;
        fin >> type;

        if (type == 2) {
            int id, x;
            fin >> id >> x;
            id--;

            for (int j = 0; j <= pos; j++) {
                if (L[j] <= id && id <= R[j]) {
                    SQRT_arr[j] = SQRT_arr[j] - a[id] + x;
                    a[id] = x;
                    break;
                }
            }
        }
        else {
            int left, right;
            fin >> left >> right;
            left--;
            right--;

            long long sum = 0;
            int full_blocks_count = 0;

            for (int j = 0; j <= pos; j++) {
                if (left <= L[j] && R[j] <= right) {
                    full_blocks_count++;
                }
            }

            if (full_blocks_count == 0) {
                for (int j = left; j <= right; j++) {
                    sum += a[j];
                }
            }
            else {
                int pos_l = -1;
                int pos_r = -1;

                for (int j = 0; j <= pos; j++) {
                    if (left <= L[j] && R[j] <= right) {
                        sum += SQRT_arr[j];
                        pos_r = R[j] + 1;
                        if (pos_l == -1) {
                            pos_l = L[j] - 1;
                        }
                    }
                }

                for (int j = left; j <= pos_l; j++) {
                    sum += a[j];
                }
                for (int j = pos_r; j <= right; j++) {
                    sum += a[j];
                }

            }
        }
    }
}

// tree - дерево отрезков
// num - номер узла для обновления
// val - новое значение
void update_tree(vector<int>& tree, int num, int val) {
    tree[num] = val;
    while (num != 1) {
        num /= 2;
        tree[num] = tree[num * 2] + tree[num * 2 + 1];
    }
}

// tree - дерево отрезков
// id - текущий узел
// tl - левая граница текущего узла
// tr - правая граница текущего узла
// l - левая граница запроса
// r - правая граница запроса
int sum_tree(const vector<int>& tree, int id, int tl, int tr, int l, int r) {
    if (l <= tl && tr <= r) {
        return tree[id];
    }
    if (tr <= l || tl >= r) {
        return 0;
    }
    int mid = (tl + tr) / 2;
    int sum_l = sum_tree(tree, id * 2, tl, mid, l, r);
    int sum_r = sum_tree(tree, id * 2 + 1, mid + 1, tr, l, r);
    return sum_l + sum_r;
}

// Рекурсивный запрос суммы
int sum_tree_recursive(const vector<int>& tree, int id, int tl, int tr, int l, int r) {
    if (l > tr || r < tl) {
        return 0;
    }

    if (l <= tl && tr <= r) {
        return tree[id];
    }
    int mid = (tl + tr) / 2;
    int sum_l = sum_tree_recursive(tree, id*2, tl, mid, l, r);
    int sum_r = sum_tree_recursive(tree, id*2 + 1, mid + 1, tr, l, r);
    return sum_l + sum_r;
}

// a - исходный массив
// n - размер массива
// q - количество запросов
void DO(vector<int>& a, int n, int q) {
    vector<int> tree(4 * n);
    int f_list = 1, count_list = 1;
    while (count_list < n) {
        count_list *= 2;
        f_list *= 2;
    }
    for (int i = 0; i < n; i++) {
        update_tree(tree, f_list + i, a[i]);
    }
    while (q--) {
        int type;
        fin >> type;
        if (type == 2) {
            int id, x;
            fin >> id >> x;
            id--;
            update_tree(tree, f_list + id, x);
        }
        else if (type == 1) {
            int l, r;
            fin >> l >> r;
            l--;
            r--;
            int sum = sum_tree_recursive(tree, 1, 0, count_list, l, r);
        }
    }
}

int main() {
    if (!fin.is_open()) {
        cout << "Error: File not found!\n";
        return 1;
    }

    int n, q;
    fin >> n >> q;

    vector<int> original_a(n);
    for (int i = 0; i < n; ++i) {
        fin >> original_a[i];
    }

    streampos query_start = fin.tellg();

    // 1. Префиксные суммы
    vector<int> a_pref = original_a;
    vector<int> pref(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        pref[i + 1] = pref[i] + a_pref[i];
    }

    auto start = high_resolution_clock::now();
    PREF(a_pref, pref, n, q);
    auto stop = high_resolution_clock::now();
    cout << "pref: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";

    // 2. SQRT Декомпозиция
    fin.clear();
    fin.seekg(query_start);

    vector<int> a_sqrt = original_a;

    start = high_resolution_clock::now();
    SQRT(a_sqrt, n, q);
    stop = high_resolution_clock::now();
    cout << "SQRT: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";

    // 3. Дерево отрезков
    fin.clear();
    fin.seekg(query_start);

    vector<int> a_do = original_a;

    start = high_resolution_clock::now();
    DO(a_do, n, q);
    stop = high_resolution_clock::now();
    cout << "DO  : " << duration_cast<milliseconds>(stop - start).count() << "ms\n";

    fin.close();
    return 0;
}
