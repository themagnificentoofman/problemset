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
   John and Bessie start at given grid points and each has a move string (N/E/S/W).
   Each time unit, we may advance John by 1 move, Bessie by 1 move, or both (until both finish).
   Energy cost for a time unit is squared distance between their positions at that time.
   Minimize total energy.

2) Things to notice + step-by-step:
   - After i moves of John and j moves of Bessie, their positions are fixed.
   - So dp[i][j] = minimum energy to reach state (i, j).
   - From (i, j), we can go to:
       (i+1, j)   (only John moves),
       (i, j+1)   (only Bessie moves),
       (i+1, j+1) (both move),
     adding the squared distance at the new positions for that step.

3) Solution outline:
   - Precompute John positions J[0..N] and Bessie positions B[0..M].
   - Run 2D DP over i=0..N, j=0..M with the three transitions above.
   - Answer is dp[N][M]. Time: O(N*M).
*/

static inline ll sq(ll x) { return x * x; }
static inline ll dist2(const pi& a, const pi& b) {
	return sq((ll)a.fi - b.fi) + sq((ll)a.se - b.se);
}

static inline int dirId(char c) {
	if (c == 'N') return 0;
	if (c == 'E') return 1;
	if (c == 'S') return 2;
	return 3; // 'W'
}

inline void solve() {
	int n, m;
	cin >> n >> m;

	pi j0, b0;
	cin >> j0.fi >> j0.se;
	cin >> b0.fi >> b0.se;

	str js, bs;
	cin >> js >> bs;

	const int dx[]{0, 1, 0, -1}; // N E S W
	const int dy[]{1, 0, -1, 0};

	vpi jp(n + 1), bp(m + 1);
	jp[0] = j0;
	bp[0] = b0;

	for (int i = 0; i < n; i++) {
		int d = dirId(js[i]);
		jp[i + 1] = {jp[i].fi + dx[d], jp[i].se + dy[d]};
	}
	for (int i = 0; i < m; i++) {
		int d = dirId(bs[i]);
		bp[i + 1] = {bp[i].fi + dx[d], bp[i].se + dy[d]};
	}

	V<vl> dp(n + 1, vl(m + 1, BIG));
	dp[0][0] = 0;

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= m; j++) {
			if (dp[i][j] == BIG) continue;

			if (i + 1 <= n) {
				dp[i + 1][j] = min(dp[i + 1][j], dp[i][j] + dist2(jp[i + 1], bp[j]));
			}
			if (j + 1 <= m) {
				dp[i][j + 1] = min(dp[i][j + 1], dp[i][j] + dist2(jp[i], bp[j + 1]));
			}
			if (i + 1 <= n && j + 1 <= m) {
				dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i][j] + dist2(jp[i + 1], bp[j + 1]));
			}
		}
	}

	cout << dp[n][m] << "\n";
}

int main() {
	setIO("radio");
	solve(); // single case
}
