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
   Bessie starts at some vertex i>1 on a rectilinear simple polygon (given clockwise).
   With lights on, she takes the shorter of clockwise/counterclockwise paths to the exit (vertex 1).
   With lights out, she walks clockwise until her sensed sequence of (angle, edge length, angle, ...)
   uniquely identifies where she is, then she takes the shorter path to the exit.
   Find the maximum extra distance (dark - light) over all start vertices.

2) Things to notice + step-by-step:
   - Distances along the perimeter can be answered with prefix sums of edge lengths.
   - At a vertex, Bessie can distinguish convex vs concave (interior angle 90 vs 270),
     and after walking an edge she knows its length exactly.
   - For each start vertex i, consider its clockwise “signature”:
       angle(i), len(i->i+1), angle(i+1), len(i+1->i+2), ...
     After t steps, Bessie has seen a prefix of length (2t+1). She can locate herself iff
     this prefix is unique among all possible start vertices (excluding the exit).

3) Solution outline:
   - Precompute edge lengths and convex/concave flags; build prefix sums for cw distances.
   - Build a signature array for each start vertex.
   - For each i=1..n-1, find the earliest t before reaching the exit where its prefix is unique;
     compute dark distance accordingly and compare to light distance.
   - Time: O(N^3) (N<=200), easily fast enough.
*/

static inline ll manDist(const pi& a, const pi& b) {
	return llabs((ll)a.fi - b.fi) + llabs((ll)a.se - b.se);
}

static inline int turnCode(const vpi& p, int i) {
	// encode convex/concave as negative numbers (so they never look like lengths)
	int n = sz(p);
	int prv = (i - 1 + n) % n, nxt = (i + 1) % n;

	ll ax = (ll)p[i].fi - p[prv].fi;
	ll ay = (ll)p[i].se - p[prv].se;
	ll bx = (ll)p[nxt].fi - p[i].fi;
	ll by = (ll)p[nxt].se - p[i].se;

	ll cross = ax * by - ay * bx;
	// points are given clockwise; convex => right turn => cross < 0
	return (cross < 0 ? -1 : -2); // -1 = 90, -2 = 270 (concave)
}

static inline ll cwDist(const vl& pref, int a, int b) {
	int n = sz(pref) - 1;
	ll per = pref[n];
	if (a <= b) return pref[b] - pref[a];
	return per - (pref[a] - pref[b]);
}

inline void solve() {
	int n;
	cin >> n;

	vpi p(n);
	for (int i = 0; i < n; i++) cin >> p[i].fi >> p[i].se;

	vl len(n);
	for (int i = 0; i < n; i++) len[i] = manDist(p[i], p[(i + 1) % n]);

	vi ang(n);
	for (int i = 0; i < n; i++) ang[i] = turnCode(p, i);

	vl pref(n + 1, 0);
	for (int i = 0; i < n; i++) pref[i + 1] = pref[i] + len[i];
	ll per = pref[n];

	auto minToExit = [&](int v) -> ll {
		ll cw = cwDist(pref, v, 0);
		return min(cw, per - cw);
	};

	// build signature for each start vertex s:
	// [ang[s], len[s], ang[s+1], len[s+1], ..., len[s+n-1], ang[s]]
	V<vi> sig(n);
	for (int s = 0; s < n; s++) {
		vi cur;
		cur.pb(ang[s]);
		for (int k = 0; k < n; k++) {
			int e = (s + k) % n;
			int v = (s + k + 1) % n;
			cur.pb((int)len[e]);
			cur.pb(ang[v]);
		}
		sig[s] = cur;
	}

	auto prefixEq = [&](int a, int b, int L) -> bool {
		for (int i = 0; i < L; i++) if (sig[a][i] != sig[b][i]) return false;
		return true;
	};

	ll ans = 0;
	for (int s = 1; s < n; s++) {
		ll light = minToExit(s);

		int stepsToExit = n - s; // moving clockwise, reach 0 after n-s edges
		int tUnique = -1;

		for (int t = 0; t < stepsToExit; t++) {
			int L = 2 * t + 1;
			bool uniq = true;
			for (int other = 1; other < n; other++) {
				if (other == s) continue;
				if (prefixEq(s, other, L)) { uniq = false; break; }
			}
			if (uniq) { tUnique = t; break; }
		}

		ll dark;
		if (tUnique == -1) {
			dark = cwDist(pref, s, 0);
		} else {
			int v = (s + tUnique) % n;
			dark = cwDist(pref, s, v) + minToExit(v);
		}

		ans = max(ans, dark - light);
	}

	cout << ans << "\n";
}

int main() {
	setIO("lightsout");
	solve(); // single case
}
