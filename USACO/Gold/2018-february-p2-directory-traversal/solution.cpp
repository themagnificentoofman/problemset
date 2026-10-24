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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   Pick a directory to stand in so the sum of lengths of relative paths to every file is as small as possible.

2) Things to notice + derivation:
   - Files are leaves; directories are internal nodes.
   - From a directory v, every file in child subtree u needs +len(name(u)) plus +1 more if u is a directory (the '/').
   - If we “reroot” from parent to a child u, every file outside u’s subtree gains an extra "../" (length 3).
   - So compute:
     inside[v] = sum of path lengths from v to files in v’s subtree
     outside[v] = sum of path lengths from v to files NOT in v’s subtree

3) Solution outline:
   - Iterative DFS to get an order; process it in reverse to compute inside[] and leaf counts.
   - Process in forward order to compute outside[] for children using rerooting formula.
   - Answer is min(inside[d] + outside[d]) over directories d.
*/

inline void solve() {
	int n; cin >> n;

	struct Node {
		bool is_file = false;
		int len = 0;
		vi ch;
	};

	V<Node> a(n);
	int tot_files = 0;

	for (int i = 0; i < n; i++) {
		str name;
		int m;
		cin >> name >> m;
		a[i].len = sz(name);
		a[i].is_file = (m == 0);
		if (a[i].is_file) tot_files++;

		a[i].ch.rsz(m);
		for (int j = 0; j < m; j++) {
			int x; cin >> x; --x;
			a[i].ch[j] = x;
		}
	}

	// build a traversal order from the root (0) without recursion
	vi order;
	order.reserve(n);
	vi st;
	st.pb(0);
	while (sz(st)) {
		int v = st.bk; st.ppb();
		order.pb(v);
		for (int u : a[v].ch) st.pb(u);
	}

	vi leaves(n, 0);
	vl inside(n, 0);

	// postorder DP: compute leaves[] and inside[]
	for (int idx = sz(order) - 1; idx >= 0; idx--) {
		int v = order[idx];
		if (a[v].is_file) {
			leaves[v] = 1;
			continue;
		}
		ll sum = 0;
		int cnt = 0;
		for (int u : a[v].ch) {
			cnt += leaves[u];
			int add = a[u].len + (a[u].is_file ? 0 : 1); // +1 for the '/' if child is a directory
			sum += inside[u] + (ll)add * leaves[u];
		}
		leaves[v] = cnt;
		inside[v] = sum;
	}

	vl outside(n, 0);

	// reroot DP: compute outside[] for children
	for (int v : order) {
		for (int u : a[v].ch) {
			int add = a[u].len + (a[u].is_file ? 0 : 1);
			ll contrib = inside[u] + (ll)add * leaves[u];
			ll same = inside[v] - contrib; // files in v-subtree but not u-subtree, measured from v
			outside[u] = outside[v] + same + 3LL * (tot_files - leaves[u]); // "../" for every file outside u-subtree
		}
	}

	ll ans = BIG;
	for (int i = 0; i < n; i++) {
		if (!a[i].is_file) ans = min(ans, inside[i] + outside[i]); // must stand in a directory
	}
	cout << ans << "\n";
}

int main() {
	setIO("dirtraverse");
	solve(); // single case
}
