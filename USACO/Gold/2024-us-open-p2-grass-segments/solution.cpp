// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;

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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // ind of 1st elem >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // ind of 1st elem > b
template<class T> using pq = priority_queue<T>; // max-heap
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>; // min-heap

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
1) Problem recap
Bessie plants cultivar i on interval [l_i, r_i] on the positive real line. Cultivar i
"grows better" if there exists a different cultivar j such that the overlap length of
their intervals is at least k_i. For each i, output how many j != i satisfy
max(0, min(r_i, r_j) - max(l_i, l_j)) >= k_i.

2) Things to notice and step-by-step derivation
- Overlap >= k_i means there exists a length-k_i sub-interval inside BOTH segments.
- Let segment i be [l, r] with k. Segment j = [L, R] works iff:
  - R >= l + k   (j reaches far enough right)
  - L <= r - k   (j starts far enough left)
- Also, j must have length (R - L) >= k, otherwise it cannot contain any length-k
  sub-interval at all.
- We can answer offline by sweeping k from large to small, maintaining all segments
  whose length >= current k, and counting how many of them satisfy the two endpoint
  inequalities for the current query segment.

3) Solution outline
- Sort segment indices by decreasing length (r_i - l_i).
- Sort query indices (the same segments) by decreasing k_i.
- Sweep queries in that order, inserting into an "active" set all segments with
  length >= current k_i.
- Keep two order-statistics trees over active segments:
  - one ordered by L, one ordered by R
- For segment i with k:
  - active = #active
  - badL = #segments with L > r_i - k
  - badR = #segments with R < l_i + k
  - answer = active - 1 - badL - badR
  (the two bad sets cannot overlap among active segments, so no inclusion-exclusion
  correction is needed).
- Time: O(n log n), Memory: O(n).
*/

// Policy-based ordered set: order_of_key gives # items strictly smaller.
template<class T>
using iset = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;
#define ook order_of_key

inline void solve() {
	int n;
	cin >> n;

	vi l(n), r(n), k(n);
	for (int i = 0; i < n; i++) cin >> l[i] >> r[i] >> k[i];

	vi byLen(n), byK(n);
	for (int i = 0; i < n; i++) byLen[i] = byK[i] = i;

	sort(all(byLen), [&](int a, int b) -> bool {
		return mp(r[a] - l[a], a) > mp(r[b] - l[b], b);
	});
	sort(all(byK), [&](int a, int b) -> bool {
		return mp(k[a], a) > mp(k[b], b);
	});

	/*
	Store (endpoint, id) so duplicates stay distinct.
	id is the segment index (unique).
	*/
	iset<AR<int, 2>> lefts, rights;

	vi ans(n, 0);
	int j = 0; // number of inserted segments
	for (int i = 0; i < n; i++) {
		int id = byK[i];

		while (j < n && r[byLen[j]] - l[byLen[j]] >= k[id]) {
			int v = byLen[j];
			lefts.insert({l[v], v});
			rights.insert({r[v], v});
			j++;
		}

		int active = j;
		int cur = active - 1; // exclude itself

		int limL = r[id] - k[id];     // need L <= limL
		int needR = l[id] + k[id];    // need R >= needR

		int cntLE = lefts.ook({limL + 1, -1}); // L <= limL
		int badL = active - cntLE;                      // L > limL
		cur -= badL;

		int badR = rights.ook({needR, -1});    // R < needR
		cur -= badR;

		ans[id] = cur;
	}

	for (int i = 0; i < n; i++) cout << ans[i] << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
