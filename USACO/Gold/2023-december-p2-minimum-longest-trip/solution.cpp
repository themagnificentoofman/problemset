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
     For every node, we want the maximum possible number of edges on a path starting there.
     Among those longest paths, we pick the one whose edge-length sequence is lexicographically
     smallest, and output the path length (edges) and the sum of edge lengths.

 (2) Things to notice and step-by-step derivation:
     - We only care about comparing paths by: longer edge-count first, then lexicographic edge
       lengths (first edge, then the rest).
     - If a node chooses an edge u->v, then its sequence is [w] + (sequence from v). So we can
       compare two choices by (childLen, w, childRank, childSum).
     - If we process nodes from sinks upward (removing out-edges), then when we finalize a
       level, all chosen children are already finalized, so childRank encodes the suffix order.

 (3) Solution outline:
     - Reverse edges so each processed node can update all predecessors.
     - Start with sinks (out-degree 0). They have len=0, sum=0, and a trivial rank.
     - Process in rounds: for each processed node u, push a candidate into each predecessor v.
       When v's out-degree drops to 0, it joins the next round and has len = len[u] + 1.
     - For nodes in the new round, finalize sum from their best candidate, then rank them by
       (firstEdgeLength, childRank) so parents can compare suffixes next round.
     - Time: O((n + m) log n); Memory: O(n + m).
 */

inline void solve() {
	int n, m; cin >> n >> m;

	V<vpi> rev(n + 1);
	vi outDeg(n + 1, 0);

	for (int i = 0; i < m; i++) {
		int u, v, w; cin >> u >> v >> w;
		rev[v].pb({u, w});
		outDeg[u]++;
	}

	vi len(n + 1, 0), rankId(n + 1, 0);
	vl sum(n + 1, 0);

	V<AR<ll, 4>> bestCand(n + 1);
	vb hasBest(n + 1, false);

	auto upd = [&](int u, int v, int w) {
		// candidate for u choosing edge u->v of length w
		AR<ll, 4> cur = {-(ll)len[v], (ll)w, (ll)rankId[v], sum[v] + (ll)w};
		if (!hasBest[u] || cur < bestCand[u]) {
			bestCand[u] = cur;
			hasBest[u] = true;
		}
	};

	vi nodes;
	for (int i = 1; i <= n; i++) if (!outDeg[i]) nodes.pb(i);

	while (sz(nodes)) {
		vi nodes2;

		for (int v : nodes) {
			for (auto [u, w] : rev[v]) {
				upd(u, v, w);
				if (--outDeg[u] == 0) {
					len[u] = len[v] + 1;
					nodes2.pb(u);
				}
			}
		}

		nodes.swap(nodes2);
		if (!sz(nodes)) break;

		V<AR<ll, 3>> ord;
		ord.reserve(sz(nodes));

		for (int u : nodes) {
			sum[u] = bestCand[u][3];
			ord.pb({bestCand[u][1], bestCand[u][2], (ll)u});
		}

		sor(ord);
		for (int i = 0; i < sz(ord); i++) {
			int u = (int)ord[i][2];
			rankId[u] = i;
		}
	}

	for (int i = 1; i <= n; i++) {
		cout << len[i] << ' ' << sum[i] << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
