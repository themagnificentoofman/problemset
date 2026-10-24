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
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple terms):
   You scan a length-N string. Keep an integer "need".
   If you see 'M', multiply the answer by C(need+K, K) and then do need += K.
   Otherwise do need -= 1.
   Output (answer)^L mod 1e9+7.

2) Things to notice and step-by-step derivation:
   - We only ever need binomials of the form C(x, K), so factorials up to max(x) are enough.
   - Do one dry pass to find maxN = max(need+K) over all 'M' positions.
   - Precompute fact/ifact up to maxN, then do the real pass multiplying C(need+K, K).

3) Solution outline:
   - First pass: track need and maxN.
   - Precompute fact and ifact up to maxN.
   - Second pass: compute product of binomials, then raise to power L.
   - Time: O(N + maxN).
*/

static inline int addmod(int a, int b) {
	a += b;
	if (a >= MOD) a -= MOD;
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
static inline int modinv(int a) {
	return modpow(a, MOD - 2);
}

inline void solve() {
	int K, N;
	ll L;
	cin >> K >> N >> L;

	str s;
	cin >> s;

	int need = 0, maxN = 0;
	for (char ch : s) {
		if (ch == 'M') {
			maxN = max(maxN, need + K);
			need += K;
		} else {
			need -= 1;
		}
	}

	vi fact(maxN + 1, 1), ifact(maxN + 1, 1);
	for (int i = 1; i <= maxN; i++) fact[i] = mulmod(fact[i - 1], i);
	ifact[maxN] = modinv(fact[maxN]);
	for (int i = maxN; i >= 1; i--) ifact[i - 1] = mulmod(ifact[i], i);

	auto nCk = [&](int n, int k) -> int {
		// assumes 0 <= k <= n <= maxN (true by construction)
		return mulmod(fact[n], mulmod(ifact[k], ifact[n - k]));
	};

	int ans = 1;
	need = 0;
	for (char ch : s) {
		if (ch == 'M') {
			ans = mulmod(ans, nCk(need + K, K));
			need += K;
		} else {
			need -= 1;
		}
	}

	cout << modpow(ans, L) << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
