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
     Bessie punches a tile x; every other tile i gains |i-x| power (tile x gains 0). Each tile i needs
     at least p[i] total power. Find the minimum number of punches.

 (2) Things to notice and step-by-step derivation:
     - In an optimal plan, all punches are at 0 and N-1, except possibly one punch in the middle.
     - With A punches at 0 and B punches at N-1, tile i gets A*i + B*(N-1-i).
     - Fix A. The minimum B that works is max over i<N-1 of (p[i] - A*i)/(N-1-i). Then K(A)=A+B.
     - That max is the upper envelope of lines in A, so K(A) is convex; its minimum is at a hull
       breakpoint or the boundary A >= p[N-1]/(N-1).
     - The best real-valued K gives an integer answer of either ceil(K) or ceil(K)+1, so we compute
       ceil(K) and verify with the official integer feasibility test.

 (3) Solution outline:
     - Build the convex hull for g(A) = max_{i=0..N-2} (p[i] - A*i)/(N-1-i) in O(N).
     - Let A_min = p[N-1]/(N-1). Evaluate K(A)=A+g(A) at A_min and all hull breakpoints A>=A_min to
       get the minimum real K*.
     - Let k = ceil(K*), then adjust using the monotone feasibility check for k-1, k, k+1.
     - Time complexity: O(N) per test case; memory complexity: O(N).
 */

struct Frac {
	i128 num = 0, den = 1; // den > 0, or den == 0 meaning -infinity
};

static inline Frac makeFrac(i128 num, i128 den) {
	if (den == 0) return {num, 0};
	if (den < 0) num = -num, den = -den;
	if (num == 0) return {0, 1};
	i128 a = num < 0 ? -num : num;
	i128 b = den;
	while (b) {
		i128 t = a % b;
		a = b;
		b = t;
	}
	num /= a;
	den /= a;
	return {num, den};
}

static inline bool leqF(const Frac& a, const Frac& b) {
	if (a.den == 0) return true; // -inf <= anything
	if (b.den == 0) return false;
	return a.num * b.den <= b.num * a.den;
}
static inline bool ltF(const Frac& a, const Frac& b) {
	if (a.den == 0) return b.den != 0;
	if (b.den == 0) return false;
	return a.num * b.den < b.num * a.den;
}

static inline Frac minF(const Frac& a, const Frac& b) {
	return ltF(a, b) ? a : b;
}

static inline ll ceilPos(const Frac& x) {
	// assumes x >= 0
	if (x.den == 0) return BIG;
	if (x.num <= 0) return 0;
	i128 v = (x.num + x.den - 1) / x.den;
	if (v > (i128)BIG) return BIG;
	return (ll)v;
}

static inline i128 floorDiv(i128 a, i128 b) {
	assert(b != 0);
	if (b < 0) a = -a, b = -b;
	if (a >= 0) return a / b;
	return -(((-a) + b - 1) / b);
}

static inline i128 ceilDiv(i128 a, i128 b) {
	return -floorDiv(-a, b);
}

static inline bool feasible(ll k, const vl& p) {
	int n = sz(p);

	if (k == 0) {
		for (int i = 0; i < n; i++) if (p[i] != 0) return false;
		return true;
	}

	if (n & 1) {
		int mid = n / 2;
		i128 have = (i128)k * (i128)mid;
		if (have < (i128)p[mid]) return false;
	}

	i128 lbA = 0;
	i128 ubA = (i128)k;
	for (int i = 0; i < n; i++) {
		ll den = 2LL * i + 1 - n;
		if (den == 0) continue;
		i128 num = (i128)p[i] - (i128)k * (i128)(n - 1 - i);
		if (den > 0) {
			lbA = max(lbA, ceilDiv(num, (i128)den));
		} else {
			ubA = min(ubA, floorDiv(num, (i128)den));
		}
	}

	lbA = max<i128>(lbA, 0);
	ubA = min<i128>(ubA, (i128)k);
	if (lbA <= ubA) return true;
	if (lbA > ubA + 1) return false;

	// special case: no integer A works with A+B=k, try one middle punch (so A+B=k-1).
	i128 A = ubA;
	i128 B = (i128)(k - 1) - A;
	if (B < 0) return false;

	vi st;
	st.reserve(n);
	ll ubPos = -1;
	for (int i = 0; i < n; i++) {
		if (i > ubPos) st.pb(i);

		i128 base = A * (i128)i + B * (i128)(n - 1 - i);
		i128 need = (i128)p[i] - base;
		if (need <= 0) continue;

		ll d = (ll)need; // need <= 1e18, fits
		i128 loPos = (i128)i - (i128)d;
		i128 hiPos = (i128)i + (i128)d;

		ll lowCut = (loPos < -1 ? -1 : (loPos > (i128)n ? n : (ll)loPos));
		ll highCut = (hiPos < -1 ? -1 : (hiPos > (i128)n ? n : (ll)hiPos));

		ubPos = max(ubPos, highCut - 1);
		while (sz(st) && st.bk > lowCut) st.ppb();
	}
	return sz(st) > 0;
}

struct Line {
	ll i, d; // g(A) = (p - i*A)/d, where d = N-1-i > 0
	ll p;
	Frac xl; // first A where this line becomes maximal
};

static inline Frac isect(const Line& a, const Line& b) {
	// (a.p - a.i*x)/a.d == (b.p - b.i*x)/b.d
	// x = (a.p*b.d - b.p*a.d) / (a.i*b.d - b.i*a.d)
	i128 num = (i128)a.p * (i128)b.d - (i128)b.p * (i128)a.d;
	i128 den = (i128)a.i * (i128)b.d - (i128)b.i * (i128)a.d;
	return makeFrac(num, den);
}

static inline Frac evalK(const Frac& A, const Line& L) {
	// K(A) = A + (L.p - L.i*A)/L.d
	i128 numA = A.num, denA = A.den;
	i128 num = numA * ((i128)L.d - (i128)L.i) + (i128)L.p * denA;
	i128 den = denA * (i128)L.d;
	return makeFrac(num, den);
}

static inline int queryHull(const V<Line>& hull, const Frac& A) {
	int lo = 0, hi = sz(hull) - 1;
	while (lo < hi) {
		int mid = (lo + hi + 1) / 2;
		if (leqF(hull[mid].xl, A)) lo = mid;
		else hi = mid - 1;
	}
	return lo;
}

static inline ll candidateK(const vl& p) {
	int n = sz(p);
	ll mxp = 0;
	for (int i = 0; i < n; i++) mxp = max(mxp, p[i]);
	if (mxp == 0) return 0;

	V<Line> hull;
	hull.reserve(n);

	for (int ii = n - 2; ii >= 0; ii--) {
		if (p[ii] == 0 && ii > 0) continue;
		Line ln{(ll)ii, (ll)(n - 1 - ii), p[ii], {-1, 0}};
		if (!sz(hull)) {
			ln.xl = {-1, 0}; // -inf
			hull.pb(ln);
			continue;
		}
		Frac x = isect(hull.bk, ln);
		while (sz(hull) && leqF(x, hull.bk.xl)) {
			hull.ppb();
			if (!sz(hull)) break;
			x = isect(hull.bk, ln);
		}
		ln.xl = sz(hull) ? x : Frac{-1, 0};
		hull.pb(ln);
	}

	Frac aMin = makeFrac((i128)p[n - 1], (i128)(n - 1));
	int idx = queryHull(hull, aMin);
	Frac best = evalK(aMin, hull[idx]);

	for (int j = idx + 1; j < sz(hull); j++) {
		// hull[j].xl is a breakpoint, so checking K there is enough for the convex minimum.
		best = minF(best, evalK(hull[j].xl, hull[j]));
	}

	return ceilPos(best);
}

inline void solve() {
	int n;
	cin >> n;
	vl p(n);
	for (auto& x : p) cin >> x;

	ll k = candidateK(p);
	while (k > 0 && feasible(k - 1, p)) k--;
	while (!feasible(k, p)) k++;
	cout << k << "\n";
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while (t--) solve();
}
