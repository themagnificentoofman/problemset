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
/* inline namespace FileIO {
	void setIn(string s){freopen(s.c_str(),"r",stdin);}
	void setOut(string s){freopen(s.c_str(),"w",stdout);}
	void setIO(string s = ""){
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if breaks things
		if (sz(s)) setIn(s+".in"), setOut(s+".out");
	}
} */ // not needed here

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = 4e18; // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)1e5+5; // modify as needed
const int LOG = 18; // > log_2(MXN) + 1

#include "grader.h" // deprecated???

/*
 (1) Problem recap in simple terms:
     Assign each tree node a unique grid point (1..N, 1..N) so that for any query path A→B, we can
     describe exactly the nodes on that path using the union of at most two axis-aligned rectangles,
     and the rectangles do not overlap on any farm.

 (2) Things to notice and step-by-step derivation:
     - Root the tree at 0 and give every subtree its own disjoint rectangle inside the N×N grid.
     - Put each node at the bottom-left corner of its subtree rectangle; then every ancestor is
       left-and-below every descendant (x[anc] ≤ x[des], y[anc] ≤ y[des]).
     - For a query (A,B) with L = LCA(A,B), the path splits into L→A and child(L)→B. Each part is an
       ancestor-to-descendant path, which becomes exactly one rectangle in this embedding.

 (3) Solution outline:
     - Store the tree in adjacency lists via addRoad.
     - In buildFarms: root at 0, compute parent/depth and subtree sizes, then iteratively assign
       subtree rectangles and call setFarmLocation once per node.
     - Precompute binary lifting parents for LCA queries.
     - In notifyFJ: output one rectangle for L→A; if B ≠ L, lift B to the child of L on the path and
       output a second rectangle for that child→B.
     - Time: O((N + Q) log N), Memory: O(N log N)
 */

// tree + embedding state.
static vi adj[MXN], child[MXN];
static int xCoord[MXN], yCoord[MXN];
static int up[LOG][MXN], depth[MXN], subSz[MXN];
static int n;

// jump v upward by k edges (assumes k <= depth).
static int jumpUp(int v, int k) {
	for (int i = 0; i < LOG; i++) if (k & (1 << i)) v = up[i][v];
	return v;
}

// LCA with binary lifting.
static int lca(int a, int b) {
	if (depth[a] < depth[b]) swap(a, b);
	a = jumpUp(a, depth[a] - depth[b]);
	if (a == b) return a;
	for (int i = LOG - 1; i >= 0; i--) {
		if (up[i][a] != up[i][b]) {
			a = up[i][a];
			b = up[i][b];
		}
	}
	return up[0][a];
}

void addRoad(int a, int b) {
	adj[a].pb(b);
	adj[b].pb(a);
}

void buildFarms() {
	n = getN();

	// build parent/depth via iterative DFS, keep an order for postprocessing.
	vi order;
	order.reserve(n);
	vi st;
	st.reserve(n);

	for (int i = 0; i < n; i++) child[i].clear();

	up[0][0] = -1;
	depth[0] = 0;
	st.pb(0);

	while (sz(st)) {
		int v = st.bk; st.ppb();
		order.pb(v);
		for (int to : adj[v]) {
			if (to == up[0][v]) continue;
			up[0][to] = v;
			depth[to] = depth[v] + 1;
			st.pb(to);
		}
	}

	// subtree sizes in reverse order.
	for (int i = n - 1; i >= 0; i--) {
		int v = order[i];
		subSz[v] = 1;
		for (int to : adj[v]) {
			if (to == up[0][v]) continue;
			subSz[v] += subSz[to];
		}
	}

	// build child lists in adjacency order (so embedding is deterministic).
	for (int v = 0; v < n; v++) {
		for (int to : adj[v]) if (to != up[0][v]) child[v].pb(to);
	}

	// assign coordinates by simulating the recursive rectangle-splitting DFS.
	struct Frame {
		int v, idx;
		int curYlo, curXhi;
	};
	V<Frame> stk;
	stk.reserve(n);

	xCoord[0] = 1;
	yCoord[0] = 1;
	setFarmLocation(0, 1, 1);
	stk.pb({0, 0, 2, n});

	while (sz(stk)) {
		Frame& f = stk.bk;
		if (f.idx == sz(child[f.v])) {
			stk.ppb();
			continue;
		}
		int to = child[f.v][f.idx++];
		int ylo = f.curYlo;
		int xhi = f.curXhi;

		int yhi = ylo + subSz[to] - 1; (void)yhi; // yhi isn't needed explicitly later.
		int xlo = xhi - subSz[to] + 1;

		f.curYlo += subSz[to];
		f.curXhi -= subSz[to];

		xCoord[to] = xlo;
		yCoord[to] = ylo;
		setFarmLocation(to, xlo, ylo);

		stk.pb({to, 0, ylo + 1, xhi});
	}

	// binary lifting table.
	for (int i = 1; i < LOG; i++) {
		for (int v = 0; v < n; v++) {
			int p = up[i - 1][v];
			up[i][v] = (p == -1 ? -1 : up[i - 1][p]);
		}
	}
}

void notifyFJ(int a, int b) {
	int l = lca(a, b);

	addBox(xCoord[l], yCoord[l], xCoord[a], yCoord[a]);
	if (l == b) return;

	int b0 = b;
	int steps = depth[b0] - depth[l] - 1; // move to the child of l on the path to b0
	int mid = jumpUp(b0, steps);

	addBox(xCoord[mid], yCoord[mid], xCoord[b0], yCoord[b0]);
}