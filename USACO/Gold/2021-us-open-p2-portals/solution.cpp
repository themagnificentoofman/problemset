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
     We have 2N portals as nodes. At each vertex v, its 4 incident portals can be paired in
     two disjoint pairs; switching within a pair is free. We may pay c_v once to reorder the
     four portals (change which two pairs exist). Find the minimum total cost so every
     location (vertex, portal) is reachable from every other.

 (2) Things to notice and step-by-step derivation:
     - Think about connectivity between portal-nodes: if portals are in the same connected
       component, we can move between their corresponding locations via allowed operations.
     - For each vertex v, we always get two free connections: (p1 <-> p2) and (p3 <-> p4).
     - Paying c_v lets us "swap partners" so that the two free pairs can be turned into a
       different pairing; in MST terms, this acts like an extra edge of weight c_v that can
       connect the two free-pair groups.
     - This reduces to building an undirected graph on the 2N portals with 3 edges per
       vertex, then taking a minimum spanning tree.

 (3) Solution outline:
     - Build edges on portal nodes (0-indexed):
       - (p1, p2, 0), (p3, p4, 0), (p1, p3, c_v).
     - Run Kruskal on the 2N nodes and sum the chosen edge weights.
     - Time: O(N log N); Memory: O(N).
 */

struct DSU {
	vi p, s;
	DSU(int n = 0) { init(n); }
	void init(int n) {
		p.rsz(n);
		s.assign(n, 1);
		iota(all(p), 0);
	}
	int f(int x) { return p[x] == x ? x : (p[x] = f(p[x])); }
	bool unite(int a, int b) {
		a = f(a), b = f(b);
		if (a == b) return false;
		if (s[a] < s[b]) swap(a, b);
		p[b] = a;
		s[a] += s[b];
		return true;
	}
};

struct Edge {
	int u, v, w;
};

inline void solve() {
	int n;
	cin >> n;

	int m = 2 * n;
	V<Edge> e;
	e.reserve(3 * n);

	for (int i = 0; i < n; i++) {
		int c, p1, p2, p3, p4;
		cin >> c >> p1 >> p2 >> p3 >> p4;
		p1--, p2--, p3--, p4--;

		e.pb({p1, p2, 0});
		e.pb({p3, p4, 0});
		e.pb({p1, p3, c});
	}

	sort(all(e), [](const Edge& a, const Edge& b) {
		return a.w < b.w;
	});

	DSU dsu(m);
	ll ans = 0;
	for (auto [u, v, w] : e) {
		if (dsu.unite(u, v)) ans += w;
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
