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
     For every subarray, repeatedly merge adjacent pairs, alternating min then max (starting with
     min), and choose merges to maximize the final remaining number. Sum these best values over
     all subarrays.

 (2) Things to notice and step-by-step derivation:
     - For length >= 5 odd, the best final value is always the maximum element of the subarray.
     - For length >= 8 even, the best final value is always the second maximum element.
     - The only lengths that can differ from these rules are 3, 4, and 6 (length 1/2 already match).
     - So we sum max over odd-length subarrays + second max over even-length subarrays, then apply
       small-length corrections.

 (3) Solution outline:
     - Sweep right endpoint i. For each parity of the left endpoint, keep segments with the same
       (max, second max) and a count of how many subarrays share them.
     - When extending by a[i], merge segments whose second max is <= a[i] and update sums.
     - Add sumMax[parity=i%2] (odd lengths) + sumSecond[parity!=i%2] (even lengths) to the answer.
     - Precompute, for k in {3,4,6}, which 0-1 patterns can achieve the "baseline" value.
     - For each length-k window, build its mask and adjust down to the next max if needed.
     - Time: O(N); Memory: O(N).
 */

inline void solve() {
	int n;
	cin >> n;
	vl a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	V<P<pl,ll>> seg[2];
	ll sumMax[2] = {0, 0};
	ll sumSecond[2] = {0, 0};

	auto mergeSeg = [&](ll x, int t) {
		V<P<pl,ll>> buf;
		while (sz(seg[t]) && x >= seg[t].bk.fi.se) {
			ll curMax = seg[t].bk.fi.fi;
			ll cnt = seg[t].bk.se;
			pl nxt = mp(max(x, curMax), min(x, curMax));

			if (!sz(buf) || buf.bk.fi != nxt) buf.pb(mp(nxt, cnt));
			else buf.bk.se += cnt;

			sumMax[t] -= seg[t].bk.fi.fi * seg[t].bk.se;
			sumSecond[t] -= seg[t].bk.fi.se * seg[t].bk.se;
			seg[t].ppb();
		}
		while (sz(buf)) {
			sumMax[t] += buf.bk.fi.fi * buf.bk.se;
			sumSecond[t] += buf.bk.fi.se * buf.bk.se;
			seg[t].pb(buf.bk);
			buf.ppb();
		}
	};

	bitset<64> dp;
	auto calc01 = [&](int k, int mask) -> int {
		vi bits(k);
		for (int i = 0; i < k; i++) bits[i] = (mask >> i) & 1;

		dp.reset();
		dp[(1 << k) - 1] = 1;

		int best = 0;
		for (int s = (1 << k) - 1; s > 0; s--) {
			if (!dp[s]) continue;
			if (__builtin_popcount(s) == 1) {
				for (int j = 0; j < k; j++) {
					if (s & (1 << j)) best = max(best, bits[j]);
				}
				continue;
			}
			int step = k - __builtin_popcount(s);
			int last = -1;
			for (int j = 0; j < k; j++) {
				if (!(s & (1 << j))) continue;
				if (last != -1) {
					int ns = ((step & 1) ^ (bits[last] > bits[j])) ? (s ^ (1 << last))
					                                               : (s ^ (1 << j));
					dp[ns] = 1;
				}
				last = j;
			}
		}
		return best;
	};

	int pre[7][64];
	for (int k = 0; k < 7; k++) for (int m = 0; m < 64; m++) pre[k][m] = 0;
	for (int k : {3, 4, 6}) {
		for (int m = 0; m < (1 << k); m++) pre[k][m] = calc01(k, m);
	}

	ll ans = 0;
	for (int i = 0; i < n; i++) {
		int t = i & 1, o = t ^ 1;

		mergeSeg(a[i], t);
		mergeSeg(a[i], o);

		seg[t].pb(mp(mp(a[i], -1LL), 1LL));
		sumMax[t] += a[i];
		sumSecond[t] -= 1;

		ans += sumMax[t] + sumSecond[o];
	}

	for (int k : {3, 4, 6}) {
		for (int i = k - 1; i < n; i++) {
			ll mx = 0, smx = 0, tmx = 0;
			for (int j = 0; j < k; j++) {
				ll x = a[i - j];
				if (x > mx) {
					tmx = smx;
					smx = mx;
					mx = x;
				} else if (x > smx) {
					tmx = smx;
					smx = x;
				} else if (x > tmx) {
					tmx = x;
				}
			}

			ll base = (k & 1) ? mx : smx;
			int mask = 0;
			for (int j = 0; j < k; j++) mask |= int(a[i - j] >= base) << j;

			if (!pre[k][mask]) ans += (k & 1) ? (smx - mx) : (tmx - smx);
		}
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
