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
     Each cow i starts with gift i. A reassignment is valid if every cow receives a gift she
     likes at least as much as gift i, and gifts can only move within the same breed group
     (H cows swap among H, G among G). For many breed strings, count valid reassignments.

 (2) Things to notice and step-by-step derivation:
     - For a fixed subset S of cows, we only care how many permutations of gifts within S
       are acceptable (each cow gets one gift from S that she accepts).
     - Count acceptable permutations of S by building cycles and forcing each cycle to be
       "closed" exactly when we reach its maximum-indexed cow, which avoids double counting.
     - Use DP over subsets S and the last cow j in the current open chain; we can extend the
       chain to a new cow k if cow k accepts gift j. If k is the max cow in S, we close a cycle.

 (3) Solution outline:
     - Read preferences and build acceptMask[g]: bitmask of cows that would accept gift g.
     - DP: dp[S][j] = #ways where current open chain uses exactly cows in S and ends at cow j.
       Let m = max index in S. We may extend to k < m not in S with (k accepts gift j).
       If m accepts gift j, we close the cycle and add to ans[S]; then we may start a new cycle
       by choosing any new anchor h > m not in S (dp[S∪{h}][h] += dp[S][j]).
     - For each query, split cows into H and G masks and output ans[H] * ans[G].
     - Time: O(2^n * n^2 + q * n), Memory: O(2^n * n)
 */

inline void solve() {
	int n; cin >> n;

	V<vi> rankPos(n, vi(n));
	for (int i = 0; i < n; i++) {
		for (int pos = 0; pos < n; pos++) {
			int g; cin >> g; --g;
			rankPos[i][g] = pos;
		}
	}

	vi acceptMask(n, 0);
	for (int g = 0; g < n; g++) {
		int mask = 0;
		for (int cow = 0; cow < n; cow++) {
			if (rankPos[cow][g] <= rankPos[cow][cow]) mask |= (1 << cow);
		}
		acceptMask[g] = mask;
	}

	int lim = 1 << n;
	int full = lim - 1;

	V<AR<ll, 18>> dp(lim);
	vl ans(lim, 0);

	for (int i = 0; i < n; i++) dp[1 << i][i] = 1;

	for (int S = 1; S < lim; S++) {
		int m = 31 - __builtin_clz(S); // max index in S

		for (int j = 0; j < n; j++) {
			ll ways = dp[S][j];
			if (!ways) continue;

			// extend chain: pick a new k < m not in S that accepts gift j
			for (int k = 0; k < m; k++) {
				if ((S & (1 << k)) == 0 && (acceptMask[j] & (1 << k))) {
					dp[S | (1 << k)][k] += ways;
				}
			}

			// close cycle at m if m accepts gift j, then optionally start a new cycle
			if (acceptMask[j] & (1 << m)) {
				ans[S] += ways;
				for (int h = m + 1; h < n; h++) {
					if ((S & (1 << h)) == 0) dp[S | (1 << h)][h] += ways;
				}
			}
		}
	}

	ans[0] = 1;

	int q; cin >> q;
	while (q--) {
		str st; cin >> st;
		int H = 0;
		for (int i = 0; i < n; i++) if (st[i] == 'H') H |= (1 << i);
		int G = full ^ H;
		cout << ans[H] * ans[G] << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
