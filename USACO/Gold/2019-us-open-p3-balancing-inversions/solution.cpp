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
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
template<class T> using pq = priority_queue<T>;
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
const db PI = acos((db)-1);
const ll BIG = 4e18;          // large enough sentinel for ll
const int MOD = (int)1e9+7;   // 998244353;
const int MXN = (int)2e5+5;   // modify as needed

/*
 (1) Problem recap in simple terms:
     - We have a boolean array A of length 2N (values are 0 or 1).
     - Bessie's score = # of inversions (1 before 0) in the first half.
       Elsie's score = # of inversions in the second half.
     - We can swap adjacent elements anywhere in the array.
       Goal: make the two scores equal with the minimum number of swaps.

 (2) Things to notice and step-by-step derivation:
     - Let inv0 = inversions in the first half, inv1 = inversions in the second half.
       We care about the difference dif = inv0 - inv1.
     - Each time we move a 1 from the left half into the right half (or symmetrically
       move a 0 the other way), dif changes in a predictable way:
         * How far that element moves.
         * How many 1s/0s it passes.
         * How this affects inversions in each half.
     - The editorial’s trick:
         * First, compute inv0 and inv1 for the initial array.
         * Then, for one “type” of element (1s), simulate pulling them from the left
           half into the right half via adjacent swaps, updating both:
             - dif (score difference),
             - cost (number of swaps done so far).
           Maintain a pointer j into the right half to know where we insert them and
           a “displaced” count of pushed-out elements.
         * Repeat the same process but treating 0s as “moving the other way”
           (by flipping the sign in the formulas).
       At each intermediate configuration, the answer candidate is cost + |dif|.

 (3) Solution outline:
     - Read N and the 2N values into an array A; count total number of 1s.
     - Define a helper to count inversions on any subarray [a, b] in O(length).
     - Define a routine tryType(tp, sgn) that:
         * Recomputes inv0, inv1, dif.
         * Iterates from the middle leftwards, pulling elements of value tp
           across the boundary, updating dif and cost using the derived formulas.
         * Moves a pointer j inside the right half to keep the window size N,
           tracking how many elements we “displace”.
         * Updates a global best answer with cost + |dif| whenever valid.
     - Run tryType(1, +1) and tryType(0, -1) and print the minimum best.
*/

inline void solve() {
    int N;
    cin >> N;

    vi A(2 * N);
    int ones = 0;
    for (int i = 0; i < 2 * N; i++) {
        cin >> A[i];
        ones += A[i];
    }

    auto llabs_ll = [&](ll x) -> ll {
        return (x < 0 ? -x : x);
    };

    // Count inversions (1 before 0) in A[a..b].
    auto ctinv = [&](int a, int b) -> ll {
        ll t = 0;
        int n1 = 0;
        for (int i = a; i <= b; i++) {
            if (A[i] == 1) {
                n1++;
            } else {
                t += n1;
            }
        }
        return t;
    };

    ll best = BIG;

    // Simulate pulling elements of value 'tp' across the middle.
    auto tryType = [&](int tp, int sgn) {
        // sgn = +1 when tp = 1, sgn = -1 when tp = 0 (symmetry trick).
        ll inv0 = ctinv(0, N - 1);         // inversions in first half
        ll inv1 = ctinv(N, 2 * N - 1);     // inversions in second half
        ll dif = inv0 - inv1;              // current score difference

        best = min(best, llabs_ll(dif));   // maybe already balanced

        int n1 = 0;        // number of moved tp-elements from left half
        int j = N;         // pointer in the right half
        int displaced = 0; // how many right-half elements were displaced
        ll cost = 0;       // total adjacent swaps used

        // Walk left half from right to left, choosing tp positions to move.
        for (int i = N - 1; i >= 0; i--) {
            if (A[i] != tp) continue;

            // Move this tp element from index i to index N-1 (right edge of left half).
            dif -= (ll)sgn * (N - 1 - i);
            cost += (N - 1 - i);

            // Then across the boundary: it passes (ones - N) 0/1s in total effect.
            dif += (ll)sgn * (ones - N);
            cost += 1;

            // It passes n1 already moved tp-elements.
            dif += (ll)sgn * n1;
            cost += n1;

            // It passes 'displaced' elements we already shoved.
            dif += (ll)sgn * displaced;
            cost += displaced;

            n1++;
            if (n1 + displaced > N) return; // can't fit more in second half

            // Adjust j to keep exactly N elements in the right half segment.
            while (n1 + displaced > j - N) {
                if (A[j] == (tp ^ 1)) {
                    // This element is the opposite type; skip over it.
                    j++;
                } else if (j == 2 * N - 1) {
                    // No more positions to push into; stop this scenario.
                    return;
                } else {
                    // Displace a tp-element in the right half further right.
                    int delta = N + n1 + displaced - j;
                    dif += (ll)sgn * delta;
                    cost += delta;
                    displaced++;
                    j++;
                }
            }

            // Now the configuration is valid; update best.
            best = min(best, cost + llabs_ll(dif));
        }
    };

    // Try treating 1s as the moving type, then 0s (by symmetry).
    tryType(1, +1);
    tryType(0, -1);

    cout << best << '\n';
}

int main() {
    setIO("balance");
    solve(); // single case
}