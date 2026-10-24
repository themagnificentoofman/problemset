// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t;
// sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default).
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
template<class T, size_t SZ> using AR = array<T,SZ>;
// c-style array but better, basically (can use .at()).
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); }
// ind of 1st elem >= b.
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); }
// ind of 1st elem > b.

template<class T> using pq = priority_queue<T>; // max-heap priority queue (largest on top)
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;
// min-heap priority queue (smallest on top).

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
const ll BIG = (ll)4e18; // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)2e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     Consider all permutations a of 1..N with exactly K inversions. Build the Cartesian/BST
     defined by taking the minimum value as the root recursively, and let d_i(a) be the depth of
     position i. Output sum_a d_i(a) mod M for every i.

 (2) Things to notice and step-by-step derivation:
     - The number of permutations by inversion count is the Mahonian polynomial
       P(x)=prod_{i=1..N}(1+x+...+x^{i-1}).
     - Multiplying/dividing by (1+x+...+x^{b-1}) can be done in O(deg) using prefix sums.
     - For a fixed distance dif, contributions to depths split into two symmetric cases, which
       can be expressed using coefficients of P after removing/adding one factor.

 (3) Solution outline:
     - Build P(x) up to degree K with repeated fast "multiply by geometric" operations.
     - For each dif=1..N-1, temporarily divide P by (1+x+...+x^{dif}) to query needed coeffs,
       then restore it.
     - Add those coeffs to the answer array at positions a and a+dif as in the derived formula.
     - Time: O(N*K), Memory: O(K)
 */

struct Mint {
	static int MOD;
	int val;

	Mint(ll v = 0) {
		if (MOD == 0) MOD = 1;
		v %= MOD;
		if (v < 0) v += MOD;
		val = (int)v;
	}

	Mint& operator+=(const Mint& o) {
		val += o.val;
		if (val >= MOD) val -= MOD;
		return *this;
	}
	Mint& operator-=(const Mint& o) {
		val -= o.val;
		if (val < 0) val += MOD;
		return *this;
	}
};
int Mint::MOD = 1;

using vm = V<Mint>;

inline void mulGeom(vm& a, int b) {
	// a *= (1 + x + ... + x^{b-1})
	a.rsz(sz(a) + b - 1);
	for (int i = sz(a) - b - 1; i >= 0; i--) a[i + b] -= a[i];
	for (int i = 1; i < sz(a); i++) a[i] += a[i - 1];
}

inline void divGeom(vm& a, int b) {
	// a /= (1 + x + ... + x^{b-1})
	for (int i = sz(a) - 1; i >= 1; i--) a[i] -= a[i - 1];
	for (int i = 0; i + b < sz(a); i++) a[i + b] += a[i];
	a.rsz(sz(a) - b + 1);
}

inline Mint getCoeff(const vm& a, int i) {
	if (i < 0 || i >= sz(a)) return 0;
	return a[i];
}

inline void solve() {
	int n, k, mod; cin >> n >> k >> mod;
	Mint::MOD = mod;

	vm poly = {1};
	for (int i = 1; i <= n; i++) mulGeom(poly, i);

	vm ans(n, getCoeff(poly, k));
	for (int dif = 1; dif < n; dif++) {
		divGeom(poly, dif + 1);
		Mint x = getCoeff(poly, k - dif);
		Mint y = getCoeff(poly, k);
		mulGeom(poly, dif + 1);

		for (int a = 0; a + dif < n; a++) {
			ans[a] += x;
			ans[a + dif] += y;
		}
	}

	for (int i = 0; i < n; i++) cout << ans[i].val << " \n"[i == n - 1];
}

int main() {
	setIO("treedepth");
	solve(); // single case
}
