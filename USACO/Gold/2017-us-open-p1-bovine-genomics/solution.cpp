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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); }
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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap:
Given N spotty and N plain genomes of length M, find the minimum length L of a contiguous window
so that spotty substrings and plain substrings are completely disjoint for that window.

2) Key idea (why it works):
Fix a start position i. Let k be the maximum LCP (common-prefix length) between any spotty/plain pair
when comparing suffixes starting at i. Any window starting at i must have L > k, so the best L for this i is k+1.
To get k, sort all 2N strings by their suffix starting at i; the maximum cross-group LCP happens between
some adjacent cross-group pair in this sorted order.

3) Solution outline:
Build suffix order for each i from right to left using stable bucket sort over {A,C,G,T}.
Use double rolling hash to compute LCP of two suffixes in O(log M).
For each i, scan adjacent indices in suffix order, take max cross-group LCP, and update answer with (LCP+1).
Time Complexity: O(M * N * log M). Memory: O(M * N).
*/

static inline int cid(char c) {
	if (c == 'A') return 0;
	if (c == 'C') return 1;
	if (c == 'G') return 2;
	return 3; // 'T'
}

inline void solve() {
	int n, m; cin >> n >> m;
	vs g(2*n);
	for (auto &s : g) cin >> s;

	// double rolling hash prep (small M, but keeps worst-case fast)
	const int MOD1 = 1000000007;
	const int MOD2 = 1000000009;
	const int B1 = 911382323;
	const int B2 = 972663749;

	vi pw1(m+1, 1), pw2(m+1, 1);
	for (int i = 1; i <= m; i++) {
		pw1[i] = (ll)pw1[i-1] * B1 % MOD1;
		pw2[i] = (ll)pw2[i-1] * B2 % MOD2;
	}

	V<vi> p1(2*n, vi(m+1, 0)), p2(2*n, vi(m+1, 0));
	for (int id = 0; id < 2*n; id++) {
		for (int i = 0; i < m; i++) {
			int v = cid(g[id][i]) + 1; // map A,C,G,T -> 1..4
			p1[id][i+1] = ((ll)p1[id][i] * B1 + v) % MOD1;
			p2[id][i+1] = ((ll)p2[id][i] * B2 + v) % MOD2;
		}
	}

	auto getHash = [&](int id, int l, int r) -> pi {
		ll x1 = p1[id][r] - (ll)p1[id][l] * pw1[r-l] % MOD1;
		ll x2 = p2[id][r] - (ll)p2[id][l] * pw2[r-l] % MOD2;
		if (x1 < 0) x1 += MOD1;
		if (x2 < 0) x2 += MOD2;
		return mp((int)x1, (int)x2);
	};

	auto lcpLen = [&](int i, int a, int b) -> int {
		int lo = 0, hi = m - i;
		while (lo < hi) {
			int mid = lo + (hi - lo + 1) / 2;
			getHash(a, i, i+mid) == getHash(b, i, i+mid) ? lo = mid : hi = mid - 1;
		}
		return lo; // length of match starting at i
	};

	vi ord(2*n);
	iota(all(ord), 0);

	int ans = (int)1e9;

	for (int i = m-1; i >= 0; i--) {
		AR<vi,4> buck;
		for (int idx : ord) buck[cid(g[idx][i])].pb(idx);

		ord.clear();
		for (int b = 0; b < 4; b++) {
			for (int idx : buck[b]) ord.pb(idx);
		}

		int mxMismatch = i; // stores mismatch index (i + max LCP)
		for (int j = 0; j < 2*n-1; j++) {
			int a = ord[j], b = ord[j+1];
			if ((a < n) == (b < n)) continue; // same group, ignore
			int k = lcpLen(i, a, b);
			mxMismatch = max(mxMismatch, i + k);
		}

		if (mxMismatch < m) ans = min(ans, mxMismatch + 1 - i);
	}

	cout << ans << "\n";
}

int main() {
	setIO("cownomics");
	solve(); // single case
}
