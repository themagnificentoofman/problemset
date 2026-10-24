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
const int MXN = (int)1e5+5; // max N (and max color) for this problem

/*
 (1) Problem recap in simple terms:
     We repeatedly splash a subtree with a color (colors stack and all remain visible). For a query
     node x, output the sum of "how many distinct colors" over all nodes in x's subtree.

 (2) Things to notice and step-by-step derivation:
     - For a fixed color, only the highest painted nodes matter: if a node is painted, painting any
       descendant with the same color is useless, and any painted nodes strictly inside a newly
       painted subtree get overridden by that new highest node.
     - Maintain, for each color, an ordered set of these "highest" nodes by Euler in-time.
     - Let these highest nodes over all colors be "special nodes". Then the query answer for x can
       be written as:
       - sub[x] * (number of special nodes on the path root -> x)
       - plus sum(sub[y]) for special nodes y strictly inside x's subtree
     - Both parts are doable with two BITs on Euler order.

 (3) Solution outline:
     - Euler-tour the tree to get st[x], en[x], sub[x] so each subtree is a contiguous interval.
     - For each color c, keep map<st[node], node> of special nodes of that color.
       - Add x: if an existing special ancestor covers x, ignore.
       - Otherwise remove all special nodes in x's subtree, then insert x.
     - Maintain two BITs:
       - bitA: range add on [st[v], en[v]] for each special v, point query at st[x].
       - bitB: point add sub[v] at st[v] for each special v, range sum on (st[x], en[x]].
     - Answer query x with sub[x]*bitA.point(st[x]) + bitB.sum(st[x]+1..en[x]).
     - Time complexity: O((N+Q) log N); Memory complexity: O(N).
 */

struct BIT {
	int n = 0;
	vl bit;

	void init(int _n) {
		n = _n;
		bit.assign(n + 1, 0);
	}

	void add(int idx, ll val) {
		for (; idx <= n; idx += idx & -idx) bit[idx] += val;
	}

	ll sumPrefix(int idx) {
		ll res = 0;
		for (; idx > 0; idx -= idx & -idx) res += bit[idx];
		return res;
	}

	ll query(int l, int r) {
		if (l > r) return 0;
		return sumPrefix(r) - sumPrefix(l - 1);
	}
};

int n, q;
V<vi> adj;

int timer;
vi st, en, subSize;

BIT bitA, bitB;
V<map<int,int>> col;

static inline void dfsEuler(int root = 1) {
	timer = 0;
	vi par(n + 1, 0), it(n + 1, 0);
	V<int> stk;
	stk.pb(root);
	par[root] = 0;

	while (!stk.empty()) {
		int x = stk.bk;
		if (it[x] == 0) st[x] = ++timer;

		if (it[x] < sz(adj[x])) {
			int y = adj[x][it[x]++];
			if (y == par[x]) continue;
			par[y] = x;
			stk.pb(y);
		} else {
			en[x] = timer;
			subSize[x] = en[x] - st[x] + 1;
			stk.ppb();
		}
	}
}

static inline void updSpecial(int x, ll delta) {
	bitA.add(st[x], delta);
	bitA.add(en[x] + 1, -delta);
	bitB.add(st[x], delta * subSize[x]);
}

inline void solve() {
	cin >> n >> q;
	adj.assign(n + 1, {});
	for (int i = 0; i < n - 1; i++) {
		int u, v;
		cin >> u >> v;
		adj[u].pb(v);
		adj[v].pb(u);
	}

	st.assign(n + 1, 0);
	en.assign(n + 1, 0);
	subSize.assign(n + 1, 0);
	dfsEuler(1);

	bitA.init(n + 2);
	bitB.init(n + 2);

	col.assign(MXN, {});

	for (int qq = 0; qq < q; qq++) {
		int t;
		cin >> t;
		if (t == 1) {
			int x, c;
			cin >> x >> c;

			auto& mp = col[c];
			auto it = mp.ub(st[x]);

			if (it != mp.begin()) {
				auto prv = prev(it);
				if (en[prv->se] >= en[x]) continue;
			}

			while (it != mp.end() && en[it->se] <= en[x]) {
				updSpecial(it->se, -1);
				it = mp.erase(it);
			}

			mp[st[x]] = x;
			updSpecial(x, 1);
		} else {
			int x;
			cin >> x;
			ll above = bitA.sumPrefix(st[x]);
			ll below = bitB.query(st[x] + 1, en[x]);
			cout << 1LL * subSize[x] * above + below << '\n';
		}
	}
}

int main() {
	setIO("snowcow");
	solve(); // single case
}
