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
     We know Q fixed positions (including 1 and N) in an array A. After repeatedly pushing boxes
     right whenever a cow has more than the cow behind it, we get a final array B. Count how many
     ways to fill the missing A values so that B equals sorted(A), modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - Only the Q remembered positions matter for stitching the answer; Q ≤ 100, but N can be 5e6.
     - Between two remembered positions, we only care about the gap length k and value difference v.
     - The transition across a gap can be written as a 2x2 matrix; its entries are sums of binomials.
     - We precompute modular inverses up to N to update binomials in O(1) per step.

 (3) Solution outline:
     - Read the Q remembered (c_i, v_i) pairs in increasing c_i order.
     - Precompute inv[x] for x = 1..N+5 to support fast binomial updates mod MOD.
     - DP over remembered points with 2 states; for each adjacent pair, compute s00/s01/s10/s11 by
       iterating j = 0..k and updating binomials incrementally, then apply the 2x2 transition.
     - Output dp[Q-1][0] + dp[Q-1][1] (mod MOD).
     - Time: O(N + sum gaps) = O(N) since sum gaps ≤ N; Memory: O(N) for inverses.
 */

// invN[x] = modular inverse of x mod MOD (1 <= x <= N+5).
static vl invN;

static void precomputeInverses(int maxN) {
	invN.assign(maxN + 1, 0);
	invN[1] = 1;
	for (int i = 2; i <= maxN; i++) {
		invN[i] = (MOD - (ll)(MOD / i) * invN[MOD % i] % MOD) % MOD;
	}
}

// Given current = (n-1)C(k-1), return nCk using nCk = current * n / k (mod MOD).
static inline ll getNextBinomial(int n, int k, ll current) {
	ll num = (current * (ll)n) % MOD;
	return (num * invN[k]) % MOD;
}

static inline ll arrEq(int v, int i, ll current) {
	if (i == 0) return 0;
	if (i == 1) return 1;
	return getNextBinomial(v + i - 2, i - 1, current);
}

static inline ll arrLess(int v, int i, ll current) {
	if (i == 0) return 1;
	return getNextBinomial(v + i - 2, i, current);
}

inline void solve() {
	int n, q;
	cin >> n >> q;

	vpl a(q);
	for (int i = 0; i < q; i++) {
		ll c, v;
		cin >> c >> v;
		--c;               // switch to 0-indexed position
		a[i] = mp(c, v);
	}

	precomputeInverses(n + 5);

	V<AR<ll, 2>> dp(q, AR<ll, 2>{0LL, 0LL});
	dp[0][0] = 1;

	for (int i = 0; i < q - 1; i++) {
		ll s00 = 0, s01 = 0, s10 = 0, s11 = 0;

		int k = int(a[i + 1].fi - a[i].fi - 1);      // number of missing entries in this gap
		int v = int(a[i + 1].se - a[i].se + 1);      // value difference parameter for formulas
		if (v < 0) continue;

		ll arrEqV = 1, arrLessV = 1;
		ll arrEqVp1 = 1, arrLessVp1 = 1;

		ll kChooseJ = 1;
		for (int j = 0; j <= k; j++) {
			arrEqV = arrEq(v, j, arrEqV);
			arrEqVp1 = arrEq(v + 1, j, arrEqVp1);
			arrLessV = arrLess(v, j, arrLessV);
			arrLessVp1 = arrLess(v + 1, j, arrLessVp1);

			s11 = (s11 + arrEqV * kChooseJ) % MOD;
			s10 = (s10 + arrLessV * kChooseJ) % MOD;
			s01 = (s01 + arrEqVp1 * kChooseJ) % MOD;
			s00 = (s00 + arrLessVp1 * kChooseJ) % MOD;

			// Update kC(j+1) from kCj: multiply by (k-j) / (j+1).
			kChooseJ = (kChooseJ * (ll)(k - j)) % MOD;
			kChooseJ = (kChooseJ * invN[j + 1]) % MOD;
		}

		if (v == 0) {
			s01 = (s01 + s11 + s10) % MOD;
			s11 = 0;
			s10 = 0;
			s00 = 0;
		} else if (v == 1) {
			s11 = (s11 + s10) % MOD;
			s10 = 0;
		}

		dp[i + 1][0] = (dp[i + 1][0] + dp[i][0] * s00) % MOD;
		dp[i + 1][0] = (dp[i + 1][0] + dp[i][1] * s10) % MOD;
		dp[i + 1][1] = (dp[i + 1][1] + dp[i][0] * s01) % MOD;
		dp[i + 1][1] = (dp[i + 1][1] + dp[i][1] * s11) % MOD;
	}

	cout << (dp[q - 1][0] + dp[q - 1][1]) % MOD << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
