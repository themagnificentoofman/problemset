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
     Given an array where all subarray sums are distinct, for each index i we want the minimum
     amount to change a[i] so that two different subarrays end up having the same sum.

 (2) Things to notice and step-by-step derivation:
     - Changing a[i] by delta changes the sum of every subarray containing i by exactly delta.
     - So two subarrays can become equal only if exactly one of them contains i; then the needed
       |delta| equals the difference of their original sums.
     - For a fixed i, we want the minimum difference between a "contains i" sum and a
       "doesn't contain i" sum; in sorted order of all sums, this minimum happens on adjacent
       sums.
     - For an adjacent pair of subarrays, the indices where they differ form up to two disjoint
       segments (their symmetric difference). Any i in those segments can use this diff.

 (3) Solution outline:
     - Enumerate all subarrays (l,r,sum) using prefix sums, then sort by sum.
     - For each adjacent pair in sorted order, let diff be their sum difference, and update the
       best answer for the two symmetric-difference segments.
     - Propagate interval minima inward so ans[i][i] becomes the minimum diff over all segments
       that contain i.
     - Output ans[i][i] for i=1..n.
     - Time: O(n^2 log n + n^2); Memory: O(n^2).
 */

struct Sub {
	int l, r;
	ll sum;
	bool operator<(const Sub& o) const { return sum < o.sum; }
};

inline void solve() {
	int n; cin >> n;
	vl a(n + 1, 0), pref(n + 1, 0);
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
		pref[i] = pref[i - 1] + a[i];
	}

	V<Sub> subs;
	subs.reserve(1LL * n * (n + 1) / 2);
	for (int r = 1; r <= n; r++) {
		for (int l = 1; l <= r; l++) {
			subs.pb({l, r, pref[r] - pref[l - 1]});
		}
	}
	sor(subs);

	V<vl> ans(n + 2, vl(n + 2, BIG));

	for (int i = 1; i < sz(subs); i++) {
		Sub a = subs[i - 1], b = subs[i];
		ll diff = b.sum - a.sum;

		int loL = min(a.l, b.l), hiL = max(a.l, b.l);
		int loR = min(a.r, b.r), hiR = max(a.r, b.r);

		// left-only segment: [loL .. min(loR, hiL-1)]
		int l1 = loL;
		int r1 = min(loR, hiL - 1);
		if (l1 <= r1) ans[l1][r1] = min(ans[l1][r1], diff);

		// right-only segment: [max(hiL, loR+1) .. hiR]
		int l2 = max(hiL, loR + 1);
		int r2 = hiR;
		if (l2 <= r2) ans[l2][r2] = min(ans[l2][r2], diff);
	}

	for (int l = 1; l <= n; l++) {
		for (int r = n; r > l; r--) {
			ans[l + 1][r] = min(ans[l + 1][r], ans[l][r]);
			ans[l][r - 1] = min(ans[l][r - 1], ans[l][r]);
		}
	}

	for (int i = 1; i <= n; i++) {
		cout << ans[i][i] << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
