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
     We have N farms, initially all active and with no roads. We apply Q updates that deactivate
     a farm, add a road between two active farms, or remove a previously added road. A farm is
     "relevant" if it is active or can reach some active farm using roads. For each farm, output
     the largest i (0..Q) such that it is relevant after the i-th update.

 (2) Things to notice and step-by-step derivation:
     - Once a farm becomes irrelevant in forward time, it can never become relevant again (roads
       are only added between active farms, so dead components can't reconnect to actives).
     - Process time backwards from state Q: undoing updates only adds roads back and re-activates
       farms, so the set of relevant farms only grows.
     - Therefore, the first backward time t (state after t updates) when a farm becomes relevant
       equals the maximum forward i when it is relevant.
     - When a node becomes relevant, its entire current connected component becomes relevant too,
       so we can flood-fill the component once and stamp its answer.

 (3) Solution outline:
     - Read all updates; store the list of added roads and mark which ones are removed later.
     - Build the final graph using roads that survive to time Q; mark farms never deactivated.
     - Flood-fill from each active farm in the final state and stamp answer = Q.
     - Walk updates backward for t = Q-1..1:
       - Undo D: activate that farm; flood-fill from it and stamp answer = t.
       - Undo R: add that road back; if either endpoint is already stamped, flood-fill from both.
     - Unstamped farms have answer 0.
     - Time: O(N+Q); Memory: O(N+Q).
 */

struct Edge {
	int u, v;
};

inline void solve() {
	int n, q;
	cin >> n >> q;

	if (q == 0) {
		for (int i = 1; i <= n; i++) cout << 0 << "\n";
		return;
	}

	V<vi> adj(n + 1);
	vi ans(n + 1, 0);

	V<Edge> addedEdges;
	addedEdges.reserve(q);
	vb alive;
	alive.reserve(q);

	V<Edge> removed(q, {0, 0});
	vb isRemoved(q, false);
	vi deactivated(q, 0);

	vb active(n + 1, true);

	for (int t = 0; t < q; t++) {
		char type;
		cin >> type;
		if (type == 'D') {
			int x;
			cin >> x;
			deactivated[t] = x;
			active[x] = false;
		} else if (type == 'A') {
			int a, b;
			cin >> a >> b;
			addedEdges.pb({a, b});
			alive.pb(true);
		} else { // 'R'
			int e;
			cin >> e;
			--e;
			removed[t] = addedEdges[e];
			isRemoved[t] = true;
			alive[e] = false;
		}
	}

	for (int i = 0; i < sz(addedEdges); i++) if (alive[i]) {
		int u = addedEdges[i].u, v = addedEdges[i].v;
		adj[u].pb(v);
		adj[v].pb(u);
	}

	auto markComponent = [&](int s, int t) {
		if (ans[s]) return;
		vi st;
		ans[s] = t;
		st.pb(s);
		while (sz(st)) {
			int u = st.bk;
			st.ppb();
			for (int v : adj[u]) {
				if (!ans[v]) {
					ans[v] = t;
					st.pb(v);
				}
			}
		}
	};

	for (int i = 1; i <= n; i++) if (active[i]) {
		markComponent(i, q);
	}

	for (int t = q - 1; t > 0; t--) {
		if (deactivated[t]) {
			markComponent(deactivated[t], t);
		} else if (isRemoved[t]) {
			int a = removed[t].u, b = removed[t].v;
			adj[a].pb(b);
			adj[b].pb(a);
			if (ans[a] || ans[b]) {
				markComponent(a, t);
				markComponent(b, t);
			}
		}
	}

	for (int i = 1; i <= n; i++) cout << ans[i] << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
