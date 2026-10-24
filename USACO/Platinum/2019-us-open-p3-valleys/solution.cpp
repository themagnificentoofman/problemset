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
const int MXN = 750; // modify as needed
const int MXCELL = MXN * MXN + 5;

const int dr8[]{0, 1, 1, 1, 0, -1, -1, -1};
const int dc8[]{1, 1, 0, -1, -1, -1, 0, 1};

/*
 (1) Problem recap in simple terms:
     Given an N x N grid with distinct heights, a valley is a 4-connected region with no holes,
     where every border cell (4-adjacent to the region) is higher than every cell inside it. We
     need the sum of sizes of all valleys.

 
 (2) Things to notice and step-by-step derivation:
     - "No holes" is about the complement being 8-connected (pointwise-contiguous), including the
       infinite outside. So we track components in the complement using 8-direction adjacency.
     - Sweep heights from high to low in DSU A (8-dir + outside). Removing one cell from the
       complement can only split components, so the component count difference tells how many new
       holes are created at that height.
     - Sweep heights from low to high in DSU B (4-dir). For each growing 4-connected component,
       we accumulate how many holes it has; if it has 0 holes at a step, that region is a valley.

 
 (3) Solution outline:
     - Sort all cells by height (increasing).
     - Pass 1 (DSU A): add cells from high to low, union with higher 8-neighbors and outside; store
       compCnt[i] = #components after inserting cells i..end into the complement.
     - Pass 2 (DSU B): add cells from low to high, union with lower 4-neighbors; add
       (compCnt[i+1] - compCnt[i]) to the hole count of the current component; if holes == 0,
       add its size to the answer.
     - Time: O(N^2 log N), Memory: O(N^2)
 */

// grid size and heights
static int n;
static int h[MXN][MXN];

// sorted cell list (no dynamic arrays)
struct Cell {
	int r, c;
};
static Cell cells[MXN * MXN];

// compCnt[i] = DSU A component count after inserting cells i..tot-1 (+ outside)
static int compCnt[MXCELL];

struct DSU {
	int par[MXCELL], sz[MXCELL], holes[MXCELL];
	int numComp = 0;

	void init(int tot, int startComp) {
		for (int i = 0; i < tot; i++) {
			par[i] = i;
			sz[i] = 1;
			holes[i] = 0;
		}
		numComp = startComp;
	}

	int get(int x) {
		while (par[x] != x) {
			par[x] = par[par[x]];
			x = par[x];
		}
		return x;
	}

	void unite(int a, int b) {
		a = get(a), b = get(b);
		if (a == b) return;
		if (sz[a] < sz[b]) swap(a, b);
		numComp--;
		par[b] = a;
		sz[a] += sz[b];
		holes[a] += holes[b];
	}
};

static DSU dsuA, dsuB;

inline int id(int r, int c) { return r * n + c; }
inline bool inside(int r, int c) { return 0 <= r && r < n && 0 <= c && c < n; }

inline void solve() {
	cin >> n;
	for (int r = 0; r < n; r++) for (int c = 0; c < n; c++) cin >> h[r][c];

	int tot = n * n;
	for (int r = 0; r < n; r++) for (int c = 0; c < n; c++) cells[id(r, c)] = {r, c};

	sort(cells, cells + tot, [](const Cell& a, const Cell& b) {
		return h[a.r][a.c] < h[b.r][b.c];
	});

	// DSU A over (all cells + outside). Track components of the complement using 8-dir adjacency.
	int outside = tot;
	dsuA.init(tot + 1, 1);
	compCnt[tot] = 1;

	for (int i = tot - 1; i >= 0; i--) {
		int r = cells[i].r, c = cells[i].c;
		int cur = id(r, c);

		dsuA.numComp++; // activate this cell in the complement

		for (int d = 0; d < 8; d++) {
			int nr = r + dr8[d], nc = c + dc8[d];
			if (!inside(nr, nc)) {
				dsuA.unite(cur, outside);
			} else if (h[nr][nc] > h[r][c]) {
				dsuA.unite(cur, id(nr, nc));
			}
		}

		compCnt[i] = dsuA.numComp;
	}

	// DSU B over all cells (4-dir). holes[root] counts how many holes this region has.
	dsuB.init(tot, 0);

	ll ans = 0;
	for (int i = 0; i < tot; i++) {
		int r = cells[i].r, c = cells[i].c;
		int cur = id(r, c);

		for (int d = 0; d < 8; d += 2) { // skip diagonals
			int nr = r + dr8[d], nc = c + dc8[d];
			if (!inside(nr, nc)) continue;
			if (h[nr][nc] < h[r][c]) dsuB.unite(cur, id(nr, nc));
		}

		int rt = dsuB.get(cur);
		dsuB.holes[rt] += compCnt[i + 1] - compCnt[i];

		if (dsuB.holes[rt] == 0) ans += dsuB.sz[rt];
	}

	cout << ans << "\n";
}

int main() {
	setIO("valleys");
	solve(); // single case
}
