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
const int MXR = 20;

/*
 (1) Problem recap in simple terms:
     We need to activate R robots on a circle of length L at given activation points. Robots drift,
     and at the final time they must be equally spaced. Find the minimum final time.

 (2) Things to notice and step-by-step derivation:
     - Work in "tau" time where real time is T = K * tau. In this view, many moves become
       translation-invariant.
     - Moving between two robot slots depends only on the slot difference k (a shift of k * L/R),
       so we can precompute a base wait xGeo[k] for each k.
     - From an activation point index idx, after making a move of type k, the next usable activation
       point is simply the first activation >= (A[idx] + shift + xGeo[k]) on the unwrapped line.
       This does not depend on the current tau, only on idx and k.

 (3) Solution outline:
     - Sort activation points A and build an unwrapped array A, A+L, A+2L.
     - Precompute for every activation idx and every k in [0..R-1]:
       - delta[idx][k] = extra tau spent (move + waiting to the next activation)
       - nxt[idx][k]   = next activation index (mod N)
     - Held-Karp subset DP over the R-1 nonzero slots:
       dpTau[mask][last], dpIdx[mask][last] = best (tau, activation idx) after filling 'mask'
       and ending at slot 'last'.
     - Answer is min over last of dpTau[all][last] * K.
     - Time: O(N log N + N*R + 2^R * R^2); Memory: O(N*R + 2^R * R).
 */

static inline ll ceilDivPos(ll x, ll y) {
	// x >= 0, y > 0
	return (x + y - 1) / y;
}

static inline ll clampAdd(ll a, ll b) {
	i128 s = (i128)a + (i128)b;
	if (s >= (i128)BIG) return BIG;
	return (ll)s;
}

static inline void printI128(i128 x) {
	if (x == 0) {
		cout << 0 << "\n";
		return;
	}
	bool neg = (x < 0);
	if (neg) x = -x;
	str s;
	while (x > 0) {
		int d = (int)(x % 10);
		s.pb(char('0' + d));
		x /= 10;
	}
	if (neg) s.pb('-');
	reverse(all(s));
	cout << s << "\n";
}

inline void solve() {
	ll l, k;
	int r, n;
	cin >> l >> r >> n >> k;

	vl a(n);
	for (int i = 0; i < n; i++) cin >> a[i];
	sor(a);

	if (r <= 1) {
		cout << 0 << "\n";
		return;
	}

	ll step = l / r;

	vl a3;
	a3.reserve(3 * n);
	for (int i = 0; i < n; i++) a3.pb(a[i]);
	for (int i = 0; i < n; i++) a3.pb(a[i] + l);
	for (int i = 0; i < n; i++) a3.pb(a[i] + 2 * l);

	AR<ll, MXR> xGeo{};
	for (int d = 0; d < r; d++) {
		ll shift = step * (ll)d; // in [0, l)
		ll best = BIG;

		if (k > 1) best = min(best, ceilDivPos(shift, k - 1));

		ll gap = (l - shift) % l; // if shift==0, gap==0
		best = min(best, ceilDivPos(gap, k + 1));

		xGeo[d] = best;
	}

	V<AR<ll, MXR>> delta(n);
	V<AR<int, MXR>> nxt(n);

	for (int d = 0; d < r; d++) {
		ll shift = step * (ll)d;
		ll xg = xGeo[d];

		int ptr = 0;
		for (int i = 0; i < n; i++) {
			ll target = a[i] + shift + xg;
			while (ptr < sz(a3) && a3[ptr] < target) ptr++;

			// By constraints, target is always within the 0..(3L) window.
			if (ptr >= sz(a3)) ptr = sz(a3) - 1;

			ll found = a3[ptr];
			delta[i][d] = xg + (found - target);
			nxt[i][d] = ptr % n;
		}
	}

	int m = r - 1;         // slots 1..r-1
	int lim = 1 << m;      // mask over these slots

	V<ll> dpTau((size_t)lim * (size_t)m, BIG);
	V<int> dpIdx((size_t)lim * (size_t)m, -1);

	auto id = [&](int mask, int last) -> size_t {
		return (size_t)mask * (size_t)m + (size_t)last;
	};

	auto relax = [&](int mask, int last, ll tau, int idx) -> void {
		size_t pos = id(mask, last);
		if (tau < dpTau[pos] || (tau == dpTau[pos] && idx < dpIdx[pos])) {
			dpTau[pos] = tau;
			dpIdx[pos] = idx;
		}
	};

	for (int last = 0; last < m; last++) {
		int slot = last + 1; // real slot index in [1..r-1]
		ll shift = step * (ll)slot;
		ll xg = xGeo[slot];

		ll target = shift + xg;
		auto it = lb(all(a3), target);
		int ptr = int(it - bg(a3));

		ll found = a3[ptr];
		ll tau = xg + (found - target);
		int idx = ptr % n;

		relax(1 << last, last, tau, idx);
	}

	for (int mask = 1; mask < lim; mask++) {
		int rem = (lim - 1) ^ mask;
		for (int last = 0; last < m; last++) {
			if (!(mask & (1 << last))) continue;

			size_t pos = id(mask, last);
			ll tau = dpTau[pos];
			int idx = dpIdx[pos];
			if (tau >= BIG || idx < 0) continue;

			for (int bits = rem; bits; bits &= (bits - 1)) {
				int nxtBit = __builtin_ctz(bits);

				int diff = nxtBit - last;
				if (diff < 0) diff += r; // shift index in [1..r-1]

				ll ntau = clampAdd(tau, delta[idx][diff]);
				int nidx = nxt[idx][diff];

				relax(mask | (1 << nxtBit), nxtBit, ntau, nidx);
			}
		}
	}

	ll bestTau = BIG;
	int full = lim - 1;
	for (int last = 0; last < m; last++) {
		bestTau = min(bestTau, dpTau[id(full, last)]);
	}

	i128 ans = (i128)bestTau * (i128)k;
	printI128(ans);
}

int main() {
	setIO();
	solve(); // single case
}
