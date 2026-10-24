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

// value range for this problem: a[i] in [-1e6, 1e6]
const int SHIFT = (int)1e6;

/*
 (1) problem recap in simple terms:
     We are given an array of n integers and q queries [l, r].
     For each query, we must count how many triples l <= i < j < k <= r have a[i] + a[j] + a[k] = 0.

 (2) things to notice and step-by-step derivation:
     n and q are about 2000, so O(n^3) per query is too slow, but about O(n^2) preprocessing is fine.
     For a fixed pair of endpoints (i, j), we only need to know how many middle positions k (i < k < j)
     produce a zero-sum triple. That is a[i] + a[k] + a[j] = 0.
     If we shift all values by a constant so they become non-negative, then the "third value" needed
     to complete a zero-sum triple becomes a single array index in a frequency array, so we can count
     complements in O(1) time while sweeping.

 (3) solution outline:
     - Shift every a[i] by +SHIFT so all values lie in [0, 2*SHIFT].
     - For each i from right to left, maintain a frequency array of values strictly between i and j.
       For each j > i + 1, compute the complement value that would make the triple sum to zero and
       record freq[complement] into ways[i][j].
     - After this sweep, ways[i][j] is the number of zero-sum triples with endpoints exactly at i and j.
       Then we run a 2D prefix-style DP over (i, j) so that ways[l][r] becomes the total number of valid
       triples fully inside the subarray [l, r].
     - Each query [l, r] is then answered in O(1) by printing ways[l-1][r-1].
*/

inline void solve() {
	int n, q;
	cin >> n >> q;

	vi val(n);
	for (int i = 0; i < n; i++) {
		cin >> val[i];
		val[i] += SHIFT; // shift values so all are non-negative and fit into freq indices
	}

	// ways[i][j] = number of triples (i < k < j) such that a[i] + a[k] + a[j] = 0
	V<vl> ways(n, vl(n, 0));

	// freq[x] = how many times value x appears strictly between the current i and j
	vi freq(2 * SHIFT + 1, 0);

	// Precompute ways[i][j] by fixing (i, j) as endpoints of the triple
	for (int i = n - 2; i >= 0; i--) {
		// start with the element right after i as "inside" the interval
		freq[val[i + 1]]++;

		for (int j = i + 2; j < n; j++) {
			// For sum zero: (original) a[i] + a[k] + a[j] = 0
			// After shifting by SHIFT: val[i] + val[k] + val[j] = 3 * SHIFT
			int complement = 3 * SHIFT - (val[i] + val[j]);

			// If complement is in the valid index range, add its frequency
			if (0 <= complement && complement <= 2 * SHIFT)
				ways[i][j] = freq[complement];

			// Move j forward: the current j becomes "inside" for the next j
			freq[val[j]]++;
		}

		// Reset freq for this i so it's clean when we move to the next i
		for (int j = i + 1; j < n; j++) freq[val[j]]--;
	}

	// Turn ways into a 2D prefix-style DP:
	// after this, ways[i][j] = total number of zero-sum triples fully inside [i, j]
	for (int i = n - 1; i >= 0; i--)
		for (int j = i + 1; j < n; j++)
			ways[i][j] += ways[i + 1][j] + ways[i][j - 1] - ways[i + 1][j - 1];

	// Answer each query in O(1) using precomputed ways
	while (q--) {
		int l, r;
		cin >> l >> r;
		cout << ways[l - 1][r - 1] << '\n'; // convert to 0-based
	}
}

int main() {
	setIO("threesum");
	solve(); // single case
}
