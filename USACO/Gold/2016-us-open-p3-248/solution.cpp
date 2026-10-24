// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default)
using db = long double;  // change to double if TL is tight (almost never)

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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;

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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   We have an array. If two adjacent segments can each be fully merged into the same value x,
   then those two segments can merge into a single value x+1. Find the maximum value achievable.

2) Things to notice + step-by-step:
   - For any subarray [i..j], either it can merge into one number (some value), or it can’t.
   - If we split at k, and [i..k] merges to x and [k+1..j] merges to x, then [i..j] merges to x+1.
   - This is a classic interval DP over all subarrays.

3) Solution outline:
   - dp[i][j] = resulting value if subarray [i..j] can merge into one number, else -1.
   - Base: dp[i][i] = a[i].
   - Transition: dp[i][j] = max over k where dp[i][k] == dp[k+1][j] != -1 of dp[i][k] + 1.
   - Track the maximum dp[i][j]. Time: O(N^3).
*/

inline void solve() {
	int n;
	cin >> n;
	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	V<vi> dp(n, vi(n, -1));
	int ans = 0;

	for (int i = n - 1; i >= 0; i--) {
		dp[i][i] = a[i];
		ans = max(ans, dp[i][i]);
		for (int j = i + 1; j < n; j++) {
			for (int k = i; k < j; k++) {
				if (dp[i][k] == -1 || dp[k + 1][j] == -1) continue;
				if (dp[i][k] == dp[k + 1][j]) {
					dp[i][j] = max(dp[i][j], dp[i][k] + 1);
				}
			}
			ans = max(ans, dp[i][j]);
		}
	}

	cout << ans << "\n";
}

int main() {
	setIO("248");
	solve(); // single case
}
