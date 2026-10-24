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
// lower_bound / upper_bound convenience; use on sorted containers
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
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)2e5+5;
const int MAX_A = (int)1e9;

/*
 (1) Problem recap in simple terms:
     We are given the minimum of every length-K window of an unknown length-N array (values are in
     [1, 1e9]). Count how many arrays match all these window minimums, modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - Fix a minimum value m. Then each position is either exactly m, or strictly larger than m.
     - For a block where all window minimums equal m, the constraint becomes: every length-K window
       must contain at least one position equal to m (so we cannot have K consecutive "larger" values).
     - This can be counted by a 1D DP with a "no run of K larger values" recurrence, where each larger
       position has (1e9 - m) choices.

 (3) Solution outline:
     - Compress consecutive equal window minimums into segments (value, count).
     - For each segment, compute an effective length L (how many array positions this minimum controls)
       based on whether neighboring segment values are higher or lower.
     - Multiply countSeq(value, K, L) over all segments, modulo MOD.
     - Time: O(N), Memory: O(N)
 */

inline int addMod(int a, int b) {
	a += b;
	if (a >= MOD) a -= MOD;
	return a;
}
inline int subMod(int a, int b) {
	a -= b;
	if (a < 0) a += MOD;
	return a;
}
inline int mulMod(int a, int b) { return int((ll)a * b % MOD); }
inline void addEq(int& a, int b) { a = addMod(a, b); }
inline void subEq(int& a, int b) { a = subMod(a, b); }
inline void mulEq(int& a, int b) { a = mulMod(a, b); }

inline int modPow(int a, ll e) {
	int r = 1;
	while (e > 0) {
		if (e & 1) r = mulMod(r, a);
		a = mulMod(a, a);
		e >>= 1;
	}
	return r;
}

inline int countSeq(int mn, int k, int len) {
	// sequences of length len over {mn} U {mn+1..MAX_A} where every K-window has min mn
	if (len <= 0) return 1;
	int succ = MAX_A - mn;      // choices for a "larger" position
	int baseAll = succ + 1;     // mn or larger

	if (len < k) return modPow(baseAll, len);

	vi dp(len + 2, 0);
	dp[0] = 1;
	dp[1] = 1;

	int curPow = 1; // baseAll^(i-1)
	for (int i = 2; i <= k; i++) {
		curPow = mulMod(curPow, baseAll);
		dp[i] = curPow;
	}

	int succPow = modPow(succ, k - 1);
	for (int i = k; i <= len; i++) {
		int x = dp[i];
		subEq(x, mulMod(dp[i - k], succPow));
		x = mulMod(x, succ);
		addEq(x, dp[i]);
		dp[i + 1] = x;
	}
	return dp[len + 1];
}

inline void solve() {
	int n, k;
	cin >> n >> k;

	int m = n - k + 1;
	vi c(m);
	for (int i = 0; i < m; i++) cin >> c[i];

	vpi seg;
	{
		int cur = c[0], cnt = 1;
		for (int i = 1; i < m; i++) {
			if (c[i] == cur) cnt++;
			else {
				seg.pb({cur, cnt});
				cur = c[i];
				cnt = 1;
			}
		}
		seg.pb({cur, cnt});
	}

	if (sz(seg) == 1) {
		cout << countSeq(seg[0].fi, k, n) << "\n";
		return;
	}

	int ans = 1;
	for (int i = 0; i < sz(seg); i++) {
		int mn = seg[i].fi;
		int cnt = seg[i].se;

		int len = 0;
		if (i == 0) {
			// only has a right neighbor
			len = (seg[1].fi > mn ? cnt - 1 : cnt + k - 1);
		} else if (i == sz(seg) - 1) {
			// only has a left neighbor
			len = (seg[i - 1].fi > mn ? cnt - 1 : cnt + k - 1);
		} else {
			bool leftHigher = (seg[i - 1].fi > mn);
			bool rightHigher = (seg[i + 1].fi > mn);

			if (leftHigher) {
				if (rightHigher) len = max(0, cnt - k - 1);
				else len = cnt - 1;
			} else {
				if (rightHigher) len = cnt - 1;
				else len = cnt + k - 1;
			}
		}

		mulEq(ans, countSeq(mn, k, len));
	}

	cout << ans << "\n";
}

int main() {
	setIO("tracking2");
	solve(); // single case
}
