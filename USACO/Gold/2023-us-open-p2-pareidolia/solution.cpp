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
     We can delete characters from a string (paying a given cost per deleted character). After
     deletions, we want to maximize how many times the substring "bessie" appears contiguously,
     and among all ways achieving that maximum, minimize the total deletion cost.

 (2) Things to notice and step-by-step derivation:
     - Characters outside all chosen "bessie" blocks never need to be deleted, since they don't
       affect the count and only increase cost.
     - While building one "bessie" block, any extra characters between needed letters must be
       deleted (to keep the substring contiguous after deletions).
     - DP over position and how many letters of "bessie" we have matched in the current block:
       maximize (#blocks), and on ties minimize (cost).

 (3) Solution outline:
     - Let dp[j] be the best pair (blocks, cost) after scanning up to i, with j matched chars of
       the current "bessie" (j=0 means not currently inside a block).
     - For j>=1: either delete s[i] (pay cost, stay at j) or match it (if it equals target[j-1]).
     - For j=0: carry dp[0] (keep character for free), or if we just reached j=6, convert it to
       one completed block and reset to j=0.
     - Time: O(n); Memory: O(1).
 */

static inline pl opt(const pl& a, const pl& b) {
	if (a.fi != b.fi) return (a.fi > b.fi ? a : b);
	return (a.se < b.se ? a : b);
}

static inline pl add(const pl& a, const pl& b) {
	if (a.fi < 0 || b.fi < 0) return {-1, BIG};
	return {a.fi + b.fi, a.se + b.se};
}

inline void solve() {
	str s; cin >> s;
	int n = sz(s);

	vl c(n);
	for (int i = 0; i < n; i++) cin >> c[i];

	const str TARGET = "bessie";
	const int m = sz(TARGET);

	AR<pl, 7> dp, ndp;
	for (int j = 0; j <= m; j++) dp[j] = {-1, BIG};
	dp[0] = {0, 0};

	for (int i = 0; i < n; i++) {
		for (int j = 0; j <= m; j++) ndp[j] = {-1, BIG};

		for (int j = 1; j <= m; j++) {
			pl del = add(dp[j], {0, c[i]});
			pl take = (s[i] == TARGET[j - 1] ? dp[j - 1] : pl{-1, BIG});
			ndp[j] = opt(del, take);
		}

		ndp[0] = opt(dp[0], add(ndp[m], {1, 0}));
		dp = ndp;
	}

	cout << dp[0].fi << '\n' << dp[0].se << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
