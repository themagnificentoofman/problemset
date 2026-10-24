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
     We yell at some cow IDs (in increasing ID order); yelled cows can swap with neighbors until sorted.
     Find the smallest set of IDs to yell so the lineup becomes sorted, and output the K-th smallest set.

 (2) Things to notice and step-by-step derivation:
     - Cows we never yell at never swap with each other, so they keep their original relative order.
     - The lineup ends sorted iff the "not yelled" cows form an increasing subsequence of the original list.
     - So we want to keep a LIS; answer size is N - LIS.
     - The K-th smallest minimal set equals the complement of the K-th lexicographically largest LIS.

 (3) Solution outline:
     - Compute lisStart[i] (LIS length starting at position i) with a BIT doing suffix-maximum by value.
     - DP from right: dp[i] = number of LIS of length lisStart[i] starting at i (cap at K+1).
     - Reconstruct the K-th lexicographically largest LIS layer-by-layer (decreasing ID), then output its
       complement.
     - O(N log N) time, O(N) memory
 */

struct BitMax {
	int n = 0;
	vi bit;
	void init(int _n) {
		n = _n;
		bit.assign(n+1, 0);
	}
	void upd(int i, int v) {
		for (; i <= n; i += i & -i) bit[i] = max(bit[i], v);
	}
	int query(int i) {
		int res = 0;
		for (; i > 0; i -= i & -i) res = max(res, bit[i]);
		return res;
	}
};

struct SegSum {
	int n = 0;
	int base = 1;
	ll cap = 0;
	vl seg;

	ll addCap(ll a, ll b) const {
		if (a >= cap || b >= cap) return cap;
		if (a > cap - b) return cap;
		return a + b;
	}

	void init(int _n, ll _cap) {
		n = _n;
		cap = _cap;
		base = 1;
		while (base < n) base <<= 1;
		seg.assign(2*base, 0);
	}

	void add(int idx, ll v) { // idx is 0-based
		int p = idx + base;
		seg[p] = addCap(seg[p], v);
		for (p >>= 1; p; p >>= 1) seg[p] = addCap(seg[2*p], seg[2*p+1]);
	}

	ll query(int l, int r) const { // [l, r), 0-based
		ll L = 0, R = 0;
		l += base, r += base;
		while (l < r) {
			if (l & 1) L = addCap(L, seg[l++]);
			if (r & 1) R = addCap(seg[--r], R);
			l >>= 1, r >>= 1;
		}
		return addCap(L, R);
	}
};

inline void solve() {
	int n;
	ll k;
	cin >> n >> k;
	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	const ll CAP = min(BIG, k + 1);

	// lis[i] = length of the longest increasing subsequence starting at position i
	BitMax bitMax;
	bitMax.init(n);
	vi lis(n, 1);
	int best = 0;

	for (int i = n-1; i >= 0; i--) {
		int q = n - a[i];              // values > a[i] map to BIT indices [1..q]
		int cur = 1 + bitMax.query(q);
		lis[i] = cur;
		best = max(best, cur);
		int pos = n - a[i] + 1;
		bitMax.upd(pos, cur);
	}

	V<vi> ids(best), loc(best);
	V<vl> dp(best);
	vi idx(n);

	for (int i = n-1; i >= 0; i--) {
		int lv = lis[i] - 1;
		idx[i] = sz(ids[lv]);
		ids[lv].pb(a[i]);
		loc[lv].pb(i);
		dp[lv].pb(0);
	}

	V<SegSum> seg(best);
	vi ptr(best, 0);
	for (int lv = 0; lv < best; lv++) seg[lv].init(sz(ids[lv]), CAP);

	for (int i = n-1; i >= 0; i--) {
		int lv = lis[i] - 1;
		int id = idx[i];

		if (lv == 0) {
			dp[lv][id] = 1;
			seg[lv].add(id, 1);
			continue;
		}

		while (ptr[lv] < sz(ids[lv-1]) && a[i] > ids[lv-1][ptr[lv]]) ptr[lv]++;
		ll cur = seg[lv-1].query(ptr[lv], sz(ids[lv-1]));
		dp[lv][id] = cur;
		seg[lv].add(id, cur);
	}

	ll rem = k;
	vb inAns(n+1, true);

	int prevPos = -1;
	for (int lv = best-1; lv >= 0; lv--) {
		int use = sz(ids[lv]) - 1;

		while (use >= 0 && loc[lv][use] < prevPos) use--;
		while (use >= 0 && dp[lv][use] < rem) {
			rem -= dp[lv][use];
			use--;
		}

		prevPos = loc[lv][use];
		inAns[ids[lv][use]] = false; // not yelled (in the chosen LIS)
	}

	int ansSz = 0;
	for (int x = 1; x <= n; x++) if (inAns[x]) ansSz++;

	cout << ansSz << "\n";
	for (int x = 1; x <= n; x++) if (inAns[x]) cout << x << "\n";
}

int main() {
	setIO("itout");
	solve(); // single case
}
