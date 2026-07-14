#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <set>

using namespace std;

// G - список смежности графа с весами
// s - стартовая вершина
// n - количество вершин
void DxSchool(vector<vector<pair<int, int>>>& G, int start, int n) {
    vector<int> Mark(n + 1, 0);
    vector<int> Len(n + 1, 1e9);

    Len[start] = 0;

    for (int iter = 0; iter < n; iter++) {
        int uk = 0;
        int len = 1e9;

        for (int i = 1; i <= n; i++) {
            if (Mark[i] == 0) {
                if (len > Len[i]) {
                    uk = i;
                    len = Len[i];
                }
            }
        }

        if (uk != 0) {
            Mark[uk] = 1;
            Len[uk] = len;

            for (int i = 0; i < G[uk].size(); i++) {
                Len[G[uk][i].second] = min(Len[G[uk][i].second], Len[uk] + G[uk][i].first);
            }
        }
    }

    cout << "School version:\n";
    for (int i = 1; i <= n; i++) {
        if (Len[i] != 1e9) {
            cout << "do versini " << i << ": " << Len[i] << '\n';
        } else {
            cout << "do versini " << i << ": nedostizima\n";
        }
    }
}

// G - список смежности графа с весами
// s - стартовая вершина
// n - количество вершин
void DxStudent(vector<vector<pair<int, int>>>& G, int start, int n) {
    vector<int> Len(n + 1, 1e9);
    vector<int> Mark(n + 1, 0);

    set<pair<int, int>> st;
    st.insert({0, start});

    while (st.size() > 0) {
        pair<int, int> tmp = *st.begin();
        st.erase(st.begin());

        // tmp.first - это текущая длина пути
        // tmp.second - это текущая вершина
        if (Mark[tmp.second] == 0) {
            Len[tmp.second] = tmp.first;
            Mark[tmp.second] = 1;

            for (int i = 0; i < G[tmp.second].size(); i++) {
                // G[].first - вес ребра, G[].second - соседняя вершина
                st.insert({tmp.first + G[tmp.second][i].first, G[tmp.second][i].second});
            }
        }
    }

    cout << "\nstudent version:\n";
    for (int i = 1; i <= n; i++) {
        if (Len[i] != 1e9)
            cout << "do versini " << i << ": " << Len[i] << '\n';
        else
            cout << "do versini " << i << ": nedostizima" << '\n';
    }
}

int main() {return 0;}
