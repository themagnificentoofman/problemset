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
   We have a huge binary array made by XOR-ing (toggling) m ranges. Then we answer q queries.
   Each query looks at a segment [l, r] and an integer k, and returns a value based on how many
   1s/0s are in that segment and a binary-hash of a chosen suffix.

2) Things to notice and step-by-step derivation:
   - After all toggles, the array is piecewise-constant over compressed coordinates.
   - We can build:
       (a) prefix count of 1s by real length
       (b) prefix “binary hash” where appending len equal bits is done with powers of 2
   - For each query, we use:
       - get_ones(l,r)
       - range_hash(l,r)
       - a binary search for the largest lo such that suffix [lo,r] has enough zeros.

3) Solution outline:
   - Coordinate compress all range endpoints (and query math uses prefix functions that can handle any x).
   - Build parity bits per compressed interval, then prefix ones + prefix hash.
   - Answer each query with O(log range) due to a small binary search + O(log MOD) powmods.
   - Time: O((m log m) + R log MOD + q log(range) log MOD), where R = #compressed coords.
*/

static inline int addmod(int a, int b) {
	a += b;
	if (a >= MOD) a -= MOD;
	return a;
}
static inline int submod(int a, int b) {
	a -= b;
	if (a < 0) a += MOD;
	return a;
}
static inline int mulmod(ll a, ll b) {
	return int((a * b) % MOD);
}
static inline int modpow(int a, ll e) {
	ll r = 1, x = a;
	while (e > 0) {
		if (e & 1) r = (r * x) % MOD;
		x = (x * x) % MOD;
		e >>= 1;
	}
	return int(r);
}

inline void solve() {
	int n, m, q;
	cin >> n >> m >> q;

	// collect all coordinates we may need to split on
	vi vals;
	vals.pb(0);
	vals.pb(INT32_MAX);

	vpi upds(m);
	for (auto &p : upds) {
		cin >> p.fi >> p.se;
		p.se++; // make ranges half-open [l, r)
		vals.pb(p.fi);
		vals.pb(p.se);
	}

	sor(vals);
	vals.erase(unique(all(vals)), end(vals));

	auto id = [&](int x) -> int {
		return lwb<int>(vals, x);
	};

	int R = sz(vals);
	vi diff(R, 0);

	for (auto &p : upds) {
		diff[id(p.fi)]++;
		diff[id(p.se)]--;
	}

	// bit[i] = parity on interval [vals[i], vals[i+1]) for i in [0..R-2]
	vi bit(R, 0);
	vl pref1(R, 0);     // pref1[i] = #ones on [vals[0], vals[i]) by real length
	vi prefH(R, 0);     // prefH[i] = hash of bits on [vals[0], vals[i]) by real length

	for (int i = 0; i + 1 < R; i++) {
		if (i) diff[i] += diff[i - 1];
		bit[i] = diff[i] & 1;

		ll len = (ll)vals[i + 1] - vals[i];
		pref1[i + 1] = pref1[i] + (bit[i] ? len : 0);

		int pw2 = modpow(2, len);
		// append len bits of value bit[i] to the binary hash
		// old * 2^len + (bit ? (2^len - 1) : 0)
		int add = bit[i] ? submod(pw2, 1) : 0;
		prefH[i + 1] = addmod(mulmod(prefH[i], pw2), add);
	}

	auto pref_ones = [&](int x) -> ll {
		// #ones on [0, x] inclusive, with x possibly inside an interval
		if (x < 0) return 0;
		int pos = upb<int>(vals, x); // first coord > x
		if (pos <= 0) return 0;
		int i = pos - 1; // interval index
		ll inside = (ll)(x - vals[i] + 1);
		return pref1[i] + (bit[i] ? inside : 0);
	};

	auto get_ones = [&](int l, int r) -> ll {
		if (l > r) return 0;
		return pref_ones(r) - pref_ones(l - 1);
	};

	auto pref_hash = [&](int x) -> int {
		// hash of bits on [0, x] inclusive
		if (x < 0) return 0;
		int pos = upb<int>(vals, x);
		if (pos <= 0) return 0;
		int i = pos - 1;

		ll inside = (ll)(x - vals[i] + 1);
		int pw2 = modpow(2, inside);
		int add = bit[i] ? submod(pw2, 1) : 0;
		return addmod(mulmod(prefH[i], pw2), add);
	};

	auto range_hash = [&](int l, int r) -> int {
		// hash of bits in [l, r] inclusive
		if (l > r) return 0;
		int A = pref_hash(r);
		int B = pref_hash(l - 1);
		int shift = modpow(2, (ll)r - l + 1);
		return submod(A, mulmod(shift, B));
	};

	for (int qq = 0; qq < q; qq++) {
		int l, r, k;
		cin >> l >> r >> k;

		ll ones = get_ones(l, r);
		if (ones >= k) {
			cout << submod(modpow(2, k), 1) << "\n";
			continue;
		}

		// need this many zeros in a suffix
		ll need0 = (ll)k - ones;

		// find the largest lo such that suffix [lo, r] has >= need0 zeros
		int lo = l, hi = r;
		while (lo < hi) {
			int mid = lo + (hi - lo + 1) / 2;
			ll len = (ll)r - mid + 1;
			ll z = len - get_ones(mid, r);
			if (z >= need0) lo = mid;
			else hi = mid - 1;
		}

		int sufH = range_hash(lo, r);
		ll suf1 = get_ones(lo, r);

		// keep the same formula as the original code
		ll exp1 = k;
		ll exp2 = (ll)k - (ones - suf1);
		ll res = (ll)sufH + modpow(2, exp1) - modpow(2, exp2);
		res %= MOD;
		if (res < 0) res += MOD;

		cout << (int)res << "\n";
	}
}

int main() {
	setIO();
	solve(); // single case
}
