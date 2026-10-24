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
     We run Bessie's "quickish" sort, where each bubble-sort pass costs the current subarray length, and
     we need the final total cost (work_counter).

 (2) Things to notice and step-by-step derivation:
     - One bubble-sort pass moves any element left by at most 1 position (it can move right many times).
     - If an element starts at index j and ends at sorted position p (stable: tie-break by index), then
       it needs at least j - p passes before it can reach position p.
     - A partition point after the first i sorted elements appears once all those i elements can fit in
       the first i spots: required passes = mx - (i - 1), where mx is the max original index among them.

 (3) Solution outline:
     - Build the stable sorted order of indices by (A[i], i).
     - Sweep i = 1..N-1, maintain mx = max original index in the first i sorted elements, and store
       need[i] = mx + 1 - i for the boundary between i-1 and i (in sorted order).
     - Position p keeps getting bubbled until both adjacent boundaries are valid, so its pass count is
       max(need[p], need[p+1]) (missing sides treated as 0), but at least 1 if N > 1.
     - Sum these pass counts over all positions to get work_counter.
     - Time: O(N log N); Memory: O(N)
 */

inline void solve() {
	int n; cin >> n;
	vl a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	vi ord(n);
	iota(all(ord), 0);

	sort(all(ord), [&](int i, int j) {
		if (a[i] != a[j]) return a[i] < a[j];
		return i < j; // stable tie-break
	});

	vi need(n + 1, 0);
	int mx = 0;
	for (int i = 1; i <= n - 1; i++) {
		mx = max(mx, ord[i - 1]);
		need[i] = mx + 1 - i; // mx - (i - 1)
	}

	ll ans = 0;
	for (int p = 0; p < n; p++) {
		int passes = 0;
		if (p > 0) passes = max(passes, need[p]);
		if (p < n - 1) passes = max(passes, need[p + 1]);
		if (passes == 0 && n > 1) passes = 1;
		ans += passes;
	}

	cout << ans << '\n';
}

int main() {
	setIO("sort");
	solve(); // single case
}
