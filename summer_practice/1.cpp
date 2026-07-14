#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>

using namespace std;

//G - список смежности, n - количество вершин. Возвращает количество компонент связности
int Compo(vector<vector<int>>& G, int n) {
    vector<int> mark(n + 1, 0);
    int count = 0;
    for (int i = 1; i <= n; ++i) {
        if (mark[i] == 0) {
            count++;
            queue<int> q;
            q.push(i);
            mark[i] = 1;
            while (q.size() > 0) {
                int tmp = q.front();
                q.pop();
                for (int j = 0; j < G[tmp].size(); ++j) {
                    if (mark[G[tmp][j]] == 0) {
                        mark[G[tmp][j]] = 1;
                        q.push(G[tmp][j]);
                    }
                }
            }
        }
    }
    return count;
}

// G - список смежности, s - начальная вершина пути, f - конечная вершина пути, n - количество вершин
void TheShortest(vector<vector<int>>& G, int s, int f, int n) {
    if (s == f) {
        cout << "Length = 0" << "\n" << "way: " << s << '\n';
        return;
    }
    vector<int> len(n + 1, -1);
    vector<int> tracking(n + 1, -1);
    queue<int> q;
    len[s] = 0;
    q.push(s);
    while (q.size() > 0) {
        int tmp = q.front();
        q.pop();
        for (int j = 0; j < G[tmp].size(); ++j) {
            if (len[G[tmp][j]] == -1) {
                len[G[tmp][j]] = len[tmp] + 1;
                tracking[G[tmp][j]] = tmp;
                q.push(G[tmp][j]);
                if (G[tmp][j] == f) {
                    vector<int> way;
                    for (int v = f; v != -1; v = tracking[v]) {
                        way.push_back(v);
                    }
                    reverse(way.begin(), way.end());
                    cout << "Length short way: " << len[G[tmp][j]] << '\n';
                    cout << "Recowery way: ";
                    for (int w : way) {
                        cout << w << " ";
                    }
                    cout << '\n';
                    return;
                }
            }
        }
    }
    cout << "Way between " << s << " and " << f << " doesn't exist" << '\n';
}

// G - список смежности, n - количество вершин
int CountCompo(vector<vector<int>>& G, int n) {
    vector<int> mark(n + 1, 0);
    int count = 0;
    for (int i = 1; i <= n; ++i) {
        if (mark[i] == 0) {
            count++;
            mark[i] = 1;
            stack<int> st;
            st.push(i);
            while (st.size() > 0) {
                int tmp = st.top();
                st.pop();
                for (int j = 0; j < G[tmp].size(); ++j) {
                    if (mark[G[tmp][j]] == 0) {
                        mark[G[tmp][j]] = 1;
                        st.push(G[tmp][j]);
                    }
                }
            }
        }
    }
    return count;
}

// G - список смежности
// mark - массив меток вершин
// flag - флаг обнаружения цикла
// st - стек для хранения пути
// x - текущая вершина
// y - предыдущая вершина
void dfs(vector<vector<int>>& G, vector<int>& mark, bool& flag, stack<int>& st, int x, int y) {
    st.push(x);
    mark[x] = 1;
    for (int i = 0; i < G[x].size() && flag == false; i++){
        if (mark[G[x][i]] == 0){
            dfs(G, mark, flag, st, G[x][i], x);
        }
        else {
            if (G[x][i] != y) {
                st.push(G[x][i]);
                flag = true;
            }
        }
    }
    if (flag == false) {
        st.pop();
    }
}


int main() {
    int n, m;
    cout << "Kolvo reber";
    cin >> n >> m;
    vector<vector<int>> G(n + 1);
    cout << "Vvedite rebra (mezdu kakimi syshestvuet):" << '\n';
    for (int i = 0; i < m; ++i) {
        int a, b;
        cin >> a >> b;
        G[a].push_back(b);
        G[b].push_back(a);
    }
    vector<int> mark(n + 1, 0);
    bool flag = false;
    stack<int> st;
    for (int i = 1; i <= n && flag == false; i++) {
        if (mark[i] == 0) {
            dfs(G, mark, flag, st, i, 0);
        }
    }
    int Compo = CountCompo(G, n);
    cout << "Kol-vo komponent svaznosty " << Compo << '\n';
    if (flag) {
        vector<int> res;
        res.push_back(st.top());
        st.pop();
        while (!st.empty() && st.top() != res[0]) {
            res.push_back(st.top());
            st.pop();
        }
        res.push_back(res[0]);
        reverse(res.begin(), res.end());
        cout << "cycle: ";
        for (int i = 0; i < res.size(); ++i) {
            cout << res[i];
            if (i < res.size() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
    else {
        cout << "Graf ne soderzit thikla" << '\n';
    }
}
