#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

ifstream fin("n100_one_types.txt");

vector<long long> PREF(vector<int>& a, int n, int q) {
    vector<long long> answers;
    vector<long long> pref(n + 1, 0);
    for (int i = 0; i < n; ++i) {
        pref[i + 1] = pref[i] + a[i];
    }

    while (q--) {
        int type;
        fin >> type;
        if (type == 1) {
            int l, r;
            fin >> l >> r;
            answers.push_back(pref[r] - pref[l - 1]);
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
    return answers;
}

vector<long long> SQRT(vector<int>& a, int n, int q) {
    vector<long long> answers;
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
            answers.push_back(sum);
        }
    }
    return answers;
}

void update_tree(vector<long long>& tree, int num, long long val) {
    tree[num] = val;
    while (num != 1) {
        num /= 2;
        tree[num] = tree[num * 2] + tree[num * 2 + 1];
    }
}

long long sum_tree_recursive(const vector<long long>& tree, int id, int tl, int tr, int l, int r) {
    if (l > tr || r < tl) {
        return 0;
    }
    if (l <= tl && tr <= r) {
        return tree[id];
    }
    int mid = (tl + tr) / 2;
    long long sum_l = sum_tree_recursive(tree, id * 2, tl, mid, l, r);
    long long sum_r = sum_tree_recursive(tree, id * 2 + 1, mid + 1, tr, l, r);
    return sum_l + sum_r;
}

vector<long long> DO(vector<int>& a, int n, int q) {
    vector<long long> answers;
    vector<long long> tree(4 * n, 0);
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
            answers.push_back(sum_tree_recursive(tree, 1, 0, count_list - 1, l, r));
        }
    }
    return answers;
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

    vector<int> a_pref = original_a;
    vector<long long> ans_pref = PREF(a_pref, n, q);

    fin.clear();
    fin.seekg(0, ios::beg);
    int dummy;
    fin >> dummy >> dummy;
    for (int i = 0; i < n; ++i) {
        fin >> dummy;
    }

    vector<int> a_sqrt = original_a;
    vector<long long> ans_sqrt = SQRT(a_sqrt, n, q);

    fin.clear();
    fin.seekg(0, ios::beg);
    fin >> dummy >> dummy;
    for (int i = 0; i < n; ++i) {
        fin >> dummy;
    }

    vector<int> a_do = original_a;
    vector<long long> ans_do = DO(a_do, n, q);

    cout << left << setw(15) << "Prefix"
         << setw(15) << "SQRT"
         << setw(15) << "DO" << "\n";
    cout << "------------------------------------------\n";

    for (size_t i = 0; i < ans_pref.size(); ++i) {
        cout << left << setw(15) << ans_pref[i]
             << setw(15) << ans_sqrt[i]
             << setw(15) << ans_do[i] << "\n";
    }

    fin.close();
    return 0;
}
