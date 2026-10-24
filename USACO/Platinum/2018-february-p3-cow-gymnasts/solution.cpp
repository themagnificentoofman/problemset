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
     There are N platforms in a circle, each with a stack size between 1 and N. After all stacks
     "fall" clockwise (k-th cow moves k-1 steps), each platform must end with the same stack
     size it started with. Count how many starting configurations are possible (mod 1e9+7).

 (2) Things to notice and step-by-step derivation:
     - Let m be the minimum stack size in a magical configuration. Then every stack size is
       either m or m+1, and the whole configuration is periodic with period gcd(N, m).
     - For fixed m < N with g = gcd(N, m), the number of valid configurations is 2^g - 1.
     - Regroup the sum by g: the count of m with gcd(N, m) = g equals phi(N/g), so
       sum_{m=1..N} (2^{gcd(N,m)} - 1) becomes sum_{g|N} 2^g * phi(N/g), plus constants.

 (3) Solution outline:
     - Prime-factorize N in O(sqrt(N)).
     - Enumerate all divisors g of N via recursion over prime powers.
     - For each divisor g, compute phi(N/g) using the prime list and add 2^g * phi(N/g) mod MOD.
     - Final answer is: 2 - N - 2^N + sum_{g|N} 2^g * phi(N/g) (mod MOD).
     - Time: O(sqrt(N) + d(N)*k*log N), Memory: O(d(N) + k).
 */

static ll modPow(ll a, ll e) {
	a %= MOD;
	ll r = 1;
	for (; e > 0; e >>= 1) {
		if (e & 1) r = (ll)((i128)r * a % MOD);
		a = (ll)((i128)a * a % MOD);
	}
	return r;
}

static void genDivs(int i, ll cur, const vl& pr, const vi& exp, vl& divs) {
	if (i == sz(pr)) {
		divs.pb(cur);
		return;
	}
	ll x = 1;
	for (int k = 0; k <= exp[i]; k++) {
		genDivs(i + 1, cur * x, pr, exp, divs);
		x *= pr[i];
	}
}

static ll phiUsingPrimes(ll x, const vl& pr) {
	ll res = x;
	for (ll p : pr) {
		if (p * p > x) break;
		if (x % p == 0) {
			res = res / p * (p - 1);
			while (x % p == 0) x /= p;
		}
	}
	if (x > 1) res = res / x * (x - 1);
	return res;
}

inline void solve() {
	ll n;
	cin >> n;

	vl pr;
	vi exp;

	// factor n into primes (pr) and exponents (exp)
	ll tmp = n;
	for (ll x = 2; x * x <= tmp; x++) {
		if (tmp % x == 0) {
			int cnt = 0;
			while (tmp % x == 0) tmp /= x, cnt++;
			pr.pb(x);
			exp.pb(cnt);
		}
	}
	if (tmp > 1) {
		pr.pb(tmp);
		exp.pb(1);
	}

	// enumerate all divisors g of n
	vl divs;
	genDivs(0, 1, pr, exp, divs);

	ll sum = 0;
	for (ll g : divs) {
		ll x = n / g;                 // x = N/g
		ll phi = phiUsingPrimes(x, pr);
		ll term = (modPow(2, g) * (phi % MOD)) % MOD;
		sum += term;
		if (sum >= MOD) sum -= MOD;
	}

	ll ans = sum;
	ans = (ans + 2) % MOD;
	ans = (ans - (n % MOD) + MOD) % MOD;
	ans = (ans - modPow(2, n) + MOD) % MOD;

	cout << ans << "\n";
}

int main() {
	setIO("gymnasts");
	solve(); // single case
}
