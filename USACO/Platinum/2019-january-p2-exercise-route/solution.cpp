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
     We have a tree (standard trails) plus extra edges (non-standard trails). Count how many
     simple cycles use exactly two extra edges.

 (2) Things to notice and step-by-step derivation:
     - Two extra edges form a valid cycle iff their paths in the tree overlap (share a tree edge).
     - Root the tree. For extra edge (u,v) with L = lca(u,v), split the tree path into u->L and v->L.
     - Count overlaps among these upward paths; this counts each valid pair once, except some pairs
       get counted twice.
     - Double counting happens only when two extra edges share the same LCA and use the same two
       child-edges out of that LCA.

 (3) Solution outline:
     - Build LCA (binary lifting) on the standard-tree.
     - For each extra edge, find L and the first child below L on the path to u and v; count these
       starts per child and per child-pair to pre-subtract the double-count cases.
     - Do one DFS to compute prefix sums: for each node, how many upward paths start on ancestors.
     - For each extra edge, add the number of starts along the two arms from L to u/v using the
       prefix sums; the earlier subtractions make this equal the number of overlapping pairs.
     - O((N+Q) log N) time, O(N+Q) memory.
 */

inline void solve() {
	int n, m; cin >> n >> m;
	int q = m - (n - 1);

	V<vi> adj(n);
	for (int i = 0; i < n - 1; i++) {
		int a, b; cin >> a >> b;
		--a, --b;
		adj[a].pb(b);
		adj[b].pb(a);
	}

	const int LOG = 20;
	V<AR<int, LOG>> up(n);
	for (int i = 0; i < n; i++) for (int j = 0; j < LOG; j++) up[i][j] = -1;

	vi depth(n, 0), order;
	order.reserve(n);

	{
		vi st;
		st.pb(0);
		up[0][0] = -1;
		depth[0] = 0;

		while (sz(st)) {
			int v = st.bk; st.ppb();
			order.pb(v);
			for (int to : adj[v]) if (to != up[v][0]) {
				up[to][0] = v;
				depth[to] = depth[v] + 1;
				st.pb(to);
			}
		}
	}

	for (int j = 1; j < LOG; j++) {
		for (int v = 0; v < n; v++) {
			int mid = up[v][j - 1];
			up[v][j] = (mid == -1 ? -1 : up[mid][j - 1]);
		}
	}

	auto lift = [&](int v, int k) -> int {
		for (int j = 0; j < LOG; j++) {
			if ((k >> j) & 1) v = up[v][j];
			if (v == -1) break;
		}
		return v;
	};

	auto lca = [&](int a, int b) -> int {
		if (depth[a] < depth[b]) swap(a, b);
		a = lift(a, depth[a] - depth[b]);
		if (a == b) return a;
		for (int j = LOG - 1; j >= 0; j--) {
			if (up[a][j] != up[b][j]) {
				a = up[a][j];
				b = up[b][j];
			}
		}
		return up[a][0];
	};

	auto childBelow = [&](int top, int bot) -> int {
		if (top == bot) return -1;
		int diff = depth[bot] - depth[top] - 1;
		return lift(bot, diff);
	};

	vi x(q), y(q), anc(q);
	vi numInters(n, 0);

	unordered_map<ll, int> pairCnt;
	pairCnt.reserve((size_t)q * 2 + 10);
	pairCnt.max_load_factor(0.7f);

	auto pack = [&](int a, int b) -> ll {
		/*
		 a,b < 2e5, so 20 bits is enough; key is unique for ordered pair (a,b).
		 */
		return (ll(a) << 20) | ll(b);
	};

	ll ans = 0;
	for (int i = 0; i < q; i++) {
		cin >> x[i] >> y[i];
		--x[i], --y[i];

		int u = x[i], v = y[i];
		anc[i] = lca(u, v);

		int cu = childBelow(anc[i], u);
		if (cu != -1) {
			ans -= ll(numInters[cu]) + 1;
			numInters[cu]++;
		}

		int cv = childBelow(anc[i], v);
		if (cv != -1) {
			ans -= ll(numInters[cv]) + 1;
			numInters[cv]++;
		}

		if (cu != -1 && cv != -1) {
			if (cu > cv) swap(cu, cv);
			ll key = pack(cu, cv);
			int &cnt = pairCnt[key];
			ans -= cnt;
			cnt++;
		}
	}

	vi psInters(n, 0);
	{
		vpi st;
		st.pb({0, 0});
		while (sz(st)) {
			auto [v, ps] = st.bk; st.ppb();
			psInters[v] = ps;
			for (int to : adj[v]) if (to != up[v][0]) {
				st.pb({to, ps + numInters[to]});
			}
		}
	}

	for (int i = 0; i < q; i++) {
		ans += ll(psInters[x[i]]) + ll(psInters[y[i]]) - 2LL * ll(psInters[anc[i]]);
	}

	cout << ans << "\n";
}

int main() {
	setIO("exercise");
	solve(); // single case
}
