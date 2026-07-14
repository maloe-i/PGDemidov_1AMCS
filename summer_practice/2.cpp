#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

void Prefix() {
    int n;
    cin >> n;

    vector<int> A(n + 1);
    for (int i = 1; i <= n; i++) {
        cin >> A[i];
    }

    vector<long long> Pref(n + 1);
    Pref[0] = 0;
    for (int i = 1; i <= n; i++) {
        Pref[i] = Pref[i - 1] + A[i];
    }

    int q, l, r;
    cin >> q;
    for (int i = 0; i < q; i++) {
        cin >> l >> r;
        cout << Pref[r] - Pref[l - 1] << '\n';
    }
}

void Matrix() {
    int n, m;
    cin >> n >> m;

    vector<vector<long long>> A(n, vector<long long>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> A[i][j];
        }
    }

    vector<vector<long long>> Dp(n, vector<long long>(m, 0));
    Dp[0][0] = A[0][0];

    for (int j = 1; j < m; j++) {
        Dp[0][j] = Dp[0][j - 1] + A[0][j];
    }

    for (int i = 1; i < n; i++) {
        Dp[i][0] = Dp[i - 1][0] + A[i][0];
    }

    for (int i = 1; i < n; i++) {
        for (int j = 1; j < m; j++) {
            Dp[i][j] = max(Dp[i - 1][j], Dp[i][j - 1]) + A[i][j];
        }
    }

    cout << "Max)" << Dp[n - 1][m - 1] << '\n';
}

int NVP_school(const vector<int>& A) {
    int n = A.size();
    if (n == 0) return 0;

    vector<int> Dp(n, 1);
    int max_len = 1;

    for (int pos = 0; pos < n; pos++) {
        for (int i = 0; i < pos; i++) {
            if (A[i] < A[pos]) {
                Dp[pos] = max(Dp[pos], 1 + Dp[i]);
            }
        }
        max_len = max(max_len, Dp[pos]);
    }

    return max_len;
}

int NVP_university(const vector<int>& A) {
    int n = A.size();
    if (n == 0) return 0;

    multiset<int> mst;

    for (int i = 0; i < n; i++) {
        mst.insert(1e9);
    }

    for (int i = 0; i < n; i++) {
        mst.erase(mst.lower_bound(A[i]));
        mst.insert(A[i]);
    }

    int count = 0;
    for (int val : mst) {
        if (val != 1e9) {
            count++;
        }
    }

    return count;
}

int Bag() {
    int n;
    cin >> n;
    vector<int> W(n + 1), C(n + 1);
    for (int i = 1; i <= n; i++){
        cin >> W[i] >> C[i];
    }
    int s;
    cin >> s;
    vector<vector<int>> Dp(n + 1, vector<int>(s + 1, -1));
    Dp[0][0] = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= s; j++) {
            Dp[i][j] = Dp[i - 1][j];
        }
        for (int j = 0; j <= s; j++){
            if (j - W[i] >= 0) {
                if (Dp[i - 1][j - W[i]] != -1){
                    Dp[i][j] = max(Dp[i][j], C[i] + Dp[i - 1][j - W[i]]);
                }
            }
        }
    }
    return Dp[n][s];
}

int main() {
    return 0;
}
