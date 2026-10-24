// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using db = long double; // Change to double if TL is tight (almost never).
template<class T> using V = vector<T>;
template<class T1, class T2> using P = pair<T1, T2>;
#define bg(x) begin(x)
#define all(x) bg(x), end(x)
#define rall(x) (x).rbegin(), (x).rend()
#define sz(x) (int)(x).size()
#define rsz resize
#define ft front()
#define bk back()
#define pb push_back
#define pf push_front
#define ppb pop_back
#define ppf pop_front
#define pp pop
#define ins insert
#define fi first
#define se second
using vi = V<int>;
using vl = V<ll>;
using pi = P<int,int>;
using pl = P<ll,ll>;

inline namespace FileIO {
    void setIn(string s){freopen(s.c_str(),"r",stdin);}
    void setOut(string s){freopen(s.c_str(),"w",stdout);}
    void setIO(string s = ""){
        cin.tie(0)->sync_with_stdio(0);
        cin.exceptions(cin.failbit); // Comment out if it breaks things.
        if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
    }
}

const db PI = acos((db)-1);
const int MOD = (int)1e9+7;
const int MX = (int)2e5+5;

/*
 (1) Problem recap in simple terms:
     - We are given N intervals and an integer K.
     - For every subset of intervals, define "complexity" = number of connected components in the union.
     - We want the sum over all subsets of (complexity^K) modulo MOD.

 (2) Things to notice and step-by-step derivation:
     - A classic DP solution processes intervals sorted by left endpoint.
     - We compress all right endpoints and use a segment tree over these compressed positions.
     - Each leaf stores DP information for collections whose rightmost interval ends there.
     - Originally, each DP state stored sums of a_c^j (powers), and "increment complexity by 1" required a
       binomial transform in O(K^2) per operation.
     - Instead, we store sums of falling factorials (a_c)_j, where (a)_j = a * (a-1) * ... * (a-j+1).
     - The key identity is: (a+1)_j = (a)_j + j * (a)_{j-1}, so increasing complexity by 1 can be done in O(K).
     - At the end, we convert back to powers using Stirling numbers of the second kind:
       a^K = sum_{j=0..K} S(K, j) * (a)_j.

 (3) Solution outline:
     - Coordinate-compress all right endpoints (plus 0).
     - Maintain a segment tree of vectors T[0..K], where T[j] = sum over collections of (complexity)_j.
     - Operations:
         * upd(pos, val): point-add a vector val at position pos.
         * mul(pos): multiply all DP states for positions >= pos by 2 (choosing or not choosing future intervals).
         * query(lo, hi, acc): accumulate DP vectors over a range.
     - For each interval [a, b] in order of increasing a:
         * A = sum over rightmost < a  (query prefix), then apply "complexity ++" using falling factorial identity.
         * B = sum over rightmost in [a, b) (middle range).
         * A += B, then upd at compressed index of b.
         * mul at positions strictly after b to account for choices on independent intervals.
     - After all intervals, query the entire range for res[j] = sum (complexity)_j.
     - Precompute Stirling numbers S(K, j), and compute answer = sum_j S(K, j) * res[j].
*/

// Simple modular integer type.
struct mi {
    int v;
    mi(ll _v = 0) {
        _v %= MOD;
        if (_v < 0) _v += MOD;
        v = (int)_v;
    }
    explicit operator int() const { return v; }
    
};

mi operator+(mi a, const mi& b) {
    a.v += b.v;
    if (a.v >= MOD) a.v -= MOD;
    return a;
}

mi operator-(mi a, const mi& b) {
    a.v -= b.v;
    if (a.v < 0) a.v += MOD;
    return a;
}

mi operator*(mi a, const mi& b) {
    a.v = (int)((ll)a.v * b.v % MOD);
    return a;
}

// Type alias for DP vector T[0..K].
using T = array<mi, 11>;

// Segment tree size (enough for constraints).
const int SZ = 1 << 18;

T seg[SZ];
mi lazy[SZ];        // Range multiply factor.
ll S2[11][11];     // Stirling numbers of the 2nd kind, S2[n][k].

int N, K;
V<pi> v;            // Intervals (l, r).
vi y;               // Compressed right endpoints (plus 0).

/*
 push:
 - Apply lazy multiplier to seg[ind].
 - Push down to children if not a leaf.
*/
void push(int ind, int L, int R) {
    if (lazy[ind].v == 1) return;
    for (int i = 0; i <= K; ++i) {
        seg[ind][i] = seg[ind][i] * lazy[ind];
    }
    if (L != R) {
        lazy[2 * ind] = lazy[2 * ind] * lazy[ind];
        lazy[2 * ind + 1] = lazy[2 * ind + 1] * lazy[ind];
    }
    lazy[ind] = mi(1);
}

/*
 mul:
 - Multiply all DP vectors in range [pos, R] by 2.
 - This models the fact that every existing collection can either take or skip new independent intervals.
*/
void mul(int pos, int ind, int L, int R) {
    push(ind, L, R);
    if (pos > R) return;
    if (pos <= L) {
        lazy[ind] = lazy[ind] * mi(2);
        push(ind, L, R);
        return;
    }
    int M = (L + R) / 2;
    mul(pos, 2 * ind, L, M);
    mul(pos, 2 * ind + 1, M + 1, R);
    for (int i = 0; i <= K; ++i) {
        seg[ind][i] = seg[2 * ind][i] + seg[2 * ind + 1][i];
    }
}

/*
 upd:
 - Point-add a vector val at position pos.
 - We add val to every node on the path that covers pos.
*/
void upd(int pos, const T& val, int ind, int L, int R) {
    push(ind, L, R);
    if (pos < L || pos > R) return;
    for (int i = 0; i <= K; ++i) {
        seg[ind][i] = seg[ind][i] + val[i];
    }
    if (L == R) return;
    int M = (L + R) / 2;
    if (pos <= M) upd(pos, val, 2 * ind, L, M);
    else upd(pos, val, 2 * ind + 1, M + 1, R);
}

/*
 query:
 - Add all DP vectors in [lo, hi] into accumulator t.
*/
void query(int lo, int hi, T& t, int ind, int L, int R) {
    push(ind, L, R);
    if (hi < L || lo > R) return;
    if (lo <= L && R <= hi) {
        for (int i = 0; i <= K; ++i) {
            t[i] = t[i] + seg[ind][i];
        }
        return;
    }
    int M = (L + R) / 2;
    query(lo, hi, t, 2 * ind, L, M);
    query(lo, hi, t, 2 * ind + 1, M + 1, R);
}

/*
 advance_complexity:
 - We store F[j] = sum (a_c)_j over collections.
 - When complexity increases by 1 (a_c -> a_c + 1), we want:
     (a+1)_j = (a)_j + j * (a)_{j-1}.
 - So we can transform the whole vector in O(K) time.
*/
void advance_complexity(T& A) {
    for (int j = K; j >= 1; --j) {
        A[j] = A[j] + mi(j) * A[j - 1];
    }
    // A[0] stays the same.
}

/*
 ad(a, b):
 - Handle an interval [a, b] in the DP.
 - i1: last index with right endpoint < a.
 - i2: index of right endpoint == b.
 Steps:
   1) A = sum over positions [0, i1], then increase complexity by 1.
   2) B = sum over positions [i1+1, i2], complexity unchanged.
   3) A += B.
   4) Place A at position i2.
   5) Multiply all positions > i2 by 2 to account for choices on independent intervals.
*/
void ad(int a, int b) {
    int i1 = (int)(lower_bound(all(y), a) - y.begin()) - 1;
    int i2 = (int)(lower_bound(all(y), b) - y.begin());

    T A = T();
    if (i1 >= 0) {
        query(0, i1, A, 1, 0, N);
    }
    advance_complexity(A);

    T B = T();
    if (i1 + 1 <= i2) {
        query(i1 + 1, i2, B, 1, 0, N);
    }

    for (int j = 0; j <= K; ++j) {
        A[j] = A[j] + B[j];
    }

    upd(i2, A, 1, 0, N);
    if (i2 + 1 <= N) {
        mul(i2 + 1, 1, 0, N);
    }
}

/*
 Precompute Stirling numbers of the second kind S2[n][k] for 0 <= n,k <= K.
 Recurrence:
   S2[0][0] = 1
   S2[n][k] = S2[n-1][k-1] + k * S2[n-1][k]
*/
void prec_stirling() {
    for (int n = 0; n <= K; ++n) {
        for (int k = 0; k <= K; ++k) {
            S2[n][k] = 0;
        }
    }
    S2[0][0] = 1;
    for (int n = 1; n <= K; ++n) {
        for (int k = 1; k <= n; ++k) {
            S2[n][k] = (S2[n-1][k-1] + (ll)k * S2[n-1][k]) % MOD;
        }
    }
}

inline void solve() {
    // Read input.
    cin >> N >> K;
    v.rsz(N);
    y.clear();
    y.pb(0); // Include 0 for prefix handling.

    for (int i = 0; i < N; ++i) {
        cin >> v[i].fi >> v[i].se;
        y.pb(v[i].se);
    }

    sort(all(v));
    sort(all(y));

    // Initialize lazy with multiplicative identity.
    for (int i = 1; i < SZ; ++i) {
        lazy[i] = mi(1);
    }

    prec_stirling();

    // Initial DP: empty set with complexity 0 gives (0)_0 = 1.
    T ori = T();
    ori[0] = mi(1);
    upd(0, ori, 1, 0, N);

    // Process intervals in order of left endpoint.
    for (int i = 0; i < N; ++i) {
        ad(v[i].fi, v[i].se);
    }

    // Aggregate all DP states.
    T res = T();
    query(0, N, res, 1, 0, N);

    // Convert from falling factorials to powers using Stirling numbers:
    // answer = sum_j S(K, j) * res[j].
    mi ans = mi(0);
    for (int j = 0; j <= K; ++j) {
        if (S2[K][j] == 0) continue;
        ans = ans + res[j] * mi(S2[K][j]);
    }

    cout << ans.v << '\n';
}

int main() {
    setIO("help");
    solve(); // single case
}