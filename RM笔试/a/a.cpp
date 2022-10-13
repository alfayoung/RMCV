#include <bits/stdc++.h>
using namespace std;
#define mp make_pair
typedef pair<int, int> pii;
typedef long long ll;
#define d(x) cerr << #x << " = " << x << endl
const int maxn = 100005;
ll n, m, k;
ll Lb, Rb, Ub, Db;
deque<pii> dq1, dq2;
map<pii, int> mmp;
pii a[maxn];

int valid(pii u) {
	return Ub <= u.first && u.first <= Db && Lb <= u.second && u.second <= Rb;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(0); cout.tie(0);
	cin >> n >> m >> k;
	for (int i = 1; i <= k; i++) cin >> a[i].first >> a[i].second, mmp[a[i]] = 1;
	sort(a + 1, a + k + 1, [](pii A, pii B) { return A.first < B.first; });
	for (int i = 1; i <= k; i++) dq1.push_back(a[i]);
	// dq1 x increasing
	sort(a + 1, a + k + 1, [](pii A, pii B) { return A.second < B.second; });
	for (int i = 1; i <= k; i++) dq2.push_back(a[i]);
	// dq2 y increasing
	Lb = 1, Rb = m, Ub = 1, Db = n;
	int dir = 0;
	ll cnt = k;
	// 0->R 1->D 2->L 3->U
	for (;;) {
		pii now;
		if (dir == 0) now = mp(Ub, Lb);
		else if (dir == 1) now = mp(Ub, Rb);
		else if (dir == 2) now = mp(Db, Rb);
		else now = mp(Db, Lb);
		if (mmp.count(now)) break;
		
		while (!dq1.empty()) {
			if (valid(dq1.front())) break;
			else dq1.pop_front();
		}
		while (!dq1.empty()) {
			if (valid(dq1.back())) break;
			else dq1.pop_back();
		}
		while (!dq2.empty()) {
			if (valid(dq2.front())) break;
			else dq2.pop_front();
		}
		while (!dq2.empty()) {
			if (valid(dq2.back())) break;
			else dq2.pop_back();
		}
		int len[4] = {1000000000, 1000000000, 1000000000, 1000000000};
		int mn = 1e9;
		if (!dq1.empty()) {
			mn = min(mn, len[0] = dq1.front().first - Ub);
			mn = min(mn, len[2] = Db - dq1.back().first);
		}
		if (!dq2.empty()) {
			mn = min(mn, len[1] = Rb - dq2.back().second);
			mn = min(mn, len[3] = dq2.front().second - Lb);
		}
		if (mn == 1e9) {
			cnt += max((Rb - Lb + 1) * (Db - Ub + 1), 0LL);
			break;
		}
		int pos = dir;
		do {
			if (len[pos] == mn) break;
			pos = (pos + 1) % 4;
		} while (pos != dir);
		pii res;
		cnt += (Rb - Lb + 1) * (Db - Ub + 1) - (Rb - Lb + 1 - 2 * mn) * (Db - Ub + 1 - 2 * mn);
		Lb += mn; Rb -= mn; Ub += mn; Db -= mn;
		if (pos == 0) {
			res = dq1.front();
			while (!dq1.empty()) {
				if (dq1.front().first != res.first) break;
				else if (valid(dq1.front()) && dq1.front().second < res.second) res = dq1.front();
				dq1.pop_front();
			}
			if (dir == 0) {
				cnt += res.second - Lb;
				Ub++; Rb = res.second - 1;
			} else if (dir == 1) {
				cnt += Db - Ub + Rb - Lb + Db - Ub + res.second - Lb;
				Lb++; Ub++; Db--; Rb = res.second - 1;
			} else if (dir == 2) {
				cnt += Rb - Lb + Db - Ub + res.second - Lb;
				Lb++; Ub++; Db--; Rb = res.second - 1;
			} else {
				cnt += Db - Ub + res.second - Lb;
				Lb++; Ub++; Rb = res.second - 1;
			}
		} else if (pos == 1) {
			res = dq2.back();
			while (!dq2.empty()) {
				if (dq2.back().second != res.second) break;
				else if (valid(dq2.back()) && dq2.back().first < res.first) res = dq2.back();
				dq2.pop_back();
			}
			if (dir == 0) {
				cnt += Rb - Lb + res.first - Ub;
				Ub++; Rb--; Db = res.first - 1;
			} else if (dir == 1) {
				cnt += res.first - Ub;
				Rb--; Db = res.first - 1;
			} else if (dir == 2) {
				cnt += Rb - Lb + Db - Ub + Rb - Lb + res.first - Ub;
				Lb++; Ub++; Rb--; Db = res.first - 1;
			} else {
				cnt += Db - Ub + Rb - Lb + res.first - Ub;
				Lb++; Ub++; Rb--; Db = res.first - 1;
			}
		} else if (pos == 2) {
			res = dq1.back();
			while (!dq1.empty()) {
				if (dq1.back().first != res.first) break;
				else if (valid(dq1.back()) && dq1.back().second > res.second) res = dq1.back();
				dq1.pop_back();
			}
			if (dir == 0) {
				cnt += Rb - Lb + Db - Ub + Rb - res.second;
				Ub++; Rb--; Db--; Lb = res.second + 1;
			} else if (dir == 1) {
				cnt += Db - Ub + Rb - res.second;
				Rb--; Db--; Lb = res.second + 1;
			} else if (dir == 2) {
				cnt += Rb - res.second;
				Db--; Lb = res.second + 1;
			} else {
				cnt += Db - Ub + Rb - Lb + Db - Ub + Rb - res.second;
				Ub++; Rb--; Db--; Lb = res.second + 1;
			}
		} else {
			res = dq2.front();
			while (!dq2.empty()) {
				if (dq2.front().second != res.second) break;
				else if (valid(dq2.front()) && dq2.front().first > res.first) res = dq2.front();
				dq2.pop_front();
			}
			if (dir == 0) {
				cnt += Rb - Lb + Db - Ub + Rb - Lb + Db - res.first;
				Db--; Rb--; Lb++; Ub = res.first + 1;
			} else if (dir == 1) {
				cnt += Db - Ub + Rb - Lb + Db - res.first;
				Db--; Rb--; Lb++; Ub = res.first + 1;
			} else if (dir == 2) {
				cnt += Rb - Lb + Db - res.first;
				Lb++; Db--; Ub = res.first + 1;
			} else {
				cnt += Db - res.first;
				Lb++; Ub = res.first + 1;
			}
		}
		dir = (pos + 1) % 4;
	}
	if (cnt == m * n) cout << "Yes" << endl;
	else cout << "No" << endl;
	return 0;
}
