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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   For each query (k, v), count how many other videos are reachable from v using only edges with relevance >= k.

2) Things to notice + derivation:
   A path is valid iff every edge on it has weight >= k. If we keep only edges with weight >= k, then the answer
   is just (size of v's connected component) - 1. Process queries in descending k, and as k decreases, we only add
   more edges, so a DSU works offline.

3) Solution outline (+ complexity):
   Sort edges by weight descending, sort queries by k descending, sweep: add all edges with w >= k into DSU, then
   answer each query by component_size(v)-1. Time: O((N+Q) log (N+Q)), DSU is near-linear.
*/

struct DSU {
	vi e; // parent/size array: negative means root with size = -e[root]
	DSU(int n=0){ init(n); }

	void init(int n) { e.assign(n, -1); }

	int get(int x) { return e[x] < 0 ? x : e[x] = get(e[x]); }

	int size(int x) { return -e[get(x)]; }

	bool unite(int a, int b) {
		a = get(a), b = get(b);
		if (a == b) return false;
		if (e[a] > e[b]) swap(a, b); // a has larger size (more negative)
		e[a] += e[b];
		e[b] = a;
		return true;
	}
};

struct Edge {
	int w, u, v;
};
struct Query {
	int k, v, idx;
};

inline void solve() {
	int n, q; cin >> n >> q;

	V<Edge> edges;
	edges.reserve(n-1);
	for (int i = 0; i < n-1; i++) {
		int u, v, w; cin >> u >> v >> w;
		--u; --v;
		edges.pb({w, u, v});
	}

	V<Query> qs;
	qs.reserve(q);
	for (int i = 0; i < q; i++) {
		int k, v; cin >> k >> v;
		--v;
		qs.pb({k, v, i});
	}

	sort(all(edges), [](const Edge& a, const Edge& b){
		return a.w > b.w;
	});
	sort(all(qs), [](const Query& a, const Query& b){
		return a.k > b.k;
	});

	DSU dsu(n);
	vi ans(q);

	int p = 0; // pointer into edges
	for (auto qu : qs) {
		while (p < sz(edges) && edges[p].w >= qu.k) {
			dsu.unite(edges[p].u, edges[p].v);
			p++;
		}
		ans[qu.idx] = dsu.size(qu.v) - 1;
	}

	for (int i = 0; i < q; i++) cout << ans[i] << '\n';
}

int main() {
	setIO("mootube");
	solve(); // single case
}
