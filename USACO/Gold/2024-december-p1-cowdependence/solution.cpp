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
// lower_bound / upper_bound convenience; use on sorted containers
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // ind of 1st elem >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // ind of 1st elem > b
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;

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
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple terms):
   For each distance x = 1..N, group equal values by scanning their positions left-to-right and
   starting a new group whenever the next occurrence is > x away from the group’s first occurrence.
   Output the total number of groups across all values for each x.

2) Things to notice / step-by-step derivation:
   - For a fixed value v, let g_v(x) be the number of groups when max span per group is x.
     As x increases, g_v(x) is non-increasing and only changes at certain x.
   - We can compute g_v(x) with a greedy two-pointer scan on the sorted position list of v.
   - Use Nachia-style divide & conquer on x to fill all g_v(x) values using only O(#changes * log N)
     point probes, while adding contributions to a global difference array.

3) Solution outline:
   - Build pos[v] = sorted indices where value v appears.
   - For each v, run D&C over x in [1..N] with boundary values g_v(0)=|pos[v]| and g_v(N+1)=1.
     Each constant interval becomes a range-add into diff[].
   - Prefix-sum diff[] and print diff[x] for x=1..N.
   - Time: O( (sum over v of (#probes_v * |pos[v]|)) ), typically near O(N log N); memory O(N).
*/

static inline int groups_at_x(const vi& pos, int x) {
	int m = sz(pos);
	if (!m) return 0;
	int ans = 0;
	for (int l = 0; l < m; ) {
		int r = l;
		while (r < m && pos[r] - pos[l] <= x) r++;
		ans++;
		l = r;
	}
	return ans;
}

struct DCSolver {
	const vi& pos;
	vi& diff;
	int N;

	DCSolver(const vi& _pos, vi& _diff, int _N) : pos(_pos), diff(_diff), N(_N) {}

	// add val to all x in [L,R] (x is in [1..N])
	inline void add_range(int L, int R, int val) {
		if (L > R) return;
		diff[L] += val;
		if (R + 1 < sz(diff)) diff[R + 1] -= val; // diff has size N+2, so R+1 up to N+1 is ok
	}

	// leftVal = g(L-1), rightVal = g(R+1)
	void solve_interval(int L, int R, int leftVal, int rightVal) {
		if (L > R) return;
		if (leftVal == rightVal) {
			add_range(L, R, leftVal);
			return;
		}
		int M = (L + R) >> 1;
		int midVal = groups_at_x(pos, M);
		add_range(M, M, midVal);
		solve_interval(L, M - 1, leftVal, midVal);
		solve_interval(M + 1, R, midVal, rightVal);
	}
};

inline void solve() {
	int N;
	cin >> N;

	V<vi> pos(N + 1);
	for (int i = 1; i <= N; i++) {
		int a; cin >> a;
		pos[a].pb(i); // store 1-based indices
	}

	vi diff(N + 2); // we use indices 1..N, with N+1 as a safe "end" marker

	for (int v = 1; v <= N; v++) {
		const vi& p = pos[v];
		if (p.empty()) continue;

		if (sz(p) == 1) {
			// always 1 group for any x>=1
			diff[1] += 1;
			diff[N + 1] -= 1;
			continue;
		}

		int leftVal = sz(p); // g(0)
		int rightVal = 1;    // g(N+1)
		DCSolver dc(p, diff, N);
		dc.solve_interval(1, N, leftVal, rightVal);
	}

	for (int x = 1; x <= N; x++) {
		diff[x] += diff[x - 1];
		cout << diff[x] << "\n";
	}
}

int main() {
	setIO();
	solve(); // single case
}
