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
const int MXN = 20; // modify as needed

/*
 (1) Problem recap (simple):
     We can pick any order of the distinct letters (size N ≤ 20). Reading the given string s,
     every time a pair (x -> y) goes from a later letter to an earlier/equal letter in our order,
     the cow starts a new hum. We want the minimum hum count.

 (2) Things to notice + step-by-step derivation:
     - Let oc[a][b] = # of adjacent occurrences of letter a followed by b in s.
       For a fixed order p of the N letters, the total hums equals:
       1 + sum over all pairs where the second letter is not later than the first in p.
     - Classic reduction (from the editorial): define dp[S] for a subset S of letters
       as the minimum hums when we only arrange the letters in S.
       If we decide which letter j in S is placed LAST, the new pairs “between j and all letters in S”
       are exactly counted by ∑_{k∈S} oc[k][j] (column j). So:
         dp[S] = min_{j∈S} ( dp[S \ {j}] + ∑_{k∈S} oc[k][j] ), with dp[∅] = 1.
     - Precompute ∑_{k∈S} oc[k][j] fast for all S and each j:
       split the N letters into two halves (≤10 each) and build two tables stor1 and stor2
       so a column-sum over any subset S can be answered in O(1).

 (3) Solution outline:
     - Compress distinct letters in s to ids [0..N-1].
     - Build oc[a][b].
     - Precompute stor1[mask][j] over the first half and stor2[mask][j] over the second half,
       where each stores ∑_{k in mask} oc[k][j].
     - Bitmask DP over all subsets S: for each j ∈ S,
         dp[S] = min(dp[S], dp[S \ {j}] + column_sum(S, j)),
       where column_sum(S, j) = stor1[S_low][j] + stor2[S_high][j].
     - Answer is dp[(1<<N) - 1].
*/

int stor1[1 << 10][MXN], stor2[1 << 10][MXN]; // precomputed partial sums for column j over two halves of letters.

inline void solve() {
    string s; cin >> s;

    // Map each distinct char to a compact id [0..n-1].
    map<char, int> m;
    for (char c : s) m[c] = 0;
    int cnt = 0;
    for (auto &t : m) t.se = cnt++;
    int n = sz(m);
    assert(n <= MXN);

    // oc[a][b] = # of times s has ... a, b ... consecutively (a -> b).
    V<vi> oc(n, vi(n));
    for (int i = 0; i < sz(s) - 1; i++)
        oc[m[s[i]]][m[s[i + 1]]]++;

    // dp[mask] = minimal hums for arranging letters in 'mask'. Base dp[0] = 1 (the initial hum).
    vi dp(1 << n, MOD);
    dp[0] = 1;

    // Split letters into two halves to precompute sums efficiently.
    int bits = n >> 1; // size of first half (≤10).
    for (int j = 0; j < n; j++) {
        // stor1[i][j] = sum_{k in i (first half)} oc[k][j].  // column j, first half subset.
        for (int i = 0; i < (1 << bits); i++)
            for (int k = 0; k < bits; k++)
                if (i & (1 << k)) stor1[i][j] += oc[k][j];

        // stor2[i][j] = sum_{k in i (second half)} oc[bits + k][j]. // column j, second half subset.
        for (int i = 0; i < (1 << (n - bits)); i++)
            for (int k = 0; k < n - bits; k++)
                if (i & (1 << k)) stor2[i][j] += oc[bits + k][j];
    }

    // Bitmask DP:
    // For each subset i and each j ∈ i, place j LAST:
    // dp[i] = min( dp[i], dp[i ^ (1<<j)] + sum_{k ∈ i} oc[k][j] ).
    for (int i = 0; i < (1 << n); i++)
        for (int j = 0; j < n; j++)
            if (i & (1 << j)) {
                int sum = dp[i ^ (1 << j)]; // previous subset without j.
                // Add column-sum for j over the whole set i = S:
                sum += stor1[i & ((1 << bits) - 1)][j]; // contribution from first half in S.
                sum += stor2[i >> bits][j];             // contribution from second half in S.
                dp[i] = min(dp[i], sum);
            }

    cout << dp[(1 << n) - 1];
}

int main() {
    setIO();
    solve(); // single case
}