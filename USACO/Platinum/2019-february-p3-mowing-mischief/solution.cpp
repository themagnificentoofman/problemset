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
 (1) Problem recap in simple terms:
     Pick as many flowers as possible that can be visited in increasing (x,y) order from (0,0)
     to (T,T). Among all maximum-size choices, minimize the worst-case grass area that can be
     swept by the wire; this equals a sum of rectangle areas between consecutive chosen flowers.

 (2) Things to notice and step-by-step derivation:
     - A maximum feasible set S is exactly a longest increasing subsequence in 2D: after sorting
       by x, it is a LIS in y.
     - If we pick a chain ... -> i -> j -> ..., the added contribution from i to j is the rectangle
       area (xj - xi) * (yj - yi), so we want the minimum-sum chain among all LIS chains.
     - Points with the same LIS length form an antichain: within a level, x increases while y
       decreases, making each point dominate a contiguous interval in the next level.

 (3) Solution outline:
     - Sort points by x, run patience sorting to build LIS levels (by y) and group points by level.
     - DP over levels: dp[p] = min cost to end at p, with transitions only from previous level.
     - For each adjacent pair of levels, use the interval structure and monotone hull sweeps to
       evaluate all dp transitions in near-linear time.
     - Time: O(N log N). Memory: O(N).
 */

inline void solve() {
	int n, tLen;
	cin >> n >> tLen;
	int n0 = n;
	vi x(n0+1), y(n0+1);
	for (int i = 0; i < n0; i++) cin >> x[i] >> y[i];
	x[n0] = tLen;
	y[n0] = tLen;
	n = n0 + 1;

	vl dp(n, BIG);
	vi xid(n);
	iota(all(xid), 0);

	sort(all(xid), [&](int a, int b){ return x[a] < x[b]; });

	V<vi> levels(n);
	vi lis(n, 0);
	levels[0].pb(xid[0]);
	lis[xid[0]] = 0;
	int mx = 0;
	for (int ii = 1; ii < n; ii++) {
		int cur = xid[ii];
		int lo = -1, hi = mx;
		while (lo != hi) {
			int mid = (lo + hi + 1) / 2;
			if (y[levels[mid].bk] < y[cur]) lo = mid;
			else hi = mid - 1;
		}
		levels[lo+1].pb(cur);
		mx = max(mx, lo+1);
		lis[cur] = lo+1;
	}

	auto cost = [&](int i, int j) -> ll {
		ll dx = (ll)x[i] - x[j];
		ll dy = (ll)y[i] - y[j];
		return dp[i] + dx * dy;
	};

	auto findOvertake = [&](int lev, int i, int j) -> int {
		int lo = 0, hi = sz(levels[lev]);
		while (lo != hi) {
			int mid = (lo + hi) / 2;
			int q = levels[lev][mid];
			if (cost(i, q) < cost(j, q)) hi = mid;
			else lo = mid + 1;
		}
		return lo;
	};

	auto findFirst = [&](int lev, int i) -> int {
		int lo = 0, hi = sz(levels[lev]) - 1;
		while (lo != hi) {
			int mid = (lo + hi) / 2;
			if (x[levels[lev][mid]] > x[i]) hi = mid;
			else lo = mid + 1;
		}
		int j = levels[lev][lo];
		if (x[j] > x[i] && y[j] > y[i]) return lo;
		return -1;
	};

	auto findLast = [&](int lev, int i) -> int {
		int lo = 0, hi = sz(levels[lev]) - 1;
		while (lo != hi) {
			int mid = (lo + hi + 1) / 2;
			if (y[levels[lev][mid]] > y[i]) lo = mid;
			else hi = mid - 1;
		}
		int j = levels[lev][lo];
		if (x[j] > x[i] && y[j] > y[i]) return lo;
		return -1;
	};

	for (int idx : levels[0]) dp[idx] = 1LL * x[idx] * y[idx];

	vi que, overtake;
	vi curLevel, firstDom, lastDom;

	auto solveStartingRegion = [&](int lev, int iStart, int iEnd, int qStart, int qEnd) {
		int len = 0;
		int i = iStart;
		for (int j = qStart; j <= qEnd; j++) {
			int q = levels[lev][j];
			while (i <= iEnd && firstDom[i] <= j) {
				while (len >= 2 && overtake[len-2] <= findOvertake(lev, que[len-1], curLevel[i])) {
					len--;
				}
				que[len] = curLevel[i];
				if (len >= 1) overtake[len-1] = findOvertake(lev, que[len-1], curLevel[i]);
				len++;
				i++;
			}
			while (len >= 2 && overtake[len-2] <= j) len--;
			dp[q] = min(dp[q], cost(que[len-1], q));
		}
	};

	auto solveEndingRegion = [&](int lev, int iStart, int iEnd, int qStart, int qEnd) {
		int len = 0;
		int i = iEnd;
		for (int j = qEnd; j >= qStart; j--) {
			int q = levels[lev][j];
			while (i >= iStart && lastDom[i] >= j) {
				while (len >= 2 && overtake[len-2] >= findOvertake(lev, curLevel[i], que[len-1])) {
					len--;
				}
				que[len] = curLevel[i];
				if (len >= 1) overtake[len-1] = findOvertake(lev, curLevel[i], que[len-1]);
				len++;
				i--;
			}
			while (len >= 2 && overtake[len-2] > j) len--;
			dp[q] = min(dp[q], cost(que[len-1], q));
		}
	};

	for (int lev0 = 0; lev0 + 1 <= mx && sz(levels[lev0+1]); lev0++) {
		curLevel.clear();
		firstDom.clear();
		lastDom.clear();

		for (int cur : levels[lev0]) {
			int fd = findFirst(lev0+1, cur);
			if (fd == -1) continue;
			int ld = findLast(lev0+1, cur);
			firstDom.pb(fd);
			lastDom.pb(ld);
			curLevel.pb(cur);
		}

		que.assign(sz(curLevel), 0);
		overtake.assign(sz(curLevel), 0);

		for (int i = 0; i < sz(curLevel); ) {
			int iEnd = i;
			while (iEnd + 1 < sz(curLevel) && firstDom[iEnd+1] <= lastDom[i]) iEnd++;

			solveStartingRegion(lev0+1, i, iEnd, firstDom[i], lastDom[i]);
			if (lastDom[iEnd] >= lastDom[i] + 1) {
				solveEndingRegion(lev0+1, i+1, iEnd, lastDom[i] + 1, lastDom[iEnd]);
			}
			i = iEnd + 1;
		}
	}

	cout << dp[n-1] << '\n';
}

int main() {
	setIO("mowing");
	solve(); // single case
}
