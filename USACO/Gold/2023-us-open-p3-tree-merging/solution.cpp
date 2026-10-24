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
const int MAXB = 1005;

/*
 (1) Problem recap in simple terms:
     We are given an initial rooted tree on labels 1..n and a final rooted tree on a subset
     of those labels. One operation merges two siblings into one node whose label is the
     max label of the two and whose children are the union. Output any merge sequence that
     transforms the initial tree into the final tree (guaranteed possible).

 (2) Things to notice and step-by-step derivation:
     - The root can never be merged, so the initial root must be the final root.
     - Each final node represents a "component" of initial nodes merged into it, and the
       representative label must be the maximum label in that component.
     - Define dp[a][b] (b is in final tree): can initial node a end up represented by b?
       Then every child of a must map to some child of b, and if a itself is final, it can
       only map to itself.
     - Use bitsets: store for each node a the set of final nodes b it can map to, so we can
       test "does child c map to any child of b" with one bitset intersection.

 (3) Solution outline:
     - Read both trees, mark which labels are present in the final tree, and build child lists.
     - Precompute a bitset childMask[b] of b's children in the final tree.
     - Bottom-up DP on the initial tree by decreasing depth to fill canMap[a] as a bitset of
       valid final nodes b.
     - Top-down reconstruction: for each initial node a, choose rep[a] as some child of
       rep[parent[a]] in the final tree that a can map to; if a != rep[a], output merge a rep[a].
     - Time: O(n^3 / 64) per test case (sum n <= 1000); Memory: O(n^2 / 8).
 */

inline void solve() {
	int n; cin >> n;

	V<vi> child1(n + 1);
	vi par1(n + 1, 0);
	for (int i = 0; i < n - 1; i++) {
		int v, p; cin >> v >> p;
		par1[v] = p;
		child1[p].pb(v);
	}

	int root = 1;
	for (int i = 1; i <= n; i++) if (par1[i] == 0) root = i;

	int m; cin >> m;

	V<vi> child2(n + 1);
	V<char> ext(n + 1, 0);
	ext[root] = 1;
	for (int i = 0; i < m - 1; i++) {
		int v, p; cin >> v >> p;
		child2[p].pb(v);
		ext[v] = 1;
		ext[p] = 1;
	}

	vi extList;
	for (int i = 1; i <= n; i++) if (ext[i]) extList.pb(i);

	vi id(n + 1, -1);
	for (int i = 0; i < sz(extList); i++) id[extList[i]] = i;

	// depth + grouping by depth in initial tree
	vi depth(n + 1, 0);
	int maxDepth = 0;
	{
		vi st;
		st.pb(root);
		while (sz(st)) {
			int u = st.bk; st.ppb();
			maxDepth = max(maxDepth, depth[u]);
			for (int v : child1[u]) {
				depth[v] = depth[u] + 1;
				st.pb(v);
			}
		}
	}

	V<vi> byDepth(maxDepth + 1);
	for (int i = 1; i <= n; i++) byDepth[depth[i]].pb(i);

	// bitset masks on final-tree nodes (indexed by extList order)
	V<bitset<MAXB>> childMask(n + 1);
	for (int b : extList) {
		childMask[b].reset();
		for (int d : child2[b]) childMask[b].set(id[d]);
	}

	// canMap[a]: which final nodes b can represent initial node a
	V<bitset<MAXB>> canMap(n + 1);
	for (int a = 1; a <= n; a++) canMap[a].reset();

	for (int d = maxDepth; d >= 0; d--) {
		for (int a : byDepth[d]) {
			canMap[a].reset();

			for (int b : extList) {
				if (a > b) continue;
				if (ext[a] && a != b) continue;

				bool ok = true;
				for (int c : child1[a]) {
					if ((canMap[c] & childMask[b]).none()) {
						ok = false;
						break;
					}
				}
				if (ok) canMap[a].set(id[b]);
			}
		}
	}

	vi rep(n + 1, 0);
	rep[root] = root;

	vpi ops;
	ops.reserve(n - m);

	for (int d = 1; d <= maxDepth; d++) {
		for (int a : byDepth[d]) {
			int p = par1[a];
			int b = rep[p];

			int chosen = -1;
			for (int dch : child2[b]) {
				if (canMap[a].test(id[dch])) {
					chosen = dch;
					break;
				}
			}

			rep[a] = chosen;
			if (a != chosen) ops.pb({a, chosen});
		}
	}

	cout << sz(ops) << '\n';
	for (auto e : ops) cout << e.fi << ' ' << e.se << '\n';
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while(t--) solve();
}
