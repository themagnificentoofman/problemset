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
     We have an undirected friendship graph. A “friendship group” is any connected induced subgraph.
     Its strength is (minimum internal degree in the group) * (group size). Find the maximum strength.

 (2) Things to notice and step-by-step derivation:
     - For any k, the k-core is the largest set where every node has degree >= k within the set.
     - Any group with min internal degree >= k must be contained in the k-core (removing outsiders
       never hurts degrees that were counted inside the group).
     - So for each k, the best strength is k * (largest connected component size in the k-core).
     - If we repeatedly remove the current minimum-degree node (degeneracy peeling), then when a
       node is removed with degree d, the remaining nodes are exactly the d-core at that moment.

 (3) Solution outline:
     - Peeling phase: maintain (deg, node) in a set, repeatedly remove the smallest; record the
       removed node order and its degree d (the current min degree).
     - Reverse phase: add nodes back in reverse order; DSU-union active edges to maintain the
       largest component size in the current active subgraph.
     - At each step i, candidate = (recorded min degree d_i) * (largest component size); take max.
     - Time: O((n+m) log n); Memory: O(n+m).
 */

struct DSU {
	vi e;
	DSU(int n = 0) { init(n); }
	void init(int n) { e.assign(n, -1); }
	int get(int x) { return e[x] < 0 ? x : e[x] = get(e[x]); }
	int size(int x) { return -e[get(x)]; }
	bool unite(int a, int b) {
		a = get(a), b = get(b);
		if (a == b) return false;
		if (e[a] > e[b]) swap(a, b);
		e[a] += e[b];
		e[b] = a;
		return true;
	}
};

inline void solve() {
	int n, m;
	cin >> n >> m;

	V<vi> g(n);
	vi deg(n, 0);
	for (int i = 0; i < m; i++) {
		int u, v;
		cin >> u >> v;
		--u; --v;
		g[u].pb(v);
		g[v].pb(u);
		deg[u]++; deg[v]++;
	}

	set<pi> s;
	for (int u = 0; u < n; u++) s.ins({deg[u], u});

	V<char> active(n, 1);
	vi ord, minDeg;
	ord.reserve(n);
	minDeg.reserve(n);

	for (int iters = 0; iters < n; iters++) {
		auto it = s.begin();
		int d = it->fi, u = it->se;
		s.erase(it);

		active[u] = 0;
		ord.pb(u);
		minDeg.pb(d);

		for (int v : g[u]) if (active[v]) {
			s.erase({deg[v], v});
			deg[v]--;
			s.ins({deg[v], v});
		}
	}

	reverse(all(ord));
	reverse(all(minDeg));

	active.assign(n, 0);
	DSU dsu(n);

	int mx = 0;
	ll ans = 0;

	for (int i = 0; i < n; i++) {
		int u = ord[i];
		active[u] = 1;

		for (int v : g[u]) if (active[v]) dsu.unite(u, v);

		mx = max(mx, dsu.size(u));
		ans = max(ans, 1LL * mx * minDeg[i]);
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
