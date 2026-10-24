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
     We have upper bounds H[i] for N cows' hunger levels h[i]. One move picks adjacent cows
     i and i+1 and decreases both by 1. Count how many h arrays (0 <= h[i] <= H[i]) can be
     reduced so that all cows end at the same hunger value, modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - If N is even, reaching all-equal value x is equivalent to reaching all zeros (shift by x).
     - To check reachability to all zeros, we can greedily eliminate left to right; this turns into
       a DP over the "leftover" value at the current position.
     - Let dp[t] be the number of ways the leftover equals t after processing a prefix.
       For cow i with cap H, dpNew[j] = sum_{x=0..H-j} dpOld[x].
     - That sum is computed fast by reversing dp[0..H], zeroing dp[H+1..], then taking prefix sums.

 (3) Solution outline:
     - Read N and H[]. Let mn = min(H), mx = max(H).
     - Run the DP once if N is even; answer is dp[0].
     - If N is odd, sum dp[0] over all targets x by repeatedly decrementing all H[i] (shift down).
     - Time: O(N*mx) if N is even, else O(N*mx*mn); Memory: O(mx).
 */

static inline int addMod(int a, int b) {
	int res = a + b;
	if (res >= MOD) res -= MOD;
	return res;
}

inline void solve() {
	int n;
	cin >> n;
	vi h(n);
	for (int i = 0; i < n; i++) cin >> h[i];

	int mn = *min_element(all(h));
	int mx = *max_element(all(h));

	vi dp(mx + 1);
	int ans = 0;

	do {
		fill(bg(dp), bg(dp) + mx + 1, 1);

		for (int i = 0; i < n; i++) {
			// reverse dp[0..h[i]] so "sum_{x=0..h[i]-j}" becomes a prefix sum after reversal
			reverse(bg(dp), bg(dp) + h[i] + 1);

			// states > h[i] are impossible after using this cow
			fill(bg(dp) + h[i] + 1, bg(dp) + mx + 1, 0);

			// prefix sums with mod addition
			partial_sum(bg(dp), bg(dp) + mx + 1, bg(dp), addMod);
		}

		ans = addMod(ans, dp[0]);

		for (int i = 0; i < n; i++) h[i]--;
	} while (n % 2 && mn-- && mx--);

	cout << ans << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
