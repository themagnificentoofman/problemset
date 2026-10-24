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
const int MXN = (int)1e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
    - We are given a permutation A[0..n-1] of {0,1,...,n-1}.
    - For each j in [0, n-1], we must output the number of pairs (x, y)
      such that x < y, A[x] > A[y], and A[y] < j.
      In other words: count inversions whose right value is < j.

 (2) Things to notice and step-by-step derivation:
    - For each position y, define:
        n[y] = number of x < y with A[x] > A[y].
      This is exactly the number of inversions where y is the right endpoint.
    - The answer for each j is:
        ans[j] = sum over all y with A[y] < j of n[y].
    - Fix a value h. For all positions y with A[y] = h:
        * n[y] depends only on elements with value > h to the left of y.
        * Once we know n[y], it should contribute to every ans[j] where j > h
          (because condition A[y] < j is the same as j > h).
    - Instead of:
        - computing all n[y],
        - then summing over all y for every j,
      we can push contributions directly into the correct "j-buckets":
        * For a position y with A[y] = h, add n[y] to ans[h+1].
        * Later, taking prefix sums over j makes ans[j] accumulate
          all n[y] with A[y] < j.
    - How to compute n[y] efficiently:
        * Process values h from largest to smallest.
        * Maintain an ordered set S of indices of positions with A[index] > h.
        * When processing positions y with A[y] = h:
            n[y] = # of indices in S that are < y
                  = S.order_of_key(y).

 (3) Solution outline:
    - Group positions by value: todo[v] = list of indices y with A[y] = v.
    - Use a policy-based tree (ordered set) S of indices to support
      order_of_key (count of elements < x) in O(log n).
    - Loop h from n down to 0:
        * For each index y in todo[h]:
            - numInv[h+1] += S.order_of_key(y)  (add n[y] to bucket h+1).
        * Then insert all these indices y into S (they become "larger values"
          for smaller h).
    - Take prefix sums of numInv[] so that each numInv[j] becomes:
        ans[j] = sum_{A[y] < j} n[y].
    - Output numInv[0..n-1] as required by the problem.
*/

#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
template<class T> using indexed_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>; 

ll numInv[MXN];
vi todo[MXN];

inline void solve() {
    int n; cin >> n;
    vi a(n);
    for (int& x : a) cin >> x;

    // todo[v] = all positions i where A[i] = v.
    for (int i = 0; i < n; ++i) todo[a[i]].pb(i);

    indexed_set<int> s; // stores indices of positions with value > current h

    // Process hair lengths h from n down to 0.
    // For a given h, S contains all indices with A[index] > h.
    for (int i = n; ~i; --i) {
        /*
         For each position x with A[x] = i:
         - s.order_of_key(x) = number of indices in s that are < x
                              = number of positions y < x with A[y] > i
                              = n[x] (inversion count with right endpoint x).
         - Each such n[x] contributes to all answers ans[j] with j > i,
           so we add it to numInv[i+1] and later prefix-sum.
        */
        for (int x : todo[i]) numInv[i+1] += s.order_of_key(x);

        // After counting contributions, insert these indices into s
        // so they are treated as "larger values" when we move to smaller i.
        for (int x : todo[i]) s.ins(x);
    }

    // Prefix sums: now numInv[j] = sum_{k <= j} numInv[k] (old),
    // which becomes ans[j] = sum_{A[y] < j} n[y].
    for (int i = 1; i < n; ++i) numInv[i] += numInv[i-1];

    // Output answers for j = 0..n-1.
    // For j = 0, answer is 0 since there is no A[y] < 0.
    for (int i = 0; i < n; ++i) cout << numInv[i] << '\n';
}

int main() {
    setIO("haircut");
    solve(); // single case
}