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
const int MXN = (int)305; // modify as needed

/*
 (1) Problem recap (simple):
     We have a color sequence a[0..n-1]. In one operation we can paint any interval [L, R] with a single color,
     overwriting earlier paint. Compute the minimum number of operations to obtain the final sequence.

 (2) Things to notice + step-by-step derivation:
     • Baseline: paint each position separately ⇒ n operations.
     • If a[i] == a[j] and both ends are painted by the same final stroke spanning [i..j], we “merge” two would-be strokes,
       effectively saving 1 operation.
     • Non-crossing structure: such endpoint pairings must be nested/concatenated (no crossings).
     • Define dp[i][j] = maximum number of non-crossing equal-color endpoint pairs we can form in subarray a[i][j].
       - If a[i] == a[j], we can pair them: dp[i][j] = max(dp[i][j], dp[i+1][j-1] + 1).
       - Also try splitting at k: dp[i][j] = max(dp[i][j], dp[i][k] + dp[k][j]) for i < k < j.
     • Answer = n - dp[0][n-1] (start with n singletons, subtract merges).

 (3) Solution outline:
     • Read n and a.
     • Fill dp over increasing segment lengths (iterate i ↓, j ↑) so subproblems are ready.
     • Transitions: pair endpoints if equal; otherwise (and additionally) try all splits.
     • Print n - dp[0][n-1].

 Variables:
     n             : length of the sequence.
     a             : vector of colors of length n.
     dp[i][j]      : max number of disjoint, non-crossing equal-color endpoint pairs inside interval [i..j].
     i, j          : current interval’s left/right endpoints.
     k             : split point to combine two subinterval solutions.
*/

// globals
int n;                        // number of positions
int dp[MXN][MXN];             // dp[i][j]: max # of equal-color endpoint pairs inside [i..j]

inline void solve() {
	// read input
	cin >> n;                 // length of the sequence
	vi a(n);                  // a[t] = color at position t
	for (int &x : a) cin >> x;

	// dp is zero-initialized (global). Base: intervals of length 0 or 1 have 0 pairs.
	// Fill over increasing lengths: ensure (i+1,j-1), (i,k), (k,j) are already computed.
	for (int i = n - 1; ~i; --i) {
		for (int j = i + 1; j < n; ++j) {
			// Option 1: pair endpoints if they’re the same color.
			if (a[i] == a[j]) {
				// If we connect i--j, we add 1 pair plus best inside [i+1..j-1].
				dp[i][j] = max(dp[i][j], dp[i + 1][j - 1] + 1);
			}
			// Option 2: split the interval at k and combine two subintervals.
			for (int k = i + 1; k < j; ++k) {
				dp[i][j] = max(dp[i][j], dp[i][k] + dp[k][j]);
			}
		}
	}

	// Minimum operations = paint each position (n) minus merges (dp[0][n-1]).
	cout << n - dp[0][n - 1];
}

int main() {
	setIO();
	solve(); // single case
}