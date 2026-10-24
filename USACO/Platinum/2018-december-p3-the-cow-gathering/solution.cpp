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
const int MXN = (int)1e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     We have a friendship tree and M "must leave before" constraints. Cows leave one by one, and as
     long as at least two cows remain, every remaining cow must still have at least one remaining
     friend. For each cow, decide if she could be the last to leave.

 (2) Things to notice and step-by-step derivation:
     - A cow can be removed only when her current degree in the remaining graph is <= 1 (otherwise we
       would split the tree and risk isolating a cow).
     - A cow also cannot leave until all constraints of the form (x -> cow) are satisfied.
     - We can simulate removing N-1 cows by repeatedly taking any "free" cow:
       degree <= 1 and remaining incoming constraints = 0. If we get stuck, no valid order exists.
     - After a successful peel, let r be the final remaining cow. Any cow with an outgoing constraint
       (cow -> someone) can never be last, and it also blocks cows past it in the tree.

 (3) Solution outline:
     - Build the tree and store outgoing constraints (a -> b), plus indegree[b].
     - Leaf/topo peeling: keep a growing list of cows with degree <= 1 and indegree = 0; remove N-1
       of them, updating neighbor degrees and constraint indegrees.
     - If peeling fails, output 0 for all cows.
     - Let r be the final cow in the peeling order; DFS/BFS from r but stop at cows that have any
       outgoing constraint. Mark all visited cows as answers.
     - Time: O(N + M), Memory: O(N + M)
 */

inline void solve() {
	int n, m;
	cin >> n >> m;

	V<vi> adj(n), outCons(n);
	vi deg(n, 0), inDeg(n, 0);

	for (int i = 0; i < n - 1; i++) {
		int a, b;
		cin >> a >> b;
		--a, --b;
		adj[a].pb(b);
		adj[b].pb(a);
		deg[a]++, deg[b]++;
	}

	for (int i = 0; i < m; i++) {
		int a, b;
		cin >> a >> b;
		--a, --b;
		outCons[a].pb(b);
		inDeg[b]++;
	}

	vi order;
	vb added(n, false);
	for (int i = 0; i < n; i++) {
		if (deg[i] <= 1 && inDeg[i] == 0) {
			order.pb(i);
			added[i] = true;
		}
	}

	for (int idx = 0; idx < n - 1; idx++) {
		if (idx == sz(order)) {
			for (int i = 0; i < n; i++) cout << 0 << "\n";
			return;
		}
		int u = order[idx];

		for (int v : adj[u]) {
			deg[v]--;
			if (deg[v] <= 1 && inDeg[v] == 0 && !added[v]) {
				order.pb(v);
				added[v] = true;
			}
		}
		for (int v : outCons[u]) {
			inDeg[v]--;
			if (deg[v] <= 1 && inDeg[v] == 0 && !added[v]) {
				order.pb(v);
				added[v] = true;
			}
		}
	}

	int r = order[n - 1];

	vb ans(n, false);
	stack<pi> st;
	st.push({r, -1});
	while (!st.empty()) {
		auto [u, p] = st.top();
		st.pop();

		if (sz(outCons[u]) > 0) continue; // u must leave before someone, so it can't be last
		ans[u] = true;

		for (int v : adj[u]) {
			if (v == p) continue;
			st.push({v, u});
		}
	}

	for (int i = 0; i < n; i++) cout << ans[i] << "\n";
}

int main() {
	setIO("gathering");
	solve(); // single case
}
