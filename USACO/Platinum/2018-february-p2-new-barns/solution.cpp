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
     We build barns one-by-one. Each new barn is either a new standalone tree, or it attaches to
     an existing barn. For query operations, we must output the maximum distance from a given
     barn to any barn in its current connected component.

 (2) Things to notice and step-by-step derivation:
     - In any tree, the farthest node from u is always one of the two endpoints of the tree's
       diameter.
     - So if we maintain the diameter endpoints (a, b) for each component, the query answer is
       max(dist(u, a), dist(u, b)).
     - We only add leaves, so we can maintain depths and binary-lifting parents to get LCA and
       distances in O(log q).

 (3) Solution outline:
     - Keep binary lifting table up[k][v] and depth[v] as nodes are added.
     - For each component, store its root id and current diameter endpoints (a, b).
     - When adding a node, update its component and possibly extend the diameter.
     - For query u, return max(dist(u, a), dist(u, b)).
     - Time: O(q log q), Memory: O(q log q)
 */

struct Forest {
	int q, LOG, cur;
	V<vi> up;
	vi dep, compRoot;
	vpi diam; // diam[r] = {a,b} for component whose id root is r

	Forest(int q) : q(q), LOG(1), cur(-1) {
		while ((1 << LOG) <= q) LOG++;
		up.assign(LOG, vi(q, 0));
		dep.assign(q, 0);
		compRoot.assign(q, 0);
		diam.assign(q, mp(0, 0));
	}

	int lca(int u, int v) {
		if (dep[u] < dep[v]) swap(u, v);
		int diff = dep[u] - dep[v];
		for (int k = 0; k < LOG; k++) if ((diff >> k) & 1) u = up[k][u];
		if (u == v) return u;
		for (int k = LOG - 1; k >= 0; k--) {
			if (up[k][u] != up[k][v]) {
				u = up[k][u];
				v = up[k][v];
			}
		}
		return up[0][u];
	}

	int dist(int u, int v) {
		int w = lca(u, v);
		return dep[u] + dep[v] - 2 * dep[w];
	}

	void addNode(int par) {
		cur++;
		int v = cur;

		if (par == -1) {
			compRoot[v] = v;
			dep[v] = 0;
			for (int k = 0; k < LOG; k++) up[k][v] = v;
			diam[v] = mp(v, v);
			return;
		}

		up[0][v] = par;
		for (int k = 1; k < LOG; k++) up[k][v] = up[k - 1][up[k - 1][v]];
		dep[v] = dep[par] + 1;

		int r = compRoot[par];
		compRoot[v] = r;

		int a = diam[r].fi, b = diam[r].se;
		int dAB = dist(a, b);
		int dVA = dist(v, a);
		int dVB = dist(v, b);

		if (dVA > dAB) diam[r] = mp(a, v);
		else if (dVB > dAB) diam[r] = mp(b, v);
	}

	int query(int u) {
		int r = compRoot[u];
		int a = diam[r].fi, b = diam[r].se;
		return max(dist(u, a), dist(u, b));
	}
};

inline void solve() {
	int q;
	cin >> q;

	Forest f(q);
	for (int i = 0; i < q; i++) {
		char tp;
		int x;
		cin >> tp >> x;

		if (tp == 'B') {
			if (x == -1) f.addNode(-1);
			else f.addNode(x - 1);
		} else {
			cout << f.query(x - 1) << "\n";
		}
	}
}

int main() {
	setIO("newbarn");
	solve(); // single case
}
