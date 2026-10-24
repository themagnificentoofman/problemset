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
   We need the minimum time to go from (0,0) to (N-1,N-1) on an N×N grid.
   Each move costs T time, and every 3rd move Bessie “eats” and pays the grass cost of the landing cell.

2) Things to notice + step-by-step:
   - Eating happens only after moves #3, #6, #9, ... (start doesn’t count).
   - So it’s natural to run Dijkstra on states “position right after eating”.
   - From any such state, the next eating position is exactly 3 moves away.
     In 3 moves, you can end at Manhattan distance 3 (various (dx,dy)) or Manhattan distance 1 (by backtracking).
   - The very end is special: you might finish with 0/1/2 extra moves after the last eating, paying only move time.

3) Solution outline:
   - Dijkstra over grid cells with edges for “exactly 3 moves”: cost = 3*T + grass[next].
   - For every popped cell, also try finishing directly if the Manhattan distance to the goal is < 3: add dist*T.
   - Output the best distance to the goal.
   - Time: O(N^2 log(N^2)).
*/

inline void solve() {
	int n;
	ll t;
	cin >> n >> t;

	V<vi> grass(n, vi(n));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) cin >> grass[i][j];
	}

	const int dx[16] = {1, 0, -1, 0, 3, 0, -3, 0, 2, 2, 1, 1, -1, -1, -2, -2};
	const int dy[16] = {0, 1, 0, -1, 0, 3, 0, -3, 1, -1, 2, -2, 2, -2, 1, -1};

	V<vl> dist(n, vl(n, BIG));
	using State = P<ll, pi>; // {dist, {r,c}}
	pqg<State> pq;

	dist[0][0] = 0;
	pq.push({0, {0, 0}});

	while (!pq.empty()) {
		auto [d, rc] = pq.top(); pq.pop();
		int r = rc.fi, c = rc.se;
		if (d != dist[r][c]) continue;

		int md = (n - 1 - r) + (n - 1 - c);
		if (md < 3) {
			dist[n - 1][n - 1] = min(dist[n - 1][n - 1], d + (ll)md * t);
		}

		for (int k = 0; k < 16; k++) {
			int nr = r + dx[k], nc = c + dy[k];
			if (nr < 0 || nr >= n || nc < 0 || nc >= n) continue;

			ll nd = d + 3LL * t + grass[nr][nc];
			if (nd < dist[nr][nc]) {
				dist[nr][nc] = nd;
				pq.push({nd, {nr, nc}});
			}
		}
	}

	cout << dist[n - 1][n - 1] << "\n";
}

int main() {
	setIO("visitfj");
	solve(); // single case
}
