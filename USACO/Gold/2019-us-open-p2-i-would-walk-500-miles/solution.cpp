// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll  = long long;
using i128 = __int128_t;
using db  = long double;

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
#define pb  push_back
#define eb  emplace_back
#define ft  front()
#define bk  back()
using vi  = V<int>;
using vb  = V<bool>;
using vl  = V<ll>;
using vd  = V<db>;
using vs  = V<str>;
using vpi = V<pi>;
using vpl = V<pl>;
using vpd = V<pd>;

// misc. ops & aliases
#define pf  push_front
#define ppb pop_back
#define ppf pop_front
#define lb  lower_bound
#define ub  upper_bound
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
template<class T> using pq  = priority_queue<T>;
template<class T> using pqg = priority_queue<T, V<T>, greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s){ freopen(s.c_str(), "r", stdin); }
	void setOut(string s){ freopen(s.c_str(), "w", stdout); }
	void setIO(string s = ""){
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit);
		if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
	}
}

// constants (add more if needed)
const db PI   = acos((db)-1);
const ll BIG  = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
 (1) Problem recap in simple terms:
     - There are N cows labeled 1..N.
     - For cows x < y, the “distance” between them is
           w(x, y) = (2019201913 * x + 2019201949 * y) mod 2019201997.
     - We must split cows into K non-empty groups. Let M be the minimum w(x, y)
       over all pairs (x, y) that lie in different groups. We want to maximize M.

 (2) Things to notice and step-by-step derivation:
     - First simplify w(x, y) using modular arithmetic.

       Let MOD0 = 2019201997.
       Note:
         2019201913 = MOD0 - 84  ⇒ 2019201913 ≡ -84 (mod MOD0)
         2019201949 = MOD0 - 48  ⇒ 2019201949 ≡ -48 (mod MOD0)

       So
         w(x,y) = (2019201913 x + 2019201949 y) mod MOD0
                 ≡ (-84x - 48y) mod MOD0.

       For 1 ≤ x,y ≤ N ≤ 7500, we have
         84x + 48y ≤ 84N + 48N = 132N ≤ 132 * 7500 < 1e6 << MOD0.
       So -84x - 48y is negative with absolute value < MOD0, meaning its
       unique residue modulo MOD0 is:
         w(x,y) = MOD0 - (84x + 48y)
                = 2019201997 - 84x - 48y.

       Thus maximizing w(x,y) is equivalent to minimizing (84x + 48y)
       over cross-group pairs.

     - Now think about grouping:
       * 84x + 48y is strictly increasing in both x and y.
       * Small indices (x,y small) give small 84x + 48y?  No:
           84x + 48y is smaller when x,y are small and larger when x,y are large.
         But w(x,y) = MOD0 - (84x + 48y), so:
           - Large x,y ⇒ large (84x + 48y) ⇒ smaller w(x,y).
           - Small x,y ⇒ small (84x + 48y) ⇒ larger w(x,y).
       * Since we want to maximize the MINIMUM w(x,y) between different groups,
         we want to avoid extremely small w(x,y) across groups.
         That means we should prevent very large x,y from being in different groups,
         because that would make w(x,y) very small.

       Greedy picture:
       - Put as many of the largest indices together in one big group, so edges
         among them never count as “between groups”.
       - Split off the K-1 smallest indices each into their own groups.
         Then we have:
           - Groups: {1}, {2}, ..., {K-1}, {K, K+1, ..., N}.

       Now, among all cross-group pairs, which one gives the *minimum* w(x,y)?
       - We want to minimize w(x,y) = 2019201997 - 84x - 48y,
         i.e. maximize 84x + 48y subject to x,y in different groups.
       - The largest y we can choose is y = N (the largest cow, in the big group).
       - To keep x in a different group from y, x must be one of 1..K-1. Among
         those, the largest is x = K-1.
       So the worst cross-group pair is (x, y) = (K-1, N).

       Therefore, for this optimal grouping, the minimum cross-group distance M is:
         M = w(K-1, N)
           = 2019201997 - 84 * (K-1) - 48 * N.

       The math/editorial argues this grouping is optimal (it matches what you’d
       get from Kruskal/Prim on the full graph), so this single formula is enough.

 (3) Solution outline:
     - Read N and K.
     - Directly compute
           answer = 2019201997 - 84 * (K - 1) - 48 * N
       using 64-bit integers (ll).
     - Print the answer.
*/

inline void solve() {
	int n, k;
	cin >> n >> k;

	// Use ll for safety even though int would suffice numerically.
	ll ans = 2019201997LL - 84LL * (k - 1LL) - 48LL * n;
	cout << ans << '\n';
}

int main() {
	setIO("walk");
	solve(); // single case
}