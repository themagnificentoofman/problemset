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
     Bessie outputs B numbers at fixed times B, B+(B-1), ..., B(B+1)/2, while each helper
     outputs at time = value. We split numbers between Bessie and helpers so the merged
     output is sorted, and we want the finishing time as small as possible.

 (2) Things to notice and step-by-step derivation:
     - If the best time is < max(a), then max(a) cannot be a helper (it would finish at
       time max(a)), so Bessie must handle it.
     - After sorting a, we can assume helpers take a prefix and Bessie takes a suffix; if
       there are equal values, we can swap equal copies between piles without changing the
       final array, so we just need to check if some split works.
     - For a fixed B>0, if sorting is possible then all helper outputs happen before
       Bessie's first output (time B), so the total time for this B is just B(B+1)/2.

 (3) Solution outline:
     - Sort the array.
     - Binary search the smallest B in [1..n] for which a valid division exists.
     - For a fixed B, do an O(n) scan: for each a[i], count how many of Bessie's outputs
       happen by time a[i], and check if we can place exactly n-B helpers before the end.
     - Compare this Bessie time B(B+1)/2 with the always-valid all-helpers time max(a).
     - Time: O(n log n) per test case; Memory: O(n).
 */

// sum(l ... r)
ll arith(ll l, ll r) {
	return (l + r) * (r - l + 1) / 2;
}

inline void solve() {
	int n; cin >> n;
	vl a(n);
	for (auto& x : a) cin >> x;
	sor(a);

	auto possible = [&](int b) -> bool {
		if (b == n) return true;

		int numBefore = 0;
		for (int i = 0; i + 1 < n; i++) {
			while (numBefore < b && arith(b - numBefore, b) <= a[i]) numBefore++;
			if (i + 1 - numBefore == n - b) return true;
		}
		return false;
	};

	int lo = 1, hi = n;
	while (lo < hi) {
		int mid = (lo + hi) / 2;
		if (possible(mid)) hi = mid;
		else lo = mid + 1;
	}

	cout << min(arith(1, lo), a.bk) << '\n';
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while (t--) solve();
}
