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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   We have N farms and M roads. Farms are closed one by one in a given order.
   After each closure, output whether the remaining open farms are all connected.

2) Things to notice + step-by-step:
   - Closing is annoying, but opening is easy.
   - Process the closure order in reverse: start with no farms open, then open them back up.
   - Use DSU to maintain connected components among currently open farms.
   - After opening a farm, union it with its open neighbors and update the component count.

3) Solution outline:
   - Read graph and closing order; reverse the order to get opening order.
   - Maintain open[i] and DSU; keep cc = number of connected components among open nodes.
   - After each open, record (cc == 1 ? "YES" : "NO"), then reverse answers for output.
   - Time: O((N+M) α(N)).
*/

struct DSU {
	vi e;
	DSU(int n = 0) { init(n); }

	void init(int n) { e.assign(n, -1); }

	int get(int x) { return e[x] < 0 ? x : e[x] = get(e[x]); }

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

	V<vi> adj(n);
	for (int i = 0; i < m; i++) {
		int u, v;
		cin >> u >> v;
		--u, --v;
		adj[u].pb(v);
		adj[v].pb(u);
	}

	vi order(n);
	for (int i = 0; i < n; i++) {
		cin >> order[i];
		--order[i];
	}
	reverse(all(order)); // now it's opening order

	DSU dsu(n);
	vb open(n, false);
	vs ans;
	ans.reserve(n);

	int cc = 0;
	for (int idx = 0; idx < n; idx++) {
		int x = order[idx];
		open[x] = true;
		cc++; // new component by itself

		for (int y : adj[x]) {
			if (!open[y]) continue;
			if (dsu.unite(x, y)) cc--;
		}

		ans.pb(cc == 1 ? "YES" : "NO");
	}

	reverse(all(ans));
	for (auto &s : ans) cout << s << "\n";
}

int main() {
	setIO("closing");
	solve(); // single case
}
