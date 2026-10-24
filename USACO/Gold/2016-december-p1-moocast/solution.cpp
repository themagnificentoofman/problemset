// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t;
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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   We have N cows at (x,y). If cow A can transmit to cow B with power = squared distance,
   we want the minimum power so that (directly/indirectly) every cow can reach every other cow.

2) Things to notice + step-by-step:
   - If we build a spanning tree of the cows, and set power to the largest edge in that tree,
     then the whole tree becomes connected under that power.
   - To minimize the needed power, we want a Minimum Spanning Tree (MST).
   - The answer is the maximum edge weight used in the MST.

3) Solution outline:
   - Run Prim’s algorithm in O(N^2) (no need for a heap since N is small enough).
   - Track the largest edge chosen while growing the MST.
   - Output that largest edge. Time: O(N^2).
*/

static inline int sqDist(const pi& a, const pi& b) {
	int dx = a.fi - b.fi;
	int dy = a.se - b.se;
	return dx * dx + dy * dy;
}

inline void solve() {
	int n;
	cin >> n;

	vpi cows(n);
	for (auto &p : cows) cin >> p.fi >> p.se;

	const int INF = (int)1e9;
	vi best(n, INF); // best edge to connect this node to current tree
	vb inTree(n, false);

	best[0] = 0;
	int ans = 0;

	for (int it = 0; it < n; it++) {
		int v = -1;
		for (int i = 0; i < n; i++) {
			if (!inTree[i] && (v == -1 || best[i] < best[v])) v = i;
		}

		inTree[v] = true;
		ans = max(ans, best[v]);

		for (int u = 0; u < n; u++) {
			if (inTree[u]) continue;
			best[u] = min(best[u], sqDist(cows[v], cows[u]));
		}
	}

	cout << ans << "\n";
}

int main() {
	setIO("moocast");
	solve(); // single case
}
