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
(1) Problem recap (simple):
- Cows at each field walk to the barn (field 1) along a shortest-time route.
- If there are multiple shortest routes, they choose the lexicographically smallest path (from their field toward the barn).
- We can add one shortcut from barn to some field with time T. Maximize total time saved.

(2) Things to notice + derivation steps:
- Compute shortest distances dist[] to the barn.
- Tie-break: for a node v, among all shortest paths, pick the one whose next step toward the barn is smallest index.
  So parent[v] (next hop toward barn) should be the smallest-index neighbor u with dist[u] + w = dist[v].
- Once parent[] is fixed, every cow’s route is on this parent tree.
  Total cows passing through v = sum of cows in v’s subtree in this tree.
- Avoid backtracking from every node (O(N^2)): instead push subtree sums upward once (O(N)).

(3) Solution outline:
- Dijkstra from barn to get dist[]; on distance ties update parent[v] to smaller u (no need to push again).
- Build cnt[v]=cows[v], then process nodes in decreasing dist order: cnt[parent[v]] += cnt[v].
- Best savings at v: cnt[v] * (dist[v] - T). Take max.
*/

inline void solve() {
	int n, m;
	ll T;
	cin >> n >> m >> T;

	vl cows(n);
	for (int i = 0; i < n; i++) cin >> cows[i];

	V<vpi> adj(n);
	for (int i = 0; i < m; i++) {
		int u, v, w;
		cin >> u >> v >> w;
		--u; --v;
		adj[u].pb({v, w});
		adj[v].pb({u, w});
	}

	vl dist(n, BIG);
	vi par(n, -1);      // next hop from v toward barn
	vb done(n, false);  // finalized in Dijkstra (so order has no duplicates)

	using pli = P<ll,int>; // (dist, node)
	pqg<pli> pq;

	dist[0] = 0;
	pq.push({0, 0});

	vi order; order.reserve(n); // nodes finalized in nondecreasing dist

	while (sz(pq)) {
		auto [d, u] = pq.top(); pq.pop();
		if (d != dist[u]) continue;
		if (done[u]) continue;
		done[u] = true;
		order.pb(u);

		for (auto [v, w] : adj[u]) {
			ll nd = d + (ll)w;
			if (nd < dist[v]) {
				dist[v] = nd;
				par[v] = u;
				pq.push({nd, v});
			} else if (nd == dist[v] && (par[v] == -1 || u < par[v])) {
				// tie-break only changes parent choice; distance unchanged => do NOT push
				par[v] = u;
			}
		}
	}

	vl cnt = cows;
	for (int i = sz(order)-1; i >= 0; i--) {
		int v = order[i];
		if (par[v] != -1) cnt[par[v]] += cnt[v];
	}

	ll ans = 0;
	for (int v = 1; v < n; v++) {
		if (dist[v] > T) ans = max(ans, cnt[v] * (dist[v] - T));
	}
	cout << ans << "\n";
}

int main() {
	setIO("shortcut");
	solve(); // single case
}