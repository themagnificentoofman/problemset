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
1) Problem recap (simple terms):
   We have n nodes. Each node i points to nxt[i]. Picking node i costs cost[i].
   We must pick nodes so every edge (i, nxt[i]) has at least one endpoint picked.
   Minimize total cost. (If nxt[i] = i, picking i is forced; the input code makes that cost 0.)

2) Things to notice and step-by-step:
   - Each connected component is a “functional graph”: trees feeding into one directed cycle.
   - If we “break” one cycle edge, the component becomes a tree (in the reverse-edge view).
   - On a tree, standard vertex-cover DP works:
       dp0[x] = min cost if x NOT picked (all children must be picked)
       dp1[x] = min cost if x picked (children can be either)
   - For a cycle, pick any cycle node u. Consider two cases:
       (A) force-pick u and break edge (u -> nxt[u])
       (B) force-pick nxt[u] and break edge (nxt[u] -> nxt[nxt[u]])
     The broken edge is always incident to the forced node, so it’s already covered.

3) Solution outline:
   - Build reverse edges children[v] = { i : nxt[i] = v }.
   - For each unprocessed component, find a cycle node with Floyd’s algorithm.
   - Run the tree DP twice (two forced roots) and add the smaller cost.
   - Time: O(n) overall (each component is processed twice, still linear).
*/

struct Solver {
	int n;
	vi nxt;
	vl cost;
	V<vi> child;

	vl dp0, dp1;
	vi seen;
	int timer = 0;

	Solver(int n_) : n(n_) {
		nxt.assign(n, 0);
		cost.assign(n, 0);
		child.assign(n, {});
		dp0.assign(n, 0);
		dp1.assign(n, 0);
		seen.assign(n, 0);
	}

	P<ll, vi> solveForced(int root) {
		int cutU = root;
		int cutV = nxt[root]; // in reverse edges, cutV has child cutU; we skip that one edge

		timer++;
		vi order;
		order.reserve(256);

		// DFS preorder from root in the reverse-edge tree
		stack<int> st;
		st.push(root);
		seen[root] = timer;

		while (!st.empty()) {
			int x = st.top(); st.pop();
			order.pb(x);

			for (int y : child[x]) {
				if (x == cutV && y == cutU) continue; // break the chosen cycle edge
				if (seen[y] == timer) continue;
				seen[y] = timer;
				st.push(y);
			}
		}

		// DP in reverse preorder => children before parent
		for (int it = sz(order) - 1; it >= 0; it--) {
			int x = order[it];
			ll take = cost[x];
			ll skip = 0;

			for (int y : child[x]) {
				if (x == cutV && y == cutU) continue;
				if (seen[y] != timer) continue;
				skip += dp1[y];
				take += min(dp0[y], dp1[y]);
			}
			dp0[x] = skip;
			dp1[x] = take;
		}

		// root is forced to be picked
		return {dp1[root], order};
	}
};

inline void solve() {
	int n;
	cin >> n;

	Solver S(n);
	for (int i = 0; i < n; i++) {
		cin >> S.nxt[i];
		--S.nxt[i];
	}
	for (int i = 0; i < n; i++) cin >> S.cost[i];

	for (int i = 0; i < n; i++) {
		S.child[S.nxt[i]].pb(i);
		if (S.nxt[i] == i) S.cost[i] = 0; // self-loop is forced; keep it free like the given code
	}

	vb done(n, false);
	ll tot = 0;

	for (int i = 0; i < n; i++) {
		if (done[i]) continue;

		// find a node on the cycle of this component (Floyd)
		int x = i, y = S.nxt[i];
		while (x != y) {
			x = S.nxt[x];
			y = S.nxt[S.nxt[y]];
		}

		auto [c1, vis1] = S.solveForced(x);
		auto [c2, vis2] = S.solveForced(S.nxt[x]);
		tot += min(c1, c2);

		for (int v : vis1) done[v] = true; // marks the whole component
	}

	cout << tot << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
