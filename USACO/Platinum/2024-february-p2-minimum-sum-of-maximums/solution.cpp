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
const int MXN = 305;
const int MXK = 7;
const int MXMASK = (1 << MXK);
const int MXA = 1000000;

/*
 (1) Problem recap in simple terms:
     Solve the minimum possible value of sum_{i=1..N-1} max(a_i, a_{i+1}) after we can freely swap
     any tiles except K stuck positions (their values must stay at those indices).


 (2) Things to notice and step-by-step derivation:
     - Stuck tiles split the line into independent gaps; each gap has fixed boundary values on both sides.
     - For a gap of length L, we only choose which L movable values go into it (their best order is sorted).
     - With sorted values v1<=...<=vL in a gap and boundary values bL,bR, the variable part depends only on
       v1 (smallest), vL (largest), and (min/max of bL,bR); middle values contribute in a fixed "baseline"
       way.
     - K<=6 => the number of non-empty gaps is at most K+1<=7, so we can DP over subsets of gaps.


 (3) Solution outline:
     - Read input, mark stuck indices, collect all movable values and sort them.
     - Build all non-empty gaps as (lo, hi, len), where lo=min(boundaries) and hi=max(boundaries).
       Also accumulate the fixed contributions from adjacent stuck tiles.
     - Interval/subset DP: dp[L][R][mask] = minimum extra cost using values vals[L..R] to fill exactly the
       gaps in mask (each gap k needs len[k] values).
     - Combine dp with the baseline sum of chosen values and boundary highs to get the final answer.
     - Time: O(N^2 * 3^M). Memory: O(N^2 * 2^M).
 */

struct Gap {
	int lo, hi, len;
};

inline void solve() {
	int n, k; cin >> n >> k;
	vi a(n+1);
	for (int i = 1; i <= n; i++) cin >> a[i];

	vi x(k);
	vb stuck(n+1, false);
	for (int i = 0; i < k; i++) {
		cin >> x[i];
		stuck[x[i]] = true;
	}

	// If everything is stuck (possible when n<=6), swaps do nothing; compute directly.
	int freeCnt = 0;
	for (int i = 1; i <= n; i++) freeCnt += !stuck[i];
	if (freeCnt == 0) {
		ll ans = 0;
		for (int i = 1; i < n; i++) ans += max(a[i], a[i+1]);
		cout << ans << '\n';
		return;
	}

	// fixed = (position, value), plus sentinel endpoints.
	vpi fixed;
	fixed.eb(0, MXA);
	for (int i = 1; i <= n; i++) if (stuck[i]) fixed.eb(i, a[i]);
	fixed.eb(n+1, MXA);
	sor(fixed);

	// vals = movable values (sorted).
	vi vals;
	for (int i = 1; i <= n; i++) if (!stuck[i]) vals.pb(a[i]);
	sor(vals);
	int f = sz(vals);

	// Build gaps between consecutive fixed tiles.
	V<Gap> gaps;
	ll fixedAdj = 0; // includes sentinel edges; we subtract 2*MXA at the end.
	for (int i = 0; i + 1 < sz(fixed); i++) {
		int p1 = fixed[i].fi, v1 = fixed[i].se;
		int p2 = fixed[i+1].fi, v2 = fixed[i+1].se;
		int len = p2 - p1 - 1;
		if (len == 0) {
			fixedAdj += max(v1, v2);
		} else {
			int lo = min(v1, v2);
			int hi = max(v1, v2);
			gaps.pb({lo, hi, len});
		}
	}

	int m = sz(gaps);
	int full = (1 << m) - 1;

	// sizeSum[mask] = total number of values needed by gaps in mask.
	vi sizeSum(1 << m, 0);
	for (int mask = 1; mask <= full; mask++) {
		int lbm = mask & -mask;
		int bit = __builtin_ctz(lbm);
		sizeSum[mask] = sizeSum[mask ^ lbm] + gaps[bit].len;
	}

	// dp[l][r][mask] = minimum "extra" cost for assigning some of vals[l..r] to gaps in mask.
	static int dp[MXN][MXN][MXMASK];
	const int INF = MOD;

	for (int l = 0; l < f; l++) for (int r = 0; r < f; r++) {
		for (int mask = 0; mask <= full; mask++) dp[l][r][mask] = INF;
	}

	for (int l = f-1; l >= 0; l--) {
		for (int r = l; r < f; r++) {
			for (int mask = 0; mask <= full; mask++) {
				int need = sizeSum[mask];
				int have = r - l + 1;
				if (need > have) continue;

				// Drop smallest or largest unused value (if we can still meet "need").
				if (need <= have - 1) {
					dp[l][r][mask] = min(dp[l][r][mask], dp[l+1][r][mask]);
					dp[l][r][mask] = min(dp[l][r][mask], dp[l][r-1][mask]);
				}

				// Split mask into two non-empty parts; each part takes a consecutive block of values.
				for (int sub = (mask-1) & mask; sub; sub = (sub-1) & mask) {
					int leftNeed = sizeSum[sub];
					int mid = l + leftNeed - 1;
					dp[l][r][mask] = min(dp[l][r][mask],
						dp[l][mid][sub] + dp[mid+1][r][mask ^ sub]);
				}

				// Let some gap k in mask "own" the extremes vals[l] and vals[r].
				for (int kk = 0; kk < m; kk++) if ((mask >> kk) & 1) {
					if (gaps[kk].len == 1 && l < r) continue; // need exactly one value.

					int small = vals[l], big = vals[r];

					// Extra over baseline when connecting boundaries to smallest/largest chosen values.
					int contrib = 0;
					contrib += max(big, gaps[kk].hi) - gaps[kk].hi;
					contrib += max(small, gaps[kk].lo) - small;

					int restMask = mask ^ (1 << kk);
					if (restMask) {
						// Need at least two values to reserve extremes for this gap.
						if (l + 1 <= r - 1) contrib += dp[l+1][r-1][restMask];
						else continue;
					}
					dp[l][r][mask] = min(dp[l][r][mask], contrib);
				}
			}
		}
	}

	ll extra = dp[0][f-1][full];

	// Baseline = sum(movable vals) + sum(gap highs) + fixed adjacencies, then remove sentinels.
	ll base = 0;
	for (int v : vals) base += v;
	for (auto g : gaps) base += g.hi;
	base += fixedAdj;
	base -= 2LL * MXA;

	cout << (base + extra) << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
