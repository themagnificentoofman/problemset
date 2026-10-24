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
- We have a tree of N barns, each must be painted color 0/1/2.
- Adjacent barns must have different colors.
- M barns are pre-painted (forced to a specific color). Count valid paintings mod 1e9+7.

(2) Key observations / derivation:
- Root the tree at 1. Let dp[v][c] = # ways to paint subtree of v if v has color c.
- If v is forced to color c*, then dp[v][c*]=1 and dp[v][other]=0 before combining children.
- For each child u of v:
  dp[v][0] *= dp[u][1]+dp[u][2]
  dp[v][1] *= dp[u][0]+dp[u][2]
  dp[v][2] *= dp[u][0]+dp[u][1]
  (all mod MOD)
- Do a parent-order DFS to get an order, then process nodes in reverse (postorder).

(3) Solution outline:
- Read tree, store adjacency list.
- Initialize dp[v]={1,1,1}, apply forced colors by zeroing the other two.
- Build parent[] and an order[] with an iterative DFS from root; process order in reverse to compute dp.
- Answer = (dp[1][0]+dp[1][1]+dp[1][2]) mod MOD.
- Complexity: O(n) time, O(n) memory.
*/

inline void solve() {
	int n, m; cin >> n >> m;

	V<vi> adj(n+1);
	for (int i = 0; i < n-1; i++) {
		int u, v; cin >> u >> v;
		adj[u].pb(v);
		adj[v].pb(u);
	}

	V<AR<ll,3>> dp(n+1);
	for (int i = 1; i <= n; i++) dp[i] = AR<ll,3>{1,1,1};

	for (int i = 0; i < m; i++) {
		int b, c; cin >> b >> c; --c;
		dp[b] = AR<ll,3>{0,0,0};
		dp[b][c] = 1;
	}

	vi par(n+1, 0), order;
	order.reserve(n);

	// iterative DFS to avoid recursion depth issues
	stack<int> st;
	st.push(1);
	par[1] = -1;
	while (!st.empty()) {
		int v = st.top(); st.pop();
		order.pb(v);
		for (int u : adj[v]) if (u != par[v]) {
			par[u] = v;
			st.push(u);
		}
	}

	// postorder DP (reverse of DFS order works because parent is visited before child)
	for (int i = n-1; i >= 0; i--) {
		int v = order[i];
		for (int u : adj[v]) if (par[u] == v) {
			ll s01 = (dp[u][0] + dp[u][1]) % MOD;
			ll s02 = (dp[u][0] + dp[u][2]) % MOD;
			ll s12 = (dp[u][1] + dp[u][2]) % MOD;

			dp[v][0] = dp[v][0] * s12 % MOD;
			dp[v][1] = dp[v][1] * s02 % MOD;
			dp[v][2] = dp[v][2] * s01 % MOD;
		}
	}

	ll ans = (dp[1][0] + dp[1][1] + dp[1][2]) % MOD;
	cout << ans << "\n";
}

int main() {
	setIO("barnpainting");
	solve(); // single case
}
