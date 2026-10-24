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

/*
1) Problem recap:
   Count the number of length-n colorings with m colors that contain at least one streak of ≥ k equal colors.
2) Things to notice + step-by-step:
   Let dp[i] = # “bad” colorings of length i with NO streak of length ≥ k. For i<k, dp[i]=m^i.
   For i≥k, the last block length is 1..k-1, and its color must differ from the previous block:
   dp[i] = (m-1) * (dp[i-1] + ... + dp[i-k+1]). Use prefix sums to get this in O(1).
3) Solution outline:
   Maintain pref[i] = dp[0]+...+dp[i]. Then dp[i]=(m-1)*(pref[i-1]-pref[i-k]) for i≥k.
   Answer = m^n - dp[n] (mod). Time complexity: O(n).
*/

inline ll modpow(ll a, ll e) {
	a %= MOD;
	if (a < 0) a += MOD;
	ll r = 1;
	while (e > 0) {
		if (e & 1) r = (ll)((i128)r * a % MOD);
		a = (ll)((i128)a * a % MOD);
		e >>= 1;
	}
	return r;
}

inline int norm(ll x) {
	x %= MOD;
	if (x < 0) x += MOD;
	return (int)x;
}

inline void solve() {
	int n, k;
	ll m;
	cin >> n >> m >> k;

	ll mm = m % MOD;
	ll m1 = (mm - 1 + MOD) % MOD;

	// pref[i] = sum_{j=0..i} dp[j] (dp[j] = “bad” length-j colorings)
	vi pref(n + 1, 0);
	pref[0] = 1; // dp[0] = 1 (empty)

	ll p = 1;    // p = m^i while i<k
	int dpn = 0; // dp[n] at the end

	for (int i = 1; i <= n; i++) {
		ll dpi = 0;
		if (i < k) {
			p = (ll)((i128)p * mm % MOD);
			dpi = p;
		} else {
			ll window = pref[i - 1] - pref[i - k];
			window = norm(window);
			dpi = (ll)((i128)m1 * window % MOD);
		}
		pref[i] = norm((ll)pref[i - 1] + dpi);
		if (i == n) dpn = (int)dpi;
	}

	ll total = modpow(mm, n);
	cout << norm(total - dpn) << "\n";
}

int main() {
	setIO("spainting");
	solve(); // single case
}
