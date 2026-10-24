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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
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
   We have N points (cows). With 1 rectangle, we cover all cows with the smallest axis-aligned rectangle.
   We may instead split cows into 2 groups using one vertical or horizontal cut, and cover each group by
   its own smallest rectangle. Find how much area we can save (original area - best two-rectangle area).

2) Things to notice + step-by-step:
   - If we cut vertically at some x between two distinct x-values, left group = cows with x <= cut,
     right group = cows with x > cut. The best rectangles for each side depend only on min/max y there.
   - After sorting by x, we can precompute prefix min/max y and suffix min/max y.
   - Then every valid split i (where x[i] != x[i+1]) gives two rectangle areas in O(1).
   - Do the same after swapping x and y to handle horizontal cuts.

3) Solution outline:
   - Function bestSaved(points): sort by x, build prefix/suffix (minY,maxY), try all valid splits,
     return (oneRectArea - bestTwoRectArea).
   - Answer = max(bestSaved(points), bestSaved(swapped points)).
   - Time: O(N log N).
*/

static inline ll bestSaved(vpi pts) {
	int n = sz(pts);
	sor(pts);

	vpi pref(n), suf(n); // {minY, maxY}

	pref[0] = {pts[0].se, pts[0].se};
	for (int i = 1; i < n; i++) {
		pref[i].fi = min(pref[i - 1].fi, pts[i].se);
		pref[i].se = max(pref[i - 1].se, pts[i].se);
	}

	suf[n - 1] = {pts[n - 1].se, pts[n - 1].se};
	for (int i = n - 2; i >= 0; i--) {
		suf[i].fi = min(suf[i + 1].fi, pts[i].se);
		suf[i].se = max(suf[i + 1].se, pts[i].se);
	}

	ll one = (ll)(pts[n - 1].fi - pts[0].fi) * (ll)(pref[n - 1].se - pref[n - 1].fi);

	ll bestTwo = BIG;
	for (int i = 0; i < n - 1; i++) {
		if (pts[i].fi == pts[i + 1].fi) continue; // no place to cut between equal x's

		ll w1 = pts[i].fi - pts[0].fi;
		ll h1 = pref[i].se - pref[i].fi;
		ll a1 = w1 * h1;

		ll w2 = pts[n - 1].fi - pts[i + 1].fi;
		ll h2 = suf[i + 1].se - suf[i + 1].fi;
		ll a2 = w2 * h2;

		bestTwo = min(bestTwo, a1 + a2);
	}

	if (bestTwo == BIG) return 0; // cannot split on this axis (all x equal)
	return max(0LL, one - bestTwo);
}

inline void solve() {
	int n;
	cin >> n;

	vpi cows(n);
	for (auto &p : cows) cin >> p.fi >> p.se;

	ll ans = bestSaved(cows);
	for (auto &p : cows) swap(p.fi, p.se);
	ans = max(ans, bestSaved(cows));

	cout << ans << "\n";
}

int main() {
	setIO("split");
	solve(); // single case
}
