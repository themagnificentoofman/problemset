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
		cin.exceptions(cin.failbit);
		if (sz(s)) setIn(s+".in"), setOut(s+".out");
	}
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;

/*
 (1) problem recap in simple terms
     - We have a tree of n cows, each cow has an integer value.
     - There are two query types:
       * Type 1: update the value of a single cow.
       * Type 2: ask for the XOR of values along the unique path between two cows u and v.

 (2) things to notice and step-by-step derivation
     - Path queries on a tree with point updates suggest tree flattening + a Fenwick/segment tree.
     - Root the tree at 0 and run an Euler tour: each node v gets entry time tin[v] and exit time tout[v].
     - If we XOR a node’s value into both tin[v] and tout[v] in a Fenwick tree over the Euler order,
       then the prefix XOR up to tin[v] equals the XOR of values on the path from the root to v.
     - For any u, v:
         XOR(root→u) ^ XOR(root→v)
       cancels everything outside the u–v path, but the LCA’s value is removed twice, so we XOR it back once.
     - We precompute LCA with binary lifting; updates don’t change the tree structure, only values.

 (3) solution outline
     - Build the adjacency list, root at 0, and DFS to compute tin/tout and binary lifting parent table up[k][v].
     - Maintain a Fenwick tree supporting XOR:
         * For each node v, XOR val[v] at tin[v] and tout[v].
         * Fenwick prefix at tin[v] gives XOR(root→v).
     - For an update at node s:
         * XOR out the old value at tin[s] and tout[s], change val[s], then XOR in the new value.
     - For a query (u, v):
         * w = LCA(u, v)
         * answer = prefXOR(tin[u]) ^ prefXOR(tin[v]) ^ val[w].
     - Complexity: O((n + q) log n).
*/

inline void solve() {
	int n, q;
	cin >> n >> q;

	vi val(n);
	for (int i = 0; i < n; i++) cin >> val[i];

	V<vi> adj(n);
	for (int i = 0; i < n - 1; i++) {
		int u, v;
		cin >> u >> v;
		--u; --v;
		adj[u].pb(v);
		adj[v].pb(u);
	}

	// choose LOG just large enough for n
	int LOG = 1;
	while ((1 << LOG) <= n) LOG++;

	V<vi> up(LOG, vi(n));
	vi tin(n), tout(n);
	int timer = 1; // Fenwick indices start at 1

	// DFS for Euler tour + binary lifting parents
	function<void(int,int)> dfs = [&](int u, int p) {
		up[0][u] = p;
		tin[u] = timer++;

		for (int k = 1; k < LOG; k++) {
			up[k][u] = up[k-1][up[k-1][u]];
		}

		for (int v : adj[u]) {
			if (v == p) continue;
			dfs(v, u);
		}

		tout[u] = timer++;
	};

	dfs(0, 0); // root the tree at node 0

	int F = 2 * n + 5;      // Fenwick size; timer <= 2*n+1
	vi bit(F);

	auto fenw_add = [&](int idx, int v) {
		for (; idx < F; idx += idx & -idx) {
			bit[idx] ^= v;
		}
	};

	auto fenw_pref = [&](int idx) {
		int res = 0;
		for (; idx > 0; idx -= idx & -idx) {
			res ^= bit[idx];
		}
		return res;
	};

	auto is_anc = [&](int u, int v) {
		// is u an ancestor of v in the rooted tree
		return (tin[u] <= tin[v] && tout[u] >= tout[v]);
	};

	function<int(int,int)> lca = [&](int u, int v) -> int {
		if (is_anc(u, v)) return u;
		if (is_anc(v, u)) return v;
		for (int k = LOG - 1; k >= 0; k--) {
			int uu = up[k][u];
			if (!is_anc(uu, v)) u = uu;
		}
		return up[0][u];
	};

	// initial Fenwick build: for each node, add its value at tin and tout
	for (int i = 0; i < n; i++) {
		fenw_add(tin[i], val[i]);
		fenw_add(tout[i], val[i]);
	}

	while (q--) {
		int t;
		cin >> t;
		if (t == 1) {
			int s, x;
			cin >> s >> x;
			--s;
			// remove old value from Fenwick
			fenw_add(tin[s], val[s]);
			fenw_add(tout[s], val[s]);
			val[s] = x;
			// add new value
			fenw_add(tin[s], val[s]);
			fenw_add(tout[s], val[s]);
		} else {
			int u, v;
			cin >> u >> v;
			--u; --v;
			int w = lca(u, v);

			// XOR(root→u) ^ XOR(root→v) ^ val[w]
			int path_xor = (fenw_pref(tin[u]) ^ fenw_pref(tin[v])) ^ val[w];
			cout << path_xor << '\n';
		}
	}
}

int main() {
	setIO("cowland"); // comment out argument if using standard input/output only
	solve();          // single case (USACO-style)
}