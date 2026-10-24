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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T, V<T>, greater<T>>;

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
     We have a string of length n using an alphabet of size m. We can change each character into another with some cost.
     We must split the final string into blocks of length at least k where each block is all the same letter, and we want
     the minimum total cost of all character changes.

 (2) Things to notice and step-by-step derivation:
     - The given cost matrix between letters is not necessarily optimal for indirect paths, so we first run Floyd–Warshall
       to get the true minimum change cost between every pair of letters.
     - For each position i and target letter x, we can precompute cst[i][x] = cost to change s[i] into x, and build prefix
       sums ps so that any segment’s cost to be all x is a quick difference of prefix sums.
     - DP idea: dp[i][x] = min cost to transform the prefix [1..i] if position i ends in a block of letter x; blocks must
       have length at least k, so we either extend a previous x-block by one or start a new block of length exactly k.

 (3) Solution outline:
     - Run Floyd–Warshall over the m×m cost matrix to get all-pairs minimum change costs.
     - Precompute cst[i][x] and prefix sums ps[i][x] for 1 ≤ i ≤ n, 0 ≤ x < m.
     - Use DP over positions and letters:
         • Option 1: extend previous block of x: dp[i][x] = dp[i-1][x] + cst[i][x].
         • Option 2 (if i ≥ k): start a new length-k block of x ending at i, cost = (ps[i][x] - ps[i-k][x]) + mn[i-k],
           where mn[t] = min_x dp[t][x]. Maintain mn[i] as we go.
     - Answer is mn[n], the minimum cost over all letters at position n.
*/

const int MAXN = 100000 + 5;
const int MAXM = 26;

// problem-specific globals (large arrays kept global for memory/performance)
int d[MAXM][MAXM];          // min change cost between letters after Floyd–Warshall
int cst[MAXN][MAXM];        // cost to change position i's original letter to letter j
int ps[MAXN][MAXM];         // prefix sums of cst along the string
int dp[MAXN][MAXM];         // dp[i][j]: best cost for prefix [1..i] if position i is letter j
int mn[MAXN];               // mn[i] = min over j of dp[i][j]

inline void solve() {
    int n, m, k;
    str s;
    cin >> n >> m >> k >> s;

    // read initial cost matrix
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            cin >> d[i][j];
        }
    }

    // Floyd–Warshall to compute all-pairs minimum letter-change costs
    for (int mid = 0; mid < m; mid++) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < m; j++) {
                d[i][j] = min(d[i][j], d[i][mid] + d[mid][j]);
            }
        }
    }

    // precompute cst[i][j] and prefix sums ps[i][j]
    // positions are 1-indexed in the DP; s is 0-indexed
    for (int i = 1; i <= n; i++) {
        int orig = s[i - 1] - 'a'; // original letter index at position i
        for (int j = 0; j < m; j++) {
            cst[i][j] = d[orig][j];              // cost to change this position to letter j
            ps[i][j] = ps[i - 1][j] + cst[i][j]; // build prefix sum for letter j
        }
    }

    // initialize DP with large values
    memset(dp, 0x3f, sizeof(dp));
    memset(mn, 0x3f, sizeof(mn));
    mn[0] = 0; // base: zero cost before the string

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < m; j++) {
            // option 1: extend existing block of letter j
            dp[i][j] = min(dp[i][j], dp[i - 1][j] + cst[i][j]);

            // option 2: if we have at least k characters, start a new block of length exactly k ending at i
            if (i >= k) {
                int segCost = ps[i][j] - ps[i - k][j]; // cost to make [i-k+1..i] all letter j
                dp[i][j] = min(dp[i][j], segCost + mn[i - k]);
            }

            // maintain the minimum cost over all letters for prefix [1..i]
            mn[i] = min(mn[i], dp[i][j]);
        }
    }

    cout << mn[n] << '\n';
}

int main() {
    setIO("cowmbat");
    solve(); // single case
}
