// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // 128-bit integer if needed
using db = long double;  // change to double if TL is tight

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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T, V<T>, greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s){freopen(s.c_str(),"r",stdin);}
	void setOut(string s){freopen(s.c_str(),"w",stdout);}
	void setIO(string s = ""){
		cin.tie(nullptr)->sync_with_stdio(false);
		cin.exceptions(cin.failbit); // comment out if this ever causes issues
		if (sz(s)) {
			setIn(s + ".in");
			setOut(s + ".out");
		}
	}
}

// constants
const db PI = acos((db)-1);
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

// problem-specific constant
const int INF = (int)1e9;

/*
 (1) Problem recap in simple terms:
     - There are n snakes in a fixed order. We put consecutive snakes into cages.
       The cage size for a group is the length of the longest snake in that group.
       We can resize cages at most k times. Total cage capacity is the sum over
       groups of (group size * cage size). We want to minimize wasted space:
       total cage capacity - sum of snake lengths.
 (2) Things to notice and step-by-step derivation:
     - It’s enough to track the first i snakes and how many size changes j we used.
       For that prefix, we only care about the minimum total cage capacity.
     - For dp[i][j], the last group is some segment [c .. i-1] (0-based). Its cost
       is max length on that segment times its length (i - c). The prefix [0 .. c-1]
       has cost dp[c][j-1].
     - So we try all possible c for the last group and keep a running maximum of
       snake length as we expand the last group backwards.
     - The total snake length is easy to keep as a running sum; we subtract it once
       at the end to get the wasted space.
 (3) Solution outline:
     - Read n, k and the snake lengths into a 0-based array a.
     - Define dp[i][j] = minimum total cage capacity to hold the first i snakes
       (snakes 0..i-1) using j size changes.
     - Base: dp[0][j] = 0 for all j (no snakes => no capacity, even if we "waste"
       some of the allowed changes). This is crucial so we can have “at most k”
       changes and still just read dp[n][k].
     - For j = 0, all snakes 0..i-1 are in one cage with size equal to the maximum
       of that prefix.
     - For j >= 1, for each i we:
         * walk c from i-1 down to 0, maintain the max length on [c..i-1],
         * transition dp[i][j] = min(dp[i][j], dp[c][j-1] + maxLen * (i - c)).
     - Sum all snake lengths into sum_len and output dp[n][k] - sum_len.
*/

inline void solve() {
	int n, k;
	cin >> n >> k;

	vi a(n);
	for (int i = 0; i < n; i++) {
		cin >> a[i];
	}

	// dp[i][j] = minimum total cage capacity to hold first i snakes (0..i-1) using j changes.
	V<vi> dp(n + 1, vi(k + 1, INF));

	// Base: 0 snakes => 0 capacity, even if we "use" some changes.
	for (int j = 0; j <= k; j++) {
		dp[0][j] = 0;
	}

	int sum_len = 0;
	int mx_prefix = 0;

	for (int i = 1; i <= n; i++) {
		// Update prefix maximum and base case with no changes (all in one cage).
		mx_prefix = max(mx_prefix, a[i - 1]);
		dp[i][0] = mx_prefix * i;

		sum_len += a[i - 1];

		// Now consider using 1..k changes.
		for (int j = 1; j <= k; j++) {
			int mx_len = 0; // max snake length in the last group [c..i-1]

			// Try all possible starting points of the last group.
			for (int c = i - 1; c >= 0; c--) {
				mx_len = max(mx_len, a[c]); // extend the last group backwards
				dp[i][j] = min(dp[i][j], dp[c][j - 1] + mx_len * (i - c));
			}
		}
	}

	// wasted space = total cage capacity - sum of snake lengths
	cout << dp[n][k] - sum_len << '\n';
}

int main() {
	setIO("snakes"); 
	solve(); // single case
}
