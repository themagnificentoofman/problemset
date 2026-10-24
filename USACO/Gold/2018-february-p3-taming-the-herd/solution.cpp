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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
We have a log a[0..n-1]. If day i is a breakout day, then a[i]=0, and after that it should increase by 1 each day until the next breakout.
Some entries are unknown (-1). We can change any entry (cost 1) to make the whole log valid. For every k=1..n, output the minimum changes
to make the log have exactly k breakout days.

2) Things to notice + derivation:
If we decide a breakout happens at day l, then for any day j in that segment (l..r) we must have a[j] = j-l.
So we can precompute cost[l][r] = number of forced mismatches in [l..r] (only counting positions where a[j] != -1).
Then the whole log is just splitting days into k contiguous segments, where each segment start is a breakout day.

3) Solution outline:
Precompute cost[l][r] in O(n^2).
DP: dp[k][i] = min changes to fix first i days (0..i-1) with exactly k segments/breakouts.
Transition: dp[k][i] = min over p<i of dp[k-1][p] + cost[p][i-1].
Answer for each k is dp[k][n]. Overall O(n^3), fine for n<=100.
*/

inline void solve() {
	const int INF = (int)1e9;

	int n; cin >> n;
	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	// cost[l][r]: segment starts at l (breakout), expected a[j]=j-l for l<=j<=r
	V<vi> cost(n, vi(n, 0));
	for (int l = 0; l < n; l++) {
		int cur = 0;
		for (int r = l; r < n; r++) {
			int need = r - l;
			if (a[r] != -1 && a[r] != need) cur++;
			cost[l][r] = cur;
		}
	}

	// dp[k][i]: first i days, exactly k breakouts/segments
	V<vi> dp(n + 1, vi(n + 1, INF));
	dp[0][0] = 0;

	for (int k = 1; k <= n; k++) {
		for (int i = 1; i <= n; i++) {
			for (int p = 0; p < i; p++) {
				dp[k][i] = min(dp[k][i], dp[k - 1][p] + cost[p][i - 1]);
			}
		}
	}

	for (int k = 1; k <= n; k++) {
		cout << dp[k][n] << "\n";
	}
}

int main() {
	setIO("taming");
	solve(); // single case
}
