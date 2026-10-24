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
     We have N cows on a line (position x[i], weight y[i]). We may pair two cows if their
     positions differ by at most K, and each cow is used in at most one pair. The pairing must
     be maximal (there do not exist two unpaired cows within distance K). For T=1 output the
     minimum total weight of unpaired cows; for T=2 output the maximum.

 (2) Things to notice and step-by-step derivation:
     - If x[i+1]-x[i] > K, no pair can cross that gap, so each segment can be solved separately.
     - Inside one segment:
       - T=1: if length is even, we can pair all cows; if odd, any maximal pairing leaves
         exactly one unpaired cow. A middle i can be that cow only if neighbors can pair across
         it, i.e., x[i+1]-x[i-1] <= K.
       - T=2: if we leave i unpaired, then every cow with x in (x[i], x[i]+K] must be paired,
         so we jump to ub = first index with x[ub] > x[i]+K and do a 2-parity DP.

 (3) Solution outline:
     - Split cows into segments where consecutive gaps are <= K.
     - T=1: for each odd-length segment, take the minimum weight among valid "lone unpaired"
       positions and add it.
     - T=2: for each segment, compute dp[i][parity] from right to left using upper_bound jumps.
     - Sum segment answers and print.
     - Time: O(N log N); Memory: O(N).
 */

inline void solve() {
	int t, n, k;
	cin >> t >> n >> k;

	auto minSpanCost = [&](vpi& span) -> ll {
		int m = sz(span);
		if ((m & 1) == 0) return 0;
		int best = 0x3f3f3f3f;
		for (int i = 0; i < m; i++) {
			bool bridge = (i > 0 && i + 1 < m && span[i + 1].fi - span[i - 1].fi <= k);
			if ((i & 1) == 0 || bridge) best = min(best, span[i].se);
		}
		return (best == 0x3f3f3f3f ? 0LL : (ll)best);
	};

	auto maxSpanCost = [&](vpi& span) -> ll {
		int m = sz(span);
		if (!m) return 0LL;

		vpl dp(m + 1, mp(0LL, -BIG));
		dp[m] = mp(0LL, -BIG);

		for (int i = m - 1; i >= 0; i--) {
			dp[i] = dp[i + 1];

			int lim = span[i].fi + k;
			int j = upb<pi>(span, pi(lim, 0x3f3f3f3f));

			bool endpoint = (i == 0 || i == m - 1);
			bool bridge = (!endpoint && span[i + 1].fi - span[i - 1].fi <= k);

			bool okEven = endpoint || bridge || (((m - i) & 1) == 0);
			bool okOdd  = endpoint || bridge || (((m - i) & 1) == 1);

			ll w = span[i].se;
			if (okEven) dp[i].fi = max(dp[i].fi, dp[j].se + w);
			if (okOdd)  dp[i].se = max(dp[i].se, dp[j].fi + w);
		}

		return (m & 1) ? dp[0].se : dp[0].fi;
	};

	ll ans = 0;
	vpi span;
	span.reserve(n);

	int prevX = 0;
	for (int i = 0; i < n; i++) {
		int x, y;
		cin >> x >> y;

		if (i && x - prevX > k) {
			if (t == 1) ans += minSpanCost(span);
			else ans += maxSpanCost(span);
			span.clear();
		}
		span.pb(mp(x, y));
		prevX = x;
	}

	if (t == 1) ans += minSpanCost(span);
	else ans += maxSpanCost(span);

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
