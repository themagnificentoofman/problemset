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
const int MXN = (int)5e4+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     We have N points connected by M undirected roads with lengths. Points 1..C are
     charging stations, and points C+1..N are travel destinations. A destination is
     well-connected if its shortest-path distance to at least K distinct charging
     stations is <= R. Output how many destinations are well-connected, then list
     them in increasing order.

 (2) Things to notice and step-by-step derivation:
     - K <= 10, so for each node we only need to remember up to K distinct stations
       that can reach it within distance R.
     - Run Dijkstra on states (dist, node, sourceStation), starting from all C
       stations at dist = 0.
     - When we pop a state, we only "accept" it if:
       - this sourceStation is new for this node, and
       - the node has accepted fewer than K sources so far.
     - If a node already accepted K sources, any later state for that node has
       distance >= its K-th best, so it cannot help any other node reach K sources
       within distance R earlier than the accepted ones.

 (3) Solution outline:
     - Build adjacency list of the road network.
     - Initialize a min-heap with (0, station, station) for all stations.
     - Pop states in increasing distance; if dist > R, stop.
     - If the source is new for this node and the node has < K sources, record it
       and push neighbor states (still tagged with the same source).
     - Scan destinations C..N-1 and output those with exactly K recorded sources.
     - Time: O((N+M) * K * log(N*K)), Memory: O(N*K + M)
 */

struct State {
	ll d;
	int u;
	int src;
	bool operator>(const State& o) const { return d > o.d; }
};

inline void solve() {
	int n, m, c, k;
	ll r;
	cin >> n >> m >> c >> r >> k;

	V<vpi> adj(n);
	for (int i = 0; i < m; i++) {
		int u, v, w;
		cin >> u >> v >> w;
		--u; --v;
		adj[u].pb({v, w});
		adj[v].pb({u, w});
	}

	V<vi> seen(n);
	for (int i = 0; i < n; i++) seen[i].reserve(k);

	pqg<State> pq;
	for (int i = 0; i < c; i++) pq.push({0, i, i});

	for (; !pq.empty(); ) {
		State cur = pq.top();
		pq.pop();

		if (cur.d > r) break;

		if (sz(seen[cur.u]) >= k) continue;

		bool has = false;
		for (int x : seen[cur.u]) {
			if (x == cur.src) {
				has = true;
				break;
			}
		}
		if (has) continue;

		seen[cur.u].pb(cur.src);

		for (auto e : adj[cur.u]) {
			int v = e.fi;
			int w = e.se;
			ll nd = cur.d + (ll)w;
			if (nd <= r && sz(seen[v]) < k) pq.push({nd, v, cur.src});
		}
	}

	int ans = 0;
	for (int i = c; i < n; i++) if (sz(seen[i]) == k) ans++;

	cout << ans << '\n';
	for (int i = c; i < n; i++) {
		if (sz(seen[i]) == k) cout << i + 1 << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
