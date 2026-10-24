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
const int MXN = 305; // max N for this problem

/*
 (1) Problem recap in simple terms:
     There are N pies in a line and M cows. Cow i has weight w[i] and can eat exactly one pie in
     the interval [l[i], r[i]]. No pie can be eaten by more than one cow. Find the maximum total
     weight of cows that can be assigned a pie.

 (2) Things to notice and step-by-step derivation:
     - If we decide that pie c is eaten by the heaviest cow we use inside interval [a, b], then all
       other chosen cows must eat in [a, c-1] or [c+1, b], so the problem splits.
     - For each center pie c and interval [a, b] containing c, define bestAt[c][a][b] as the maximum
       weight of a cow whose range is within [a, b] and includes c.
     - We can fill bestAt by placing weights on exact [l, r] ranges, then expanding intervals
       outward (smaller to larger) using max transitions.

 (3) Solution outline:
     - Read all cows; for each cow (w, l, r) and each c in [l, r], set bestAt[c][l][r] = max(...).
     - For each fixed c, expand bestAt[c][a][b] so it is correct for every a <= c <= b.
     - Interval DP: dp[a][b] is the best answer using only pies in [a, b].
       - Split: dp[a][mid] + dp[mid+1][b]
       - Use a cow at c: bestAt[c][a][b] + dp[a][c-1] + dp[c+1][b]
     - Time complexity: O(N^3); Memory complexity: O(N^3).
 */

static int bestAt[MXN][MXN][MXN];
static int dp[MXN][MXN];

template<class T> static inline bool ckmax(T& a, const T& b) {
	if (a < b) { a = b; return 1; }
	return 0;
}

inline void solve() {
	int n, m;
	cin >> n >> m;

	memset(bestAt, 0, sizeof(bestAt));
	memset(dp, 0, sizeof(dp));

	for (int i = 0; i < m; i++) {
		int w, l, r;
		cin >> w >> l >> r;
		l--, r--;
		for (int c = l; c <= r; c++) ckmax(bestAt[c][l][r], w);
	}

	for (int c = 0; c < n; c++) {
		for (int l = c; l >= 0; l--) {
			for (int r = c; r < n; r++) {
				if (l) ckmax(bestAt[c][l - 1][r], bestAt[c][l][r]);
				if (r + 1 < n) ckmax(bestAt[c][l][r + 1], bestAt[c][l][r]);
			}
		}
	}

	for (int len = 1; len <= n; len++) {
		for (int l = 0; l + len - 1 < n; l++) {
			int r = l + len - 1;

			for (int mid = l; mid < r; mid++) {
				ckmax(dp[l][r], dp[l][mid] + dp[mid + 1][r]);
			}

			for (int c = l; c <= r; c++) {
				int w = bestAt[c][l][r];
				if (!w) continue;
				int res = w;
				if (c > l) res += dp[l][c - 1];
				if (c < r) res += dp[c + 1][r];
				ckmax(dp[l][r], res);
			}
		}
	}

	cout << dp[0][n - 1] << '\n';
}

int main() {
	setIO("pieaters");
	solve(); // single case
}
