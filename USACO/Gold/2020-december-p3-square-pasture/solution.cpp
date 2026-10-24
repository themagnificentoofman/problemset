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
 (1) problem recap in simple terms:
     - We have N cows on distinct grid cells (distinct x and y).
     - Count how many distinct subsets of cows can be enclosed by an axis-aligned square of cells
       (the empty set and singletons count).

 (2) things to notice and step-by-step derivation:
     - Any subset S with |S| >= 2 that fits in some square has a bounding rectangle R(S).
     - If width(R) >= height(R), then S fits in a square whose left/right sides match the leftmost/rightmost cows.
       Let those extremes be a (left) and b (right), with side length len = xb - xa.
     - The square’s vertical position is [y, y+len]. For a,b to be inside, y must lie in:
          y ∈ [lo, hi], where lo = max(ya, yb) - len, hi = min(ya, yb).
     - All cows with xa < x < xb must also lie within the square’s vertical interval; as y increases,
       cows only enter through the top or leave through the bottom. We can sweep y using two pointers on
       the sorted list of intermediate y-coordinates and count each maximal interval of y that yields a valid square.
     - To handle height(R) > width(R), swap x and y and repeat.
     - Bounding rectangles with width == height are counted in both orientations; subtract those once.

 (3) solution outline:
     - ans starts at N + 1 (empty set + N singletons).
     - sweep_once(cows): sort by x; for each (a,b) as extremes, collect y’s of cows strictly between them,
       build two-pointer window over y as we move the square upward from lo to hi; count one square per
       maximal y-interval; also detect when the bounding rectangle becomes a square (height == len) to mark duplicates.
     - Run sweep_once on (x,y), then swap coordinates and run again.
     - Final answer = ans - eq/2, where eq is the number of “width == height” cases seen across both sweeps.

     Complexity: O(N^3 log N) or O(N^3) with careful structures (N ≤ 200 is fine).
*/

// Sweep once assuming we are counting subsets whose bounding rectangle has width >= height.
// cows: (x,y) pairs. ans accumulates total counted squares; eq counts cases where height == width.
static void sweep_once(vpi& cows, ll& ans, ll& eq) {
	/* Sort by x so a,b enumerate the left/right extremes in x-order. */
	sor(cows);

	int N = sz(cows);
	for (int a = 0; a < N; ++a) {                 // leftmost cow index
		set<int> midsY;                           // y-coordinates of cows strictly between a and b in x-order

		for (int b = a+1; b < N; ++b) {          // rightmost cow index
			if (a < b-1) midsY.ins(cows[b-1].se); // maintain y’s of (a, b) interior cows as b advances

			int len = cows[b].fi - cows[a].fi;    // candidate square side length (width)
			int lo  = max(cows[a].se, cows[b].se) - len; // lowest y that keeps both a,b inside
			int hi  = min(cows[a].se, cows[b].se);       // highest bottom y allowed

			if (lo > hi) continue;               // impossible if height(R) would exceed width

			// Prepare two-pointer sweep over the y-sorted interior cows.
			vi y(all(midsY));               // sorted ascending
			int l = 0, r = -1;                  // current window [l..r] are interior cows inside [bot, top]
			// Initialize window for bottom = lo, top = lo + len.
			while (l < sz(y) && lo >= y[l] + 1) l++;            // advance until y[l] >= lo
			while (r+1 < sz(y) && lo >= y[r+1] - len) r++;      // extend while y[r+1] <= lo+len

			/*
			 Sweep bottom y upward. Each iteration counts one maximal y-interval where the in-square
			 interior set [l..r] is unchanged. The bounding rectangle’s vertical span is:
			   [yl, yr] = hull of {ya, yb} and (if l<=r) {y[l], y[r]}.
			 Validity invariant: yr - yl <= len.
			*/
			for (;;) {
				++ans; // count this maximal y-position interval (i.e., one distinct subset)

				int yl = min(cows[a].se, cows[b].se);
				int yr = max(cows[a].se, cows[b].se);
				if (l <= r) { yl = min(yl, y[l]); yr = max(yr, y[r]); }

				// If height == width, this bounding rectangle is a square — will be seen in both orientations.
				assert(yr - yl <= len);
				eq += (yr - yl == len);

				// Find the next y at which the interior set changes:
				// - bottom passes y[l]  -> cow l leaves when bottom hits y[l] + 1
				// - top meets  y[r+1]   -> cow r+1 enters when bottom hits y[r+1] - len
				int leave_bottom = (l < sz(y)   ? y[l] + 1     : INT32_MAX);
				int enter_top    = (r+1 < sz(y) ? y[r+1] - len : INT32_MAX);
				int change_y     = min(leave_bottom, enter_top);

				if (change_y > hi) break;        // no further valid bottoms in [lo, hi]

				// Apply whichever event happens:
				if (leave_bottom == change_y) l++;
				if (enter_top    == change_y) r++;
			}
		}
	}
}

inline void solve() {
	int N; 
	cin >> N;
	vpi cows(N);
	for (auto& p : cows) cin >> p.fi >> p.se;

	/*
	  ans includes:
	    - 1 for the empty set
	    - N for each singleton (any single cow fits in a 1x1 square)
	    - plus all |S| >= 2 subsets counted by sweeps.
	*/
	ll ans = N + 1;
	ll eq  = 0; // number of width==height bounding-rectangles seen across both sweeps

	// Sweep assuming width >= height (x is the "wide" axis).
	{
		vpi A = cows;
		sweep_once(A, ans, eq);
	}

	// Sweep after swapping coordinates to handle height > width.
	{
		vpi B = cows;
		for (auto& p : B) swap(p.fi, p.se);
		sweep_once(B, ans, eq);
	}

	// Each square bounding-rectangle (width==height) was counted twice; remove duplicates.
	assert(eq % 2 == 0);
	cout << (ans - eq/2) << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
