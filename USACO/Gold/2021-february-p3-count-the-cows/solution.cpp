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
 (1) Problem recap (simple):
     For each query (d, x, y), count the number of integers k with 0 ≤ k ≤ d such that,
     in base-3 addition (with carries), the i-th ternary digit of (x + k) and (y + k)
     have the same parity (even/odd) for every digit position i.

 (2) Things to notice & derivation:
     • Work digit-by-digit in base 3. For a digit i, the digit of (x + k) is
       (xi + carry_x + ki) mod 3, with next carry floor((xi + carry_x + ki)/3);
       similarly for y. The parity constraint at digit i only depends on these local values.
     • To enforce 0 ≤ k ≤ d, track the comparison between the already-formed lower
       (i+1) digits of k and d using cmp ∈ {0:<, 1:=, 2:>}. When processing digit i:
           - if ki > di then the (i+1)-digit number of k is > d (cmp := 2),
           - if ki < di then it's < d (cmp := 0),
           - else keep the previous cmp (digits below still decide).
     • Iterate from least significant digit to most; the rule above remains valid because
       the new digit at position i dominates all lower digits in base-3 weight.

 (3) Solution outline:
     DP over digits i = 0..38. State dp[cmp][cx][cy] = number of ways to choose digits
     k0..k_{i-1} so far such that:
       - cmp is the relation between k (mod 3^i) and d (mod 3^i),
       - cx, cy are the carries into digit i for x+k and y+k,
       - parity constraint has held for all processed digits.
     Transition by trying ki ∈ {0,1,2}, computing the next digits/carries for x+k and y+k
     and updating cmp via the rule above. Keep states only when the two produced digits
     have equal parity. After all 39 digits, sum states with no leftover carry and cmp∈{<,=}.
     Complexity per query is O(39 * 3 * 2 * 2 * 3), which is tiny.
*/

inline void solve() {
    vl po3{1}; po3.reserve(40);
    for (int i = 1; i < 40; i++)
        po3.pb(3 * po3.bk); // powers of 3 for extracting ternary digits

    AR<AR<AR<ll,2>,3>,3> dp, ndp; // dp[cmp (0:<,1:=,2:>)][carry_x in {0,1}][carry_y in {0,1}]
    int q; cin >> q;
    while (q--) {
        ll d, x, y;
        cin >> d >> x >> y;

        dp = {}; dp[1][0][0] = 1; // start with empty (0 digits): k == d on 0 digits, no carries

        for (int i = 0; i < 39; i++) { // process digit i (LSB -> MSB)
            ndp = {};
            int dd = d / po3[i] % 3, xd = x / po3[i] % 3, yd = y / po3[i] % 3; // current base-3 digits

            // iterate over previous comparison state + carries, and choose ki ∈ {0,1,2}
            for (int cmp = 0; cmp < 3; cmp++) for (int xc = 0; xc < 2; xc++) for (int yc = 0; yc < 2; yc++) for (int j = 0; j < 3; j++) {
                // add ki=j to xi,yi with carries, compute produced digit and next carry in base 3
                int nxd = (xd + xc + j) % 3, nxc = (xd + xc + j) / 3;
				int nyd = (yd + yc + j) % 3, nyc = (yd + yc + j) / 3;

                // update comparison for the lower (i+1) digits of k vs d:
                // if j > dd -> definitely greater; if j < dd -> definitely less; else keep previous cmp
                int ncmp = cmp;
                if (j > dd) ncmp = 2;
				if (j < dd) ncmp = 0;

                // parity constraint at this digit: digits of (x+k) and (y+k) must have same parity
                if ((nxd & 1) == (nyd & 1))
                    ndp[ncmp][nxc][nyc] += dp[cmp][xc][yc];
            }
            swap(dp, ndp); // move to next digit
        }

        // valid k must satisfy k ≤ d  ⇔ cmp ∈ {0:<, 1:=}, and no leftover carry into the 40th digit
        cout << dp[0][0][0] + dp[1][0][0] << '\n';
    }
}

int main() {
    setIO();
    solve(); // single case
}