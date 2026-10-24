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
 (1) Problem recap (plain):
     We are given the final edited genome S (length up to 1e5), consisting of A/G/C/T and '?'.
     The original genome was transformed by:
       - split between every pair of equal consecutive letters,
       - reverse each resulting block,
       - concatenate the blocks.
     Count how many original genomes could lead to S (mod 1e9+7).

 (2) Things to notice & step-by-step derivation:
     • After the transformation, adjacent equal letters in S mark boundaries between reversed blocks.
     • A valid block cannot contain equal adjacent letters (otherwise a split would have happened there).
     • When we reconstruct left-to-right, think of S as being partitioned into those reversed blocks.
       The last two blocks must "touch" with matching boundary letters (the split happened there).
     • To avoid O(N^2), track only the minimal context needed to extend or start a new block.

 (3) Solution outline:
     • Let letters be indexed as 0..3 for A,G,C,T. Maintain DP state dp[n][l][m]:
         - n: first letter of the previous block (block before the last),
         - l: first letter of the current (last) block,
         - m: last letter of the current (last) block (i.e., the last character placed).
       This corresponds to the editorial's dp_{k, n, l, m} after processing k+1 characters.
     • Transitions for next character x (index m2):
         (a) Extend current block if m2 != m (no equal-adjacent inside a block):
             dp'[n][l][m2] += dp[n][l][m]
         (b) Start a new 1-length block (so its first=last=m2) if the boundary matches:
             Allowed iff n == m (previous block ended with same letter as the split).
             Then dp'[l][m2][m2] += dp[n][l][m]
     • Initialize for position 0: for any letter l matching S[0], dp[n][l][l] = 1 for all n.
     • Answer after processing all positions: sum over n==m states, i.e., sum dp[m][l][m] for all l,m.
     • Time O(4^4 * N) ≈ O(N), memory O(1) with rolling arrays.
*/

inline void solve() {
    str s; cin >> s;

    // dp[n][l][m] as described above.
    static ll dp[4][4][4]{}, nxt[4][4][4]{}; // zero-initialized

    // Map base to index in order "AGCT"
    auto baseIdx = [](char c) {
        if (c == 'A') return 0;
        if (c == 'G') return 1;
        if (c == 'C') return 2;
        if (c == 'T') return 3;
        return -1; // for '?'
    };

    // First character initialization:
    // For any allowed letter l at position 0, the last block is length 1 (first=last=l).
    // For all n (unknown previous block's first letter), dp[n][l][l] = 1.
    for (int l = 0; l < 4; ++l) {
        if (s[0] == '?' || baseIdx(s[0]) == l) {
            for (int n = 0; n < 4; ++n) dp[n][l][l] = 1;
        }
    }

    // Process S[1..]
    for (int k = 1; k < sz(s); ++k) {
        memset(nxt, 0, sizeof(nxt));
        for (int m2 = 0; m2 < 4; ++m2) {
            if (!(s[k] == '?' || baseIdx(s[k]) == m2)) continue; // letter m2 not allowed here
            for (int n = 0; n < 4; ++n) {
                for (int l = 0; l < 4; ++l) {
                    for (int m = 0; m < 4; ++m) {
                        ll ways = dp[n][l][m];
                        if (!ways) continue;

                        // (a) Extend current block: next char must differ from current last (no split inside a block).
                        if (m != m2) {
                            ll &ref = nxt[n][l][m2];
                            ref += ways;
                            if (ref >= MOD) ref -= MOD;
                        }

                        // (b) Start a new block of length 1 at this position:
                        // The split between blocks exists only if previous block's end equals previous-previous block's start.
                        if (n == m) {
                            ll &ref2 = nxt[l][m2][m2]; // new state: previous block's first becomes 'l'; new block first&last=m2
                            ref2 += ways;
                            if (ref2 >= MOD) ref2 -= MOD;
                        }
                    }
                }
            }
        }
        // roll
        memcpy(dp, nxt, sizeof(dp));
    }

    // Final count: valid partitions must have n == m at the end (boundary condition satisfied).
    ll ans = 0;
    for (int l = 0; l < 4; ++l) for (int m = 0; m < 4; ++m) {
        ans += dp[m][l][m];
        if (ans >= MOD) ans -= MOD;
    }

    cout << ans % MOD << '\n';
}

int main() {
    setIO();
    solve(); // single case
}