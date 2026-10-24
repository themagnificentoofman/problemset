// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default)
using db = long double; // change to double if TL is tight (almost never)

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
// lower_bound / upper_bound convenience; use on sorted containers
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better, basically (can use .at())
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // ind of 1st elem >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // ind of 1st elem > b
template<class T> using pq = priority_queue<T>; // max-heap priority queue (largest on top)
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>; // min-heap priority queue (smallest on top)

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
const ll BIG = 4e18; // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)2e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     We have a tree, and each query gives a path (p,q) with a weight r. For every tree edge, we want the
     smallest r among queries whose path uses that edge (or -1 if none).

 (2) Things to notice and step-by-step derivation:
     - If we add r at both endpoints p and q, then along the rooted tree it should “bubble up” until LCA.
     - At l = LCA(p,q), we must delete r twice (because we inserted it twice), so r only stays on edges
       strictly below l on the p-q path.
     - If each node maintains a multiset of active r-values in its subtree, then the answer for the edge
       (parent[node], node) is just the minimum element in that multiset.
     - Merging multisets small-into-large (DSU-on-tree style) keeps total work fast.

 (3) Solution outline:
     - Root the tree at 0 and compute parent/depth plus a binary-lifting table for LCA in O(n log n).
     - For each query (p,q,r): add r to add[p] and add[q], and add r to rem[lca(p,q)].
     - Process nodes in postorder: merge children's multisets into the node, apply add/rem at the node,
       then set ans[node] = min(multiset) (this is for the edge to its parent).
     - For each original edge (u,v), output ans[deeper_endpoint].
     - Time: O((n+m) log(n+m)); Memory: O(n+m)
 */

inline void solve() {
	int n, m;
	cin >> n >> m;

	V<vi> adj(n);
	vpi edges;
	edges.reserve(n - 1);

	for (int i = 0; i < n - 1; i++) {
		int a, b;
		cin >> a >> b;
		--a; --b;
		adj[a].pb(b);
		adj[b].pb(a);
		edges.eb(a, b);
	}

	vi parent(n, 0), depth(n, 0), order;
	order.reserve(n);

	// build parent/depth + a DFS order (reversed later for postorder)
	{
		stack<int> st;
		parent[0] = 0;
		depth[0] = 0;
		st.push(0);

		while (!st.empty()) {
			int v = st.top();
			st.pop();
			order.pb(v);
			for (int to : adj[v]) if (to != parent[v]) {
				parent[to] = v;
				depth[to] = depth[v] + 1;
				st.push(to);
			}
		}
	}

	int LOG = 1;
	while ((1 << LOG) <= n) LOG++;
	V<vi> up(LOG, vi(n));
	up[0] = parent;
	for (int j = 1; j < LOG; j++) {
		for (int v = 0; v < n; v++) up[j][v] = up[j - 1][up[j - 1][v]];
	}

	auto lca = [&](int a, int b) -> int {
		if (depth[a] < depth[b]) swap(a, b);
		int diff = depth[a] - depth[b];
		for (int j = 0; j < LOG; j++) if ((diff >> j) & 1) a = up[j][a];
		if (a == b) return a;
		for (int j = LOG - 1; j >= 0; j--) {
			if (up[j][a] != up[j][b]) {
				a = up[j][a];
				b = up[j][b];
			}
		}
		return parent[a];
	};

	V<vi> add(n), rem(n);
	add.assign(n, {});
	rem.assign(n, {});
	for (int i = 0; i < m; i++) {
		int p, q, r;
		cin >> p >> q >> r;
		--p; --q;
		int l = lca(p, q);
		add[p].pb(r);
		add[q].pb(r);
		rem[l].pb(r);
	}

	vi ans(n, -1);
	V<multiset<int>> sets(n);

	// postorder DP by processing reversed DFS order
	for (int idx = n - 1; idx >= 0; idx--) {
		int v = order[idx];

		for (int to : adj[v]) if (to != parent[v]) {
			if (sz(sets[to]) > sz(sets[v])) sets[v].swap(sets[to]);
			sets[v].insert(all(sets[to]));
			sets[to].clear();
		}

		for (int w : add[v]) sets[v].insert(w);

		for (int w : rem[v]) {
			for (int t = 0; t < 2; t++) {
				auto it = sets[v].find(w);
				if (it != end(sets[v])) sets[v].erase(it);
			}
		}

		if (v != 0) {
			if (!sets[v].empty()) ans[v] = *bg(sets[v]);
		}
	}

	for (auto [a, b] : edges) {
		if (depth[a] < depth[b]) swap(a, b);
		cout << ans[a] << '\n';
	}
}

int main() {
	setIO("disrupt");
	solve(); // single case
}
