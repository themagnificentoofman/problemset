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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // 1st idx with a[idx] >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // 1st idx with a[idx] > b
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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple terms):
   We have a heap-indexed binary tree. Each node i has a value a[i] and a cost c[i] to change it to anything.
   Running the "median heap" process bottom-up makes each internal node end up holding the median of (itself + 2 children).
   For each query target m, find the minimum total cost to edit some node values so the final root value becomes m.

2) Things to notice and step-by-step:
   - For a fixed target m, each node’s original value is either < m, = m, or > m.
   - If we force a node to act as one of these three relations, it costs 0 if it matches its original relation, else costs c[i].
   - The median-heap operation composes as: state(parent) = median(state(parent), state(left), state(right)).
     So we can do DP on the tree: dp[v][s] = min cost to make subtree v evaluate to state s (s in {>, =, <}).

3) Solution outline:
   - Offline sweep m in sorted order with coordinate compression:
       values > m start as state 0, values = m temporarily set to state 1, then become state 2 after passing m.
   - Maintain dp with point updates up the ancestor chain (each merge is 27 transitions).
   - Answer for m is dp[root][1].
   - Time: O((N+Q) log N * 27), memory O(N).
*/

inline void solve() {
	int n; cin >> n;
	vi a(n);
	vl c(n);
	V<AR<ll,3>> dp(n);
	vi f(n, 0); // 0: ">", 1: "=", 2: "<" (relative to current m in sweep)

	vi pos;
	pos.reserve(n);

	for (int i = 0; i < n; i++) {
		cin >> a[i] >> c[i];
		pos.pb(a[i]);
	}

	int q; cin >> q;
	vi query(q);
	pos.reserve(n + q);
	for (int i = 0; i < q; i++) {
		cin >> query[i];
		pos.pb(query[i]);
	}

	sor(pos);
	pos.erase(unique(all(pos)), end(pos));
	int m = sz(pos);

	V<vi> nodes(m), qs(m);
	for (int i = 0; i < n; i++) nodes[lwb(pos, a[i])].pb(i);
	for (int i = 0; i < q; i++) qs[lwb(pos, query[i])].pb(i);

	auto med3 = [&](int x, int y, int z) -> int {
		return x + y + z - min({x, y, z}) - max({x, y, z});
	};

	auto merge = [&](int p) -> void {
		AR<ll,3> cost{c[p], c[p], c[p]};
		cost[f[p]] = 0;

		int l = 2 * p + 1, r = 2 * p + 2;
		if (l >= n) {
			dp[p] = cost;
			return;
		}

		dp[p] = {BIG, BIG, BIG};
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					int s = med3(i, j, k);
					ll cand = cost[i] + dp[l][j] + dp[r][k];
					dp[p][s] = min(dp[p][s], cand);
				}
			}
		}
	};

	auto update = [&](int p, int state) -> void {
		f[p] = state;
		while (true) {
			merge(p);
			if (p == 0) break;
			p = (p - 1) / 2;
		}
	};

	// initial build with all nodes as ">" (state 0)
	for (int i = n - 1; i >= 0; i--) merge(i);

	vl ans(q, 0);

	for (int id = 0; id < m; id++) {
		// nodes with value == current m become "="
		for (int v : nodes[id]) update(v, 1);

		// answer queries for this m
		for (int qi : qs[id]) ans[qi] = dp[0][1];

		// after passing this m, those nodes become "<" for future larger targets
		for (int v : nodes[id]) update(v, 2);
	}

	for (int i = 0; i < q; i++) cout << ans[i] << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
