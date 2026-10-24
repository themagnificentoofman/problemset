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

// ---------- helpers ----------
static inline ll mulmod(ll a, ll b, ll MOD) {
	return (ll)((i128)a * (i128)b % MOD);
}
static inline ll addmod(ll a, ll b, ll MOD) {
	a += b; if (a >= MOD) a -= MOD; return a;
}
static inline ll submod(ll a, ll b, ll MOD) {
	a -= b; if (a < 0) a += MOD; return a;
}
static inline ll powmod(ll a, ll e, ll MOD) {
	ll r = 1LL; a %= MOD;
	while (e > 0) {
		if (e & 1LL) r = mulmod(r, a, MOD);
		a = mulmod(a, a, MOD);
		e >>= 1LL;
	}
	return r;
}

// sieve primes up to n
static vi primes_upto(int n) {
	vi primes;
	V<char> comp(n+1, 0);
	for (int i = 2; i <= n; ++i) {
		if (!comp[i]) {
			primes.pb(i);
			if (1LL * i * i <= n) for (ll j = 1LL * i * i; j <= n; j += i) comp[(int)j] = 1;
		}
	}
	return primes;
}

// v_p(n!) via Legendre
static inline ll vp_fact(ll n, int p) {
	ll s = 0;
	while (n) { n /= p; s += n; }
	return s;
}

/*
 (1) Problem recap in simple terms.
    We need the product (over all permutations A of [1..N]) of the number of steps until
    the permutation returns to identity, modulo prime M. The steps needed for A is the
    lcm of its cycle lengths. We want ∏_A lcm(cycles(A)) mod M.

 (2) Things to notice and step-by-step derivation.
    • Write the answer as ∏_p p^{E_p} (mod M). For a fixed prime p, v_p(lcm) ≥ k iff A has
      some cycle length divisible by z = p^k. So
         E_p = Σ_{k≥1} with(z),  where with(z) = #permutations whose cycle length is divisible by z.
    • Editorial identity for “no multiple of z” gives
         without(z) = n! / ( z^d · d! ) · ∏_{t=1}^d (z t − 1),  where d = ⌊n/z⌋,
      hence with(z) = n! − without(z)  (all exponents computed modulo T = M − 1).
    • The divisibility pain point is z^d and d!, but note:
         n! / d! = ∏_{i=d+1}^n i.
      Also, v_p(∏_{i=d+1}^n i) ≥ k·d and each (z t − 1) is coprime to p, so we can
      cancel all p^kd **inside the tail product** (no modular inverses needed).

 (3) Solution outline.
    Step A: Read N, M; let T = M − 1. Precompute n! mod T.
    Step B: Sieve primes p ≤ N.
    Step C (divide-and-conquer init per prime):
        Build a static product structure (iterative segment tree) over the array
        g_p(i) = i with all p-factors removed. This supports range product
        ∏_{i=L}^R g_p(i) modulo T in O(log N).
    Step D: For each prime power z = p^k ≤ N:
        d = ⌊N/z⌋.
        // tail with p-factors stripped, then multiply back p^{v_p(tail) − k d}
        S_no_p = prod_{i=d+1..N} g_p(i) using the segment tree.
        e_tail = (v_p(N!) − v_p(d!)) − k·d ≥ 0.
        R = S_no_p · p^{e_tail} (mod T).
        P = ∏_{t=1}^d (z t − 1) (mod T).
        a(N,z) = R · P (mod T); with(z) = n! − a(N,z) (mod T).
        Answer = Answer · p^{with(z)} (mod M).
    Complexity: building all per-prime structures costs O(N log N) overall;
                the sweep over prime powers costs O(N log log N) extra, dominated by build.
*/

// per-prime iterative segment tree for products of g_p(i) = i with all p-factors removed
struct SegMul {
	int SZ; ll MOD; vl t; // tree size = 2*SZ
	void init(int n, int p, ll MOD_in) {
		MOD = MOD_in;
		SZ = 1; while (SZ < n) SZ <<= 1;
		t.assign(2*SZ, 1LL);
		// leaves: 1..n
		for (int i = 1; i <= n; ++i) {
			ll x = i;
			while (x % p == 0) x /= p; // remove all p-factors
			t[SZ + i - 1] = x % MOD;
		}
		// pad others with 1
		for (int i = n+1; i <= SZ; ++i) t[SZ + i - 1] = 1LL;
		// build
		for (int i = SZ - 1; i >= 1; --i) t[i] = mulmod(t[i<<1], t[i<<1|1], MOD);
	}
	// product on [l..r], 1-based inclusive; if l>r return 1
	ll query(int l, int r) const {
		if (l > r) return 1LL;
		ll L = 1LL, R = 1LL;
		int i = l - 1 + SZ, j = r - 1 + SZ;
		while (i <= j) {
			if (i & 1) L = mulmod(L, t[i++], MOD);
			if (!(j & 1)) R = mulmod(t[j--], R, MOD);
			i >>= 1; j >>= 1;
		}
		return mulmod(L, R, MOD);
	}
};

inline void solve() {
	int N; ll MOD;
	cin >> N >> MOD;
	const ll T = MOD - 1; // exponent modulus

	// n! mod T
	ll factN = 1LL;
	for (int i = 1; i <= N; ++i) factN = mulmod(factN, i, T);

	// primes up to N
	vi ps = primes_upto(N);

	// build per-prime product structures: g_p(i) = i with p-factors removed
	// and reuse them for all powers of that prime.
	V<SegMul> segs; segs.rsz(sz(ps));

	ll ans = 1LL;

	for (int idx = 0; idx < sz(ps); ++idx) {
		int p = ps[idx];
		segs[idx].init(N, p, T);

		// iterate prime powers z = p^k
		ll z = p;
		int k = 1;
		while (z <= N) {
			int d = N / (int)z; // floor(N/z)

			// product over (d+1..N) with p-factors removed
			ll S_no_p = segs[idx].query(d + 1, N);

			// exponent of p in tail (d+1..N) minus k*d (from z^d) -- never negative
			ll e_tail = (vp_fact(N, p) - vp_fact(d, p)) - 1LL * k * d;

			// R = (∏_{i=d+1}^N i) / p^{k d}  ==  (∏ g_p(i)) * p^{e_tail}
			ll R = mulmod(S_no_p, powmod(p % T, e_tail, T), T);

			// P = ∏_{t=1}^d (z*t - 1) mod T
			ll P = 1LL;
			for (int t = 1; t <= d; ++t) {
				ll term = (ll)((i128)z * t - 1) % T;
				if (term < 0) term += T;
				P = mulmod(P, term, T);
			}

			// a(N,z) and with(z)
			ll aNz = mulmod(R, P, T);
			ll with_z = submod(factN, aNz, T);

			// contribute p^{with_z} to the final result modulo M
			ans = mulmod(ans, powmod(p % MOD, with_z, MOD), MOD);

			// next power
			if (z > (ll)N / p) break; // avoid overflow and unnecessary loop
			z *= (ll)p;
			++k;
		}
	}

	cout << ans % MOD << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
