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
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // ind of 1st elem >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // ind of 1st elem > b
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
const ll BIG = (ll)4e18; // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)2e5+5; // modify as needed

/*
 (1) Problem recap (simple):
     We remove nodes 1..N in order. Removing a '0' node just deletes it. Removing a '1' node deletes it,
     but also adds edges between every pair of its neighbors. Before each removal t, output how many
     unordered pairs of remaining nodes can reach each other.

 (2) Key idea / derivation:
     Work in reverse time (i = N..1), where the “currently remaining” nodes are exactly {i..N}.
     - A removed '0' node truly disappears and can’t be used in paths, so we only “activate” 0-nodes
       when we reach them in the reverse sweep.
     - A removed '1' node has the same effect on connectivity among other nodes as if it could still be
       used as an intermediate node (because its neighbors get connected into a clique when it’s removed).
       So we can treat all '1' nodes as always available for connectivity, but only start counting them as
       endpoints when we reach their index in the reverse sweep.
     Use a DSU over all nodes, and maintain for each component:
       cnt = number of currently-alive endpoints (nodes in {i..N}) in that component.
     Then total reachable pairs = sum over components C(cnt,2). We maintain this incrementally:
       - When activating a '1' node: it joins as an endpoint in its current component, adding cnt pairs.
       - When unioning two components: we add cntA * cntB new pairs.

 (3) Solution outline:
     - Read graph and s. DSU has cnt[v]=1 for 0-nodes, cnt[v]=0 for 1-nodes.
     - Union all edges between 1-nodes (they’re always “present” for connectivity).
     - Sweep i from N-1 down to 0:
         * if s[i]=='1': pairs += cnt[comp(i)], then cnt[comp(i)]++
         * else: union i with neighbors that are already usable (skip left 0-nodes not activated yet),
                 and for each successful union add cntA*cntB to pairs.
       Record pairs; reverse and print.
     - Time: O((N+M) α(N)), Memory: O(N+M).
*/

struct DSU {
	vi e;      // e[x] < 0 => size = -e[x], else parent
	vl cnt;    // cnt[root] = number of "alive endpoints" in this component

	DSU(int n) : e(n, -1), cnt(n, 1) {}

	int get(int x) {
		if (e[x] < 0) return x;
		return e[x] = get(e[x]);
	}

	bool unite(int a, int b) {
		a = get(a), b = get(b);
		if (a == b) return false;
		if (-e[a] < -e[b]) swap(a, b);
		e[a] += e[b];
		e[b] = a;
		cnt[a] += cnt[b];
		return true;
	}

	void addCnt(int x, ll delta) {
		x = get(x);
		cnt[x] += delta;
	}
};

inline void solve() {
	int n, m;
	str s;
	cin >> n >> m >> s;

	V<vi> adj(n);
	adj.assign(n, {});
	DSU dsu(n);

	// '1' nodes are "connectors" but not counted until activated.
	for (int i = 0; i < n; i++) if (s[i] == '1') dsu.cnt[i] = 0;

	for (int i = 0; i < m; i++) {
		int a, b;
		cin >> a >> b;
		--a; --b;
		adj[a].pb(b);
		adj[b].pb(a);
		if (s[a] == '1' && s[b] == '1') dsu.unite(a, b); // always-on connectivity among 1-nodes
	}

	ll pairs = 0;
	vl out; out.reserve(n);

	for (int i = n - 1; i >= 0; i--) {
		if (s[i] == '1') {
			int r = dsu.get(i);
			pairs += dsu.cnt[r];     // new pairs formed with existing alive endpoints in this component
			dsu.cnt[r]++;            // activate this node as an endpoint
		} else {
			for (int j : adj[i]) {
				// left 0-nodes are not active yet in the reverse sweep, so can't be used
				if (j < i && s[j] == '0') continue;

				int a = dsu.get(i), b = dsu.get(j);
				if (a == b) continue;

				pairs += dsu.cnt[a] * dsu.cnt[b];
				dsu.unite(a, b);
			}
		}
		out.pb(pairs);
	}

	reverse(all(out));
	for (ll x : out) cout << x << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
