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
 (1) Problem recap in simple terms
     - We have N piles with sizes a[i]. A legal move takes s stones from some pile.
       Next move must take a multiple of the previous s. First player (Bessie) wants to
       choose her first move (pile, s1) so she is guaranteed to win, regardless of Elsie.
     - We must count how many first moves (pile and amount) are winning.

 (2) Things to notice & step-by-step derivation
     - Fix a first move amount x = s1. All future amounts are multiples of x.
       If we divide every pile size by x and look at S_x = floor(a[i]/x), then the game
       is equivalent to repeatedly picking some pile with S_x[i] > 0 and decreasing it by 1.
       (Because taking k·x stones corresponds to subtracting k from floor(a[i]/x) one step at a time.)
     - In this reduced game, a position is losing iff every positive integer value appears an even # of times
       in S_x (mirror strategy). Otherwise it's winning, and the canonical winning move is to
       decrease one pile from the largest t whose frequency in S_x is odd, making all positive counts even.
     - For a fixed x, let counts[t] = # of piles with floor(a[i]/x) = t. We only care about t ≥ 1.
       Bessie has winning first moves for this x exactly when:
         • odd indices in { t ≥ 1 | counts[t] is odd } are either {1} or {t-1, t} for some t.
       In that case, the number of concrete first moves using amount x equals counts[largest odd t]
       (choose any pile in that bucket to reduce).

 (3) Solution outline
     - Precompute prefix counts of values to query counts on intervals [x*t, x*(t+1)-1].
     - For each x from 1..max(a), build counts[t] via prefix sums, gather the odd t's.
     - If odd == {1} OR odd == {t-1, t} (two consecutive), add counts[last odd] to the answer.
     - Complexity: O(N + maxA * log maxA) due to harmonic series sum over x of maxA/x.
*/

inline void solve() {
	int N; cin >> N;
	vi A(N);
	int mx = 0; // maxA
	for (int &t : A) {
		cin >> t;
		mx = max(mx, t);
	}

	// pref[v] = # of elements in A with value ≤ v (frequency prefix over values 0..mx).
	vi pref(mx + 1);
	for (int t : A) ++pref[t];
	for (int i = 1; i <= mx; ++i) pref[i] += pref[i - 1];

	// Helper: # of A's ≤ ind (handles ind > mx safely).
	auto getPref = [&](int ind) {
		if (ind > mx) return pref.bk;
		return pref[ind];
	};

	ll ans = 0;

	// Try every possible first-take amount x = s1.
	for (int x = 1; x <= mx; ++x) {
		// counts[t] = # of a in [x*t, x*(t+1)-1]  (i.e., floor(a/x) == t).
		// Put a dummy counts[0] to align indices so t starts at 1.
		vi counts{0};
		for (int t = 1; x * t <= mx; ++t) {
			int L = x * t, R = x * (t + 1) - 1;
			counts.pb(getPref(R) - getPref(L - 1));
		}

		// Collect indices t ≥ 1 having odd frequency in S_x.
		vi odd;
		for (int t = 1; t < sz(counts); ++t)
			if (counts[t] & 1) odd.pb(t);

		// Winning pattern for this x:
		//   - exactly {1}, meaning only 1's occur odd times; OR
		//   - exactly two consecutive indices {t-1, t}.
		// If so, the winning first moves using amount x are exactly the piles in the top odd bucket.
		if (odd == vi{1} || (sz(odd) == 2 && odd[0] + 1 == odd[1]))
			ans += counts[odd.bk];
	}

	cout << ans << "\n";
}

int main() {
    setIO();
    solve(); // One test, Truly
}