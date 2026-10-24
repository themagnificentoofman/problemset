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
     Given a grid of rock (#) and empty (.), choose some empty cells to be water.
     If a cell is water, then every empty cell reachable from it without ever moving to a higher row
     must also be water. Count valid paintings modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - Define "a forces b" if making a water implies b must be water; this relation is transitive.
     - Cells in the same row can mutually force each other, so we compress them into a single node.
     - After compressing, edges only go from a row to the next lower row, and each node has <= 1
       parent, so the graph is a forest.
     - For one node, either we put water somewhere in that node (then it forces all descendants), or
       we put no water in that node and choices in children are independent:
       dp[node] = 1 + product(dp[child]).

 (3) Solution outline:
     - Sweep rows from bottom to top; union empty cells horizontally inside the row.
     - If two cells in the current row touch the same component in the row below, union them too;
       store one "up" column for each below-component to detect these merges.
     - Assign ids bottom-to-top; connect each node to its parent using the stored column, otherwise
       connect it to a fake root 0.
     - Compute dp in increasing id order. The fake root should not have the extra "+1", so the final
       answer is product(dp[child of root]).
     - Time complexity: O(NM * α(M)); Memory complexity: O(NM).
 */

inline void solve() {
	int n, m;
	cin >> n >> m;
	vs grid(n);
	for (int i = 0; i < n; i++) cin >> grid[i];

	V<vi> uf(n, vi(m));
	V<vi> up(n, vi(m, -1));      // for a component in row r, stores a column in row r-1 that links to it
	V<vi> compId(n, vi(m, -1));  // id for DSU roots only

	auto findRoot = [&](int r, int c) {
		int x = c;
		while (uf[r][x] != x) {
			uf[r][x] = uf[r][uf[r][x]];
			x = uf[r][x];
		}
		return x;
	};
	auto unite = [&](int r, int a, int b) {
		a = findRoot(r, a), b = findRoot(r, b);
		uf[r][a] = b;
	};

	// build DSU for each row, sweeping upward (so "connections through lower rows" are already known)
	for (int i = n - 1; i >= 0; i--) {
		for (int j = 0; j < m; j++) uf[i][j] = j;

		for (int j = 0; j + 1 < m; j++) {
			if (grid[i][j] == '.' && grid[i][j + 1] == '.') unite(i, j, j + 1);
		}

		if (i + 1 < n) {
			for (int j = 0; j < m; j++) {
				if (grid[i][j] != '.' || grid[i + 1][j] != '.') continue;
				int belowRoot = findRoot(i + 1, j);
				if (up[i + 1][belowRoot] == -1) up[i + 1][belowRoot] = j;
				else unite(i, j, up[i + 1][belowRoot]);
			}
		}
	}

	// assign component ids bottom-to-top so children always have smaller ids than parents
	int treeSize = 1; // 0 is the fake root
	for (int i = n - 1; i >= 0; i--) {
		for (int j = 0; j < m; j++) {
			if (grid[i][j] == '.' && findRoot(i, j) == j) compId[i][j] = treeSize++;
		}
	}

	vi head(treeSize, -1), to, nxt;
	to.reserve(treeSize);
	nxt.reserve(treeSize);

	auto addEdge = [&](int p, int ch) {
		nxt.pb(head[p]);
		to.pb(ch);
		head[p] = sz(to) - 1;
	};

	// connect each node to its unique parent (or to the fake root)
	for (int i = n - 1; i >= 0; i--) {
		for (int j = 0; j < m; j++) {
			if (grid[i][j] != '.' || findRoot(i, j) != j) continue;

			int ch = compId[i][j];
			int p = 0;
			if (i > 0 && up[i][j] != -1) {
				int pRoot = findRoot(i - 1, up[i][j]);
				p = compId[i - 1][pRoot];
			}
			addEdge(p, ch);
		}
	}

	vi dp(treeSize, 1);
	for (int i = 1; i < treeSize; i++) {
		ll ways = 1;
		for (int e = head[i]; e != -1; e = nxt[e]) ways = ways * dp[to[e]] % MOD;
		dp[i] = int((ways + 1) % MOD);
	}

	ll ans = 1;
	for (int e = head[0]; e != -1; e = nxt[e]) ans = ans * dp[to[e]] % MOD;
	cout << ans % MOD << '\n';
}

int main() {
	setIO("cave");
	solve(); // single case
}
