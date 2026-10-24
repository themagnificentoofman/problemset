// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t;
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
template<class T, size_t SZ> using AR = array<T,SZ>;
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
   We have two lists of cow breed IDs (length N). We want the maximum number of pairs (i, j)
   such that i and j increase (no crossing lines) and |A[i] - B[j]| <= 4.

2) Things to notice + step-by-step:
   - This is the same structure as LCS: either skip A[i], skip B[j], or match them.
   - A match is allowed when the breed IDs differ by at most 4.
   - Let dp[i][j] = best answer using first i items of A and first j items of B.

3) Solution outline:
   - Transition:
       dp[i][j] = max(dp[i-1][j], dp[i][j-1])
       if |A[i]-B[j]| <= 4, dp[i][j] = max(dp[i][j], dp[i-1][j-1] + 1)
   - Answer is dp[N][N].
   - Time: O(N^2).
*/

inline void solve() {
	int n;
	cin >> n;

	vi a(n + 1), b(n + 1);
	for (int i = 1; i <= n; i++) cin >> a[i];
	for (int i = 1; i <= n; i++) cin >> b[i];

	V<vi> dp(n + 1, vi(n + 1, 0));

	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
			if (abs(a[i] - b[j]) <= 4) {
				dp[i][j] = max(dp[i][j], dp[i - 1][j - 1] + 1);
			}
		}
	}

	cout << dp[n][n] << "\n";
}

int main() {
	setIO("nocross");
	solve(); // single case
}
