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
     Each cow is a K-bit mask. For every cow, output a value based on how similar it is to all other
     cows, where similarity depends on the size of the bitwise intersection.

 (2) Things to notice and step-by-step derivation:
     - Many terms look like sums over all masks x of cnt[x] * f(x & m) for each query mask m.
     - This is an "AND-style" convolution that can be computed for all m at once with zeta/mobius
       transforms on subsets/supersets.
     - After we can compute sum_x cnt[x] * f(x & m), we can reuse it with different weightings of
       cnt[x] (e.g., multiplied by popcount(x)) to form the final answer per cow.

 (3) Solution outline:
     - Count frequencies cnt[mask].
     - Build f[mask] = 1 / popcount(mask) (and f[0] = 0).
     - Compute unionSum(cnt, f) and unionSum(cnt*popcount, f) using zeta/mobius transforms.
     - For each cow mask m, combine the precomputed values into the requested result.
     - Time complexity: O(K * 2^K + N). Memory complexity: O(2^K).
 */

struct mi {
	int v;
	mi(ll _v = 0) {
		_v %= MOD;
		if (_v < 0) _v += MOD;
		v = (int)_v;
	}
	explicit operator int() const { return v; }
};

static inline mi& operator+=(mi& a, mi b) {
	a.v += b.v;
	if (a.v >= MOD) a.v -= MOD;
	return a;
}
static inline mi& operator-=(mi& a, mi b) {
	a.v -= b.v;
	if (a.v < 0) a.v += MOD;
	return a;
}
static inline mi operator+(mi a, mi b) { return a += b; }
static inline mi operator-(mi a, mi b) { return a -= b; }
static inline mi operator*(mi a, mi b) { return mi((ll)a.v * b.v); }
static inline mi& operator*=(mi& a, mi b) { return a = a * b; }

using vm = V<mi>;

static vm supersetSum(vm a, int k) {
	int n = sz(a);
	for (int b = 0; b < k; b++) {
		for (int m = n - 1; m >= 0; m--) {
			if (!((m >> b) & 1)) a[m] += a[m ^ (1 << b)];
		}
	}
	return a;
}

static vm inverseSupersetSum(vm a, int k) {
	for (int b = k - 1; b >= 0; b--) {
		for (int m = 0; m < sz(a); m++) {
			if (!((m >> b) & 1)) a[m] -= a[m ^ (1 << b)];
		}
	}
	return a;
}

static vm subsetSum(vm a, int k) {
	for (int b = 0; b < k; b++) {
		for (int m = 0; m < sz(a); m++) {
			if ((m >> b) & 1) a[m] += a[m ^ (1 << b)];
		}
	}
	return a;
}

static vm unionSum(vm cnt, vm f, int k) {
	// for each mask m: sum_x cnt[x] * f(x & m)
	cnt = subsetSum(cnt, k);
	vm g = inverseSupersetSum(f, k);
	for (int m = 0; m < sz(cnt); m++) cnt[m] *= g[m];
	cnt = supersetSum(cnt, k);
	return cnt;
}

static vm sumSimilarities(const vi& cows, int k) {
	int n = sz(cows);
	int s = 1 << k;

	vm cnt(s);
	for (int x : cows) cnt[x] += mi(1);

	vm invs(k + 1);
	if (k >= 1) invs[1] = mi(1);
	for (int i = 2; i <= k; i++) {
		invs[i] = mi(MOD - (ll)(MOD / i) * invs[MOD % i].v % MOD);
	}

	vm f(s);
	for (int m = 1; m < s; m++) f[m] = invs[__builtin_popcount((unsigned)m)];

	vm oneSum = unionSum(cnt, f, k);

	vm cntBySize(s);
	for (int m = 0; m < s; m++) cntBySize[m] = cnt[m] * mi(__builtin_popcount((unsigned)m));
	vm sizeSum = unionSum(cntBySize, f, k);

	vm res;
	res.reserve(n);
	for (int x : cows) {
		int pc = __builtin_popcount((unsigned)x);
		mi ans = oneSum[x] * mi(pc) + sizeSum[x] - mi(n);
		res.pb(ans);
	}
	return res;
}

inline void solve() {
	int n, k;
	cin >> n >> k;

	vi cows(n);
	for (int i = 0; i < n; i++) cin >> cows[i];

	vm ans = sumSimilarities(cows, k);
	for (mi x : ans) cout << x.v << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
