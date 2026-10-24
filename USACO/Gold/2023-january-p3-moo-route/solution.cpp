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
     Given N and counts A[i] of how many times Bessie's route crosses each boundary i+0.5,
     count (mod 1e9+7) how many routes are consistent with A and use the minimum possible
     number of direction changes.

 (2) Things to notice and step-by-step derivation:
     - Since Bessie starts and ends at 0, each boundary i+0.5 must be crossed an even number
       of times. Let B[i] = A[i]/2, so we need exactly B[i] moves R_i and B[i] moves L_i.
     - In a minimum-turn route, crossings at adjacent boundaries must “chain” together in a
       forced way, leaving only one local choice per i: which R_i are followed by R_{i+1}
       (if B[i] >= B[i+1]) or which L_{i+1} are followed by L_i (if B[i] < B[i+1]).
     - These choices are independent across i, giving a product of binomial coefficients:
         if B[i] >= B[i+1] -> C(B[i], B[i+1])
         else              -> C(B[i+1]-1, B[i]-1) (last L_{i+1} must be followed by L_i)

 (3) Solution outline:
     - Read A, build B[i] = A[i]/2 and maxB = max(B[i]).
     - Precompute fact and invFact up to maxB with one modpow for invFact[maxB].
     - Multiply the correct binomial term for each i = 0..N-2, mod MOD, and print.
     - Time: O(maxB + N + log MOD); Memory: O(maxB).
 */

inline ll modPow(ll a, ll e) {
	a %= MOD;
    if (a < 0) a += MOD;
	ll r = 1LL;
	while (e > 0) {
		if (e & 1) r = (r * a) % MOD;
		a = (a * a) % MOD;
		e >>= 1;
	}
	return r;
}

inline void initComb(int maxN, vl& fact, vl& invFact) {
	fact.rsz(maxN + 1);
	invFact.rsz(maxN + 1);

	fact[0] = 1;
	for (int i = 1; i <= maxN; i++) fact[i] = (fact[i - 1] * i) % MOD;

	invFact[maxN] = modPow(fact[maxN], MOD - 2);
	for (int i = maxN - 1; i >= 0; i--) invFact[i] = (invFact[i + 1] * (i + 1)) % MOD;
}

inline ll nCk(int n, int k, const vl& fact, const vl& invFact) {
	if (k < 0 || k > n) return 0;
	return (((fact[n] * invFact[k]) % MOD) * invFact[n - k]) % MOD;
}

inline void solve() {
	int n;
	cin >> n;

	vl b(n);
	int maxB = 0;
	for (int i = 0; i < n; i++) {
		ll x;
		cin >> x;
		b[i] = x / 2;
		maxB = max(maxB, (int)b[i]);
	}

	int combN = max(1, maxB);
	vl fact, invFact;
	initComb(combN, fact, invFact);

	ll ans = 1;
	for (int i = 0; i < n - 1; i++) {
		if (b[i] >= b[i + 1]) {
			ans = (ans * nCk((int)b[i], (int)b[i + 1], fact, invFact)) % MOD;
		} else {
			ans = (ans * nCk((int)b[i + 1] - 1, (int)b[i] - 1, fact, invFact)) % MOD;
		}
	}
	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
