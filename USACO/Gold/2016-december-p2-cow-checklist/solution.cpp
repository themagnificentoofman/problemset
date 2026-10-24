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
   We must visit all Holsteins in order (H1..HH). We may also visit Guernseys in order (G1..GG),
   interleaving them however we want. Start at H1 and end at HH. Moving cost is squared distance.

2) Things to notice + step-by-step:
   - The only thing that matters is how many from each list we’ve visited, and which type we’re standing on.
   - Let dp[i][j][t] be the minimum energy after visiting first i Holsteins and first j Guernseys,
     ending at t=0 (Holstein i) or t=1 (Guernsey j).
   - Transitions add one more cow from the appropriate list, plus the movement cost from the previous cow.

3) Solution outline:
   - Initialize dp[1][0][0] = 0 (start at H1).
   - Fill dp in increasing i and j using 4 transitions (H->H, G->G, G->H, H->G).
   - Answer is dp[H][G][0] (must end at the last Holstein).
   - Time: O(H*G).
*/

static inline ll sqDist(const pl& a, const pl& b) {
	ll dx = a.fi - b.fi;
	ll dy = a.se - b.se;
	return dx * dx + dy * dy;
}

inline void solve() {
	int H, G;
	cin >> H >> G;

	vpl h(H + 1), g(G + 1);
	for (int i = 1; i <= H; i++) cin >> h[i].fi >> h[i].se;
	for (int j = 1; j <= G; j++) cin >> g[j].fi >> g[j].se;

	V<V<AR<ll,2>>> dp(H + 1, V<AR<ll,2>>(G + 1, AR<ll,2>{BIG, BIG}));
	dp[1][0][0] = 0;

	for (int i = 1; i <= H; i++) {
		for (int j = 0; j <= G; j++) {
			// end at Holstein i
			if (dp[i][j][0] < BIG) {
				if (i + 1 <= H) {
					dp[i + 1][j][0] = min(dp[i + 1][j][0], dp[i][j][0] + sqDist(h[i], h[i + 1]));
				}
				if (j + 1 <= G) {
					dp[i][j + 1][1] = min(dp[i][j + 1][1], dp[i][j][0] + sqDist(h[i], g[j + 1]));
				}
			}

			// end at Guernsey j (only valid if j >= 1)
			if (j >= 1 && dp[i][j][1] < BIG) {
				if (j + 1 <= G) {
					dp[i][j + 1][1] = min(dp[i][j + 1][1], dp[i][j][1] + sqDist(g[j], g[j + 1]));
				}
				if (i + 1 <= H) {
					dp[i + 1][j][0] = min(dp[i + 1][j][0], dp[i][j][1] + sqDist(g[j], h[i + 1]));
				}
			}
		}
	}

	cout << dp[H][G][0] << "\n";
}

int main() {
	setIO("checklist");
	solve(); // single case
}
