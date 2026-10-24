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
     World i starts at height A[i] and falls downward at speed i. When two worlds have the same
     height, a cow can instantly jump between them. For each i, find the earliest time the cow can
     reach world to[i], or print -1 if impossible (as a reduced fraction).

 (2) Things to notice and step-by-step derivation:
     - World i's height is A[i] - i*t, so each world is a line with slope -i.
     - Two worlds align at a time t that is a fraction made from differences in A and indices.
     - The optimal first jump for a cow can be found by comparing slopes against a convex hull of
       points (A[i], -i) built in sorted A order.
     - We need two sweeps (above / below) depending on whether A[i] > A[to[i]] or A[i] < A[to[i]].

 (3) Solution outline:
     - Build points p[i] = (A[i], -(i+1)), sort by A.
     - Sweep from high A to low A, maintaining a monotone hull; answer cows with A[i] > A[to[i]] via
       binary search on the hull.
     - Sweep from low A to high A, maintaining the opposite hull; answer cows with A[i] < A[to[i]].
     - For a chosen hull point h, the time is |A[h]-A[to]| / |d[h]-d[to]| reduced by gcd.
     - Time complexity: O(n log n); Memory complexity: O(n).
 */

struct Pt {
	ll y, d; // (A[i], -(i+1))
	int ind;
	bool operator<(const Pt& o) const { return y < o.y; }
};

static inline ll gcdll(ll a, ll b) {
	a = llabs(a), b = llabs(b);
	while (a && b) {
		if (a > b) a %= b;
		else b %= a;
	}
	return a + b;
}

static inline pl diff(const Pt& a, const Pt& b) {
	return {llabs(a.y - b.y), llabs(a.d - b.d)};
}

static inline int sgn(i128 x) {
	if (x < 0) return -1;
	if (x > 0) return 1;
	return 0;
}

static inline int cmpSlope(const Pt& a, const Pt& b, const Pt& c) {
	pl l = diff(a, c);
	pl r = diff(b, c);
	i128 res = (i128)l.fi * r.se - (i128)r.fi * l.se;
	return sgn(res);
}

static inline bool keepMiddle(const Pt& a, const Pt& b, const Pt& c) {
	pl l = diff(a, b);
	pl r = diff(b, c);
	i128 res = (i128)l.fi * r.se - (i128)r.fi * l.se;
	return res < 0;
}

static inline int bestIdx(const V<Pt>& hull, const Pt& target, ll targetD, int mode) {
	// mode=0: require hull[*].d <= targetD (first sweep)
	// mode=1: require hull[*].d >= targetD (second sweep)
	if (mode == 0) {
		if (hull[0].d > targetD) return -1;
	} else {
		if (hull[0].d < targetD) return -1;
	}

	int lo = 0, hi = sz(hull) - 1;
	while (lo < hi) {
		int mid = (lo + hi) / 2;
		int l = mid, r = mid + 1;

		if (mode == 0) {
			if (hull[r].d > targetD) {
				hi = l;
				continue;
			}
		} else {
			if (hull[r].d < targetD) {
				hi = l;
				continue;
			}
		}

		int res = cmpSlope(hull[l], hull[r], target);
		if (res < 0) hi = l;
		else if (res == 0) lo = hi = l;
		else lo = r;
	}
	return lo;
}

inline void solve() {
	int n;
	cin >> n;

	vl a(n);
	vi to(n);
	for (int i = 0; i < n; i++) cin >> a[i];
	for (int i = 0; i < n; i++) {
		cin >> to[i];
		to[i]--;
	}

	V<Pt> pts(n), ptsByIdx(n);
	for (int i = 0; i < n; i++) {
		pts[i] = {a[i], -(ll)(i + 1), i};
		ptsByIdx[i] = pts[i];
	}
	sor(pts);

	vl num(n, 0), den(n, 0);

	V<Pt> hull;
	hull.reserve(n);

	// sweep 1: decreasing y, answer a[i] > a[to[i]]
	for (int i = n - 1; i >= 0; i--) {
		Pt now = pts[i];
		while (!hull.empty()) {
			if (now.d < hull.bk.d) {
				hull.ppb();
				continue;
			}
			if (sz(hull) > 1 && !keepMiddle(now, hull.bk, hull[sz(hull) - 2])) {
				hull.ppb();
				continue;
			}
			break;
		}
		hull.pb(now);

		int ind = now.ind;
		int dst = to[ind];
		if (a[ind] > a[dst]) {
			ll targetD = -(ll)(dst + 1);
			int j = bestIdx(hull, ptsByIdx[dst], targetD, 0);
			if (j == -1) {
				num[ind] = -1;
			} else {
				pl got = diff(hull[j], ptsByIdx[dst]);
				ll g = gcdll(got.fi, got.se);
				num[ind] = got.fi / g;
				den[ind] = got.se / g;
			}
		}
	}

	hull.clear();

	// sweep 2: increasing y, answer a[i] < a[to[i]]
	for (int i = 0; i < n; i++) {
		Pt now = pts[i];
		while (!hull.empty()) {
			if (now.d > hull.bk.d) {
				hull.ppb();
				continue;
			}
			if (sz(hull) > 1 && !keepMiddle(now, hull.bk, hull[sz(hull) - 2])) {
				hull.ppb();
				continue;
			}
			break;
		}
		hull.pb(now);

		int ind = now.ind;
		int dst = to[ind];
		if (a[ind] < a[dst]) {
			ll targetD = -(ll)(dst + 1);
			int j = bestIdx(hull, ptsByIdx[dst], targetD, 1);
			if (j == -1) {
				num[ind] = -1;
			} else {
				pl got = diff(hull[j], ptsByIdx[dst]);
				ll g = gcdll(got.fi, got.se);
				num[ind] = got.fi / g;
				den[ind] = got.se / g;
			}
		}
	}

	for (int i = 0; i < n; i++) {
		if (num[i] == -1) cout << -1 << '\n';
		else cout << num[i] << "/" << den[i] << '\n';
	}
}

int main() {
	setIO("falling");
	solve(); // single case
}
