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
     Count how many length-n score arrays (values in [1..c]) match q facts (a, h): cow h is the
     first index whose score is strictly greater than every score among cows 1..a (answer mod 1e9+7).

 (2) Things to notice and step-by-step derivation:
     - Each constraint (a, h) behaves like a range [a, h-1]; if two ranges overlap, they must share
       the same h, and only the smaller a matters (the other is redundant).
     - After compressing by h and sorting by a, these ranges are disjoint and appear left-to-right.
     - DP over constraints: dp[i][j] = ways to satisfy the first i constraints with score at h_i = j.
       Use dp[i][j-1] monotonicity and a running prefix sum over dp[i-1][k] to handle transitions.

 (3) Solution outline:
     - Read constraints, keep for each h the minimum a, and add a sentinel (0, 0); sort pairs by a.
     - For each constraint i, compute gap1 = a_i - h_{i-1} and gap2 = h_i - a_i - 1, then update
       dpCur[j] using the official recurrence with fast exponentiation and a running prefSum.
     - Answer is (sum_j dpLast[j]) * c^(n - last_h) mod MOD.
     - Time: O(q * c * log n), Memory: O(c).
*/

static inline ll modPow(ll a, ll e) {
	a %= MOD;
	if (a < 0) a += MOD;
	ll r = 1;
	for (; e > 0; e >>= 1) {
		if (e & 1) r = (r * a) % MOD;
		a = (a * a) % MOD;
	}
	return r;
}

inline void solve() {
	ll n; int q, c;
	cin >> n >> q >> c;

	unordered_map<ll,ll> bestA;
	bestA.reserve(q * 2 + 5);
	bestA[0] = 0; // sentinel

	for (int i = 0; i < q; i++) {
		ll a, h; cin >> a >> h;
		auto it = bestA.find(h);
		if (it == bestA.end()) bestA[h] = a;
		else it->se = min(it->se, a);
	}

	vpl pairs; pairs.reserve(sz(bestA));
	for (auto &kv : bestA) pairs.eb(kv.se, kv.fi); // (a, h)
	sor(pairs);

	int qq = sz(pairs) - 1; // number of real constraints (excluding sentinel)
	vl dpPrev(c + 1, 0), dpCur(c + 1, 0);
	dpPrev[0] = 1;

	for (int i = 1; i <= qq; i++) {
		ll gap1 = pairs[i].fi - pairs[i - 1].se;
		ll gap2 = pairs[i].se - pairs[i].fi - 1;

		fill(all(dpCur), 0);
		ll prefSum = 0;

		for (int j = 1; j <= c; j++) {
			ll term1 = dpCur[j - 1];

			ll term2 = dpPrev[j - 1] * modPow(j - 1, gap1 + gap2) % MOD;

			ll diff = (modPow(j - 1, gap1) - modPow(j - 2, gap1)) % MOD;
			if (diff < 0) diff += MOD;

			ll term3 = prefSum * diff % MOD * modPow(j - 1, gap2) % MOD;

			dpCur[j] = (term1 + term2 + term3) % MOD;

			prefSum += dpPrev[j - 1];
			if (prefSum >= MOD) prefSum -= MOD;
		}

		dpPrev.swap(dpCur);
	}

	ll sumWays = 0;
	for (int j = 0; j <= c; j++) {
		sumWays += dpPrev[j];
		if (sumWays >= MOD) sumWays -= MOD;
	}

	ll lastH = pairs[qq].se;
	ll ans = sumWays * modPow(c, n - lastH) % MOD;
	cout << ans << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
