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
     We have K tree-components. Add K roads of length X to form one big loop that visits each
     component exactly once, and we must travel at least one original road inside each component.
     Sum the total lengths of all loops whose total length is at least Y (mod 1e9+7).

 (2) Things to notice and step-by-step derivation:
     - Inside one component, the loop enters at one node and leaves at another, so we only need
       all unordered-pair distances in that tree (each gives a possible internal segment length).
     - Total loop length = K*X + (sum of chosen internal distances, one per component).
     - We can bucket all lengths >= Y together (once we reach Y, future additions stay "interesting").
     - DP over components: for each length bucket t, store (sumLengths, countWays) modulo MOD.
       Combining two groups uses: newCount = Acount*Bcount, newSum = Asum*Bcount + Acount*Bsum.
     - After DP, multiply by 2^(K-1)*(K-1)! to account for ordering components in a cycle and
       choosing which endpoint is used as entry/exit in each step.

 (3) Solution outline:
     - Find connected components; for each component, run DFS from every node to list all
       unordered-pair distances, then compress into "only buckets that appear" (clamp >=Y).
     - Convolve this component's buckets into the global DP over length 0..Y, tracking (sum,count).
     - Answer is DP[Y].sum * 2^(K-1) * (K-1)! mod MOD.
     - Time: O(N^2 + N*Y^1.5). Memory: O(N + Y).
 */

inline int addMod(int a, int b) {
	a += b;
	if (a >= MOD) a -= MOD;
	return a;
}
inline int mulMod(int a, int b) {
	return int((ll)a * b % MOD);
}
inline void addPair(pi& a, const pi& b) {
	a.fi = addMod(a.fi, b.fi);
	a.se = addMod(a.se, b.se);
}
inline pi comb(const pi& a, const pi& b) {
	int sum = int(((ll)a.fi * b.se + (ll)a.se * b.fi) % MOD);
	int cnt = mulMod(a.se, b.se);
	return {sum, cnt};
}

inline void solve() {
	int n, m, xAdd, yLim;
	cin >> n >> m >> xAdd >> yLim;

	int k = n - m; // number of components in a forest
	V<vpi> adj(n + 1);
	for (int i = 0; i < m; i++) {
		int a, b, d;
		cin >> a >> b >> d;
		adj[a].pb({b, d});
		adj[b].pb({a, d});
	}

	V<char> vis(n + 1, 0);
	vi comp, distVals;

	function<void(int)> dfsComp = [&](int u) {
		if (vis[u]) return;
		vis[u] = 1;
		comp.pb(u);
		for (auto e : adj[u]) dfsComp(e.fi);
	};

	function<void(int,int,int,int)> dfsDist = [&](int u, int p, int ori, int d) {
		for (auto e : adj[u]) {
			int v = e.fi, w = e.se;
			if (v == p) continue;
			int nd = d + w;
			if (ori < v) distVals.pb(nd);
			dfsDist(v, u, ori, nd);
		}
	};

	vpi res(yLim + 1, {0, 0}), tmp(yLim + 1, {0, 0});
	int baseLen = k * xAdd;
	res[min(baseLen, yLim)] = {baseLen % MOD, 1};

	for (int st = 1; st <= n; st++) {
		if (vis[st]) continue;

		comp.clear();
		dfsComp(st);

		distVals.clear();
		for (int u : comp) dfsDist(u, -1, u, 0);

		vpi ways(yLim + 1, {0, 0});
		vi keys;
		keys.reserve(sz(distVals));
		for (int d : distVals) {
			int key = min(d, yLim);
			if (ways[key].se == 0) keys.pb(key);
			ways[key].fi = addMod(ways[key].fi, d % MOD);
			ways[key].se = addMod(ways[key].se, 1);
		}

		fill(all(tmp), pi{0, 0});
		for (int key : keys) {
			for (int t = 0; t <= yLim; t++) {
				int nt = min(t + key, yLim);
				addPair(tmp[nt], comb(res[t], ways[key]));
			}
		}
		res.swap(tmp);
	}

	int ans = res[yLim].fi;
	for (int i = 0; i < k - 1; i++) ans = mulMod(ans, 2);
	for (int i = 1; i < k; i++) ans = mulMod(ans, i);

	cout << ans << "\n";
}

int main() {
	setIO("mooriokart");
	solve(); // single case
}
