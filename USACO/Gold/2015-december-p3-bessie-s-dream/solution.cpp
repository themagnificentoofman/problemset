// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default)
using db = long double;  // change to double if TL is tight (almost never)

// pairs
template<class T1, class T2> using P = pair<T1,T2>;
#define mp make_pair
#define fi first
#define se second
using pi = P<int,int>;
using pl = P<ll,ll>;
using pd = P<db,db>;

// vectors
template<class T> using V = vector<T>;
#define sz(x) int((x).size())
#define bg(x) begin(x)
#define all(x) bg(x), end(x)
#define rall(x) (x).rbegin(), (x).rend()
#define sor(x) sort(all(x))
#define rsz resize
#define ins insert
#define pb push_back
#define eb emplace_back
#define ft front()
#define bk back()
using vi = V<int>;
using vb = V<bool>;
using vl = V<ll>;
using vd = V<db>;
using vs = V<str>;
using vpi = V<pi>;
using vpl = V<pl>;
using vpd = V<pd>;

// misc. ops & aliases
#define pf push_front
#define ppb pop_back
#define ppf pop_front
#define lb lower_bound
#define ub upper_bound
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s){freopen(s.c_str(),"r",stdin);}
	void setOut(string s){freopen(s.c_str(),"w",stdout);}
	void setIO(string s = ""){
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if breaks things
		if (sz(s)) setIn(s+".in"), setOut(s+".out");
	}
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   Move from (0,0) to (N-1,M-1) on a grid with special tiles:
   0=blocked, 1=normal, 2=orange (gain smell), 3=blue (only if smell),
   4=purple (slide in the direction you entered; smell becomes 0 on purple).
   Each step costs 1. Find the minimum steps, or -1 if impossible.

2) Things to notice + step-by-step:
   - This is shortest path on an unweighted state graph, so BFS works.
   - State needs: (r,c), whether we currently have smell, and last direction (for purple sliding).
   - From a purple tile with a valid last direction, we first try to keep sliding forward.
     If blocked, we can choose a new direction like normal.

3) Solution outline:
   - BFS over states (r,c,ld,smell), store dist in a 1D array.
   - Transition rules follow the tile meanings above.
   - Time: O(N*M*10).
*/

inline void solve() {
	int n, m;
	cin >> n >> m;

	V<vi> g(n, vi(m));
	for (int i = 0; i < n; i++) for (int j = 0; j < m; j++) cin >> g[i][j];

	const int dr[]{-1, 0, 1, 0};
	const int dc[]{0, -1, 0, 1};

	auto inside = [&](int r, int c) -> bool {
		return (0 <= r && r < n && 0 <= c && c < m);
	};

	auto idx = [&](int r, int c, int ld, int smell) -> int {
		// ld in {-1,0,1,2,3} -> (ld+1) in [0..4]
		int cell = r * m + c;
		return ((cell * 5 + (ld + 1)) * 2 + smell);
	};

	auto decode = [&](int id, int &r, int &c, int &ld, int &smell) {
		smell = id & 1;
		id >>= 1;
		ld = (id % 5) - 1;
		id /= 5;
		int cell = id;
		r = cell / m;
		c = cell % m;
	};

	int S = n * m * 10;
	vi dist(S, -1);
	queue<int> q;

	int start_smell = (g[0][0] == 2 ? 1 : 0);
	if (g[0][0] == 4) start_smell = 0;
	if (g[0][0] == 0) {
		cout << -1 << "\n";
		return;
	}
	if (g[0][0] == 3 && start_smell == 0) {
		cout << -1 << "\n";
		return;
	}

	int st = idx(0, 0, -1, start_smell);
	dist[st] = 0;
	q.push(st);

	while (!q.empty()) {
		int curId = q.ft; q.pop();
		int r, c, ld, smell;
		decode(curId, r, c, ld, smell);

		if (r == n - 1 && c == m - 1) {
			cout << dist[curId] << "\n";
			return;
		}

		// if on purple and we have a direction, try sliding forward
		if (g[r][c] == 4 && ld != -1) {
			int nr = r + dr[ld], nc = c + dc[ld];
			if (inside(nr, nc)) {
				int col = g[nr][nc];
				if (col != 0 && !(col == 3 && !smell)) {
					int nsm = smell;
					if (col == 2) nsm = 1;
					if (col == 4) nsm = 0;

					int nid = idx(nr, nc, ld, nsm);
					if (dist[nid] == -1) {
						dist[nid] = dist[curId] + 1;
						q.push(nid);
					}
					continue; // must keep sliding if possible
				}
			}
			// blocked: fall through and allow choosing a new direction
		}

		for (int d = 0; d < 4; d++) {
			int nr = r + dr[d], nc = c + dc[d];
			if (!inside(nr, nc)) continue;

			int col = g[nr][nc];
			if (col == 0) continue;
			if (col == 3 && !smell) continue;

			int nsm = smell;
			if (col == 2) nsm = 1;
			if (col == 4) nsm = 0;

			int nid = idx(nr, nc, d, nsm);
			if (dist[nid] != -1) continue;

			dist[nid] = dist[curId] + 1;
			q.push(nid);
		}
	}

	cout << -1 << "\n";
}

int main() {
	setIO("dream");
	solve(); // single case
}
