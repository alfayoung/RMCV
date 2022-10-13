#include <bits/stdc++.h>
using namespace std;
#define mp make_pair
typedef pair<int, int> pii;
#define d(x) cerr << #x << " = " << x << endl
const int maxn = 1005;
int n, m, k;
int vis[maxn][maxn];
int dirx[4] = {0, 1, 0, -1}, diry[4] = {1, 0, -1, 0};

int valid(pii u) {
	return 1 <= u.first && u.first <= n && 1 <= u.second && u.second <= m;
}

void dfs(pii u, int dir) {
	vis[u.first][u.second] = 1;
	pii v = mp(u.first + dirx[dir], u.second + diry[dir]);
	if (valid(v) && !vis[v.first][v.second]) {
		dfs(v, dir);
		return ;
	}
	dir = (dir + 1) % 4;
	v = mp(u.first + dirx[dir], u.second + diry[dir]);
	if (valid(v) && !vis[v.first][v.second]) {
		dfs(v, dir);
		return ;
	}
}

int main() {
	cin >> n >> m >> k;
	for (int i = 1; i <= k; i++) {
		int x, y; cin >> x >> y;
		vis[x][y] = 1;
	}
	dfs(mp(1, 1), 0);
	int ans = 1;
	for (int i = 1; i <= n; i++) for (int j = 1; j <= m; j++) ans &= vis[i][j];
	if (ans) cout << "Yes" << endl;
	else cout << "No" << endl;
	return 0;
}
