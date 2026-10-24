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
const int MXN = (int)5e4+5; // N <= 5e4
const int MAXK = 20;        // K <= 20

/*
 (1) Problem recap in simple terms:
     We have an array A[1..N] with values in [1..K]. For each query [L,R], count the number of
     non-decreasing subsequences of A_L..A_R (including the empty subsequence) modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - K <= 20, so we can keep DP states by value and treat each element as a small linear transform.
     - For one value x, the DP update "append x to any subsequence ending with <= x" is linear, so it
       can be written as multiplying by a KxK upper-triangular matrix M_x.
     - For a range [L,R], the transform is M_{A_L} * ... * M_{A_R}. We want the sum of all DP states
       starting from dp = [1,0,0,...] (the empty subsequence).
     - With STO[i] = M_{A_1}*...*M_{A_i} and ISTO[i] = STO[i]^{-1}, the range product is
       ISTO[L-1] * STO[R]. We only need row-sums of STO[R] and the first row of ISTO[L-1].

 (3) Solution outline:
     - Scan A left to right, maintaining STO and ISTO (both KxK). Update them in O(K^2) using the
       special structure of M_x and M_x^{-1}.
     - For each prefix i, store sto[i][j] = sum_{t=j..K-1} STO[j][t], and isto[i][j] = ISTO[0][j].
     - For query [L,R], compute ans = sum_{j=0..K-1} isto[L-1][j] * sto[R][j].
     - Time complexity: O(N*K^2 + Q*K); Memory complexity: O(N*K).
 */

struct Mint {
	int v;
	Mint(ll _v = 0) {
		_v %= MOD;
		if (_v < 0) _v += MOD;
		v = int(_v);
	}
	Mint& operator+=(const Mint& o) {
		v += o.v;
		if (v >= MOD) v -= MOD;
		return *this;
	}
	Mint& operator-=(const Mint& o) {
		v -= o.v;
		if (v < 0) v += MOD;
		return *this;
	}
	Mint& operator*=(const Mint& o) {
		v = int((ll)v * o.v % MOD);
		return *this;
	}
	friend Mint operator+(Mint a, const Mint& b) { return a += b; }
	friend Mint operator-(Mint a, const Mint& b) { return a -= b; }
	friend Mint operator*(Mint a, const Mint& b) { return a *= b; }
};

using Mat = AR<AR<Mint, MAXK>, MAXK>;

inline void solve() {
	int n, k;
	cin >> n >> k;
	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	V<AR<Mint, MAXK>> sto(n + 1), isto(n + 1);

	Mat STO{}, ISTO{};
	for (int i = 0; i < k; i++) STO[i][i] = 1, ISTO[i][i] = 1;

	const Mint inv2 = Mint((MOD + 1) / 2);

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j < k; j++) {
			Mint sum = 0;
			for (int t = j; t < k; t++) sum += STO[j][t];
			sto[i][j] = sum;
		}
		for (int j = 0; j < k; j++) isto[i][j] = ISTO[0][j];
		if (i == n) break;

		int x = a[i] - 1;

		for (int j = 0; j <= x; j++) {
			for (int t = x; t >= j; t--) STO[j][x] += STO[j][t];
		}

		for (int j = 0; j < x; j++) {
			for (int t = x; t < k; t++) ISTO[j][t] -= inv2 * ISTO[x][t];
		}
		for (int t = x; t < k; t++) ISTO[x][t] *= inv2;
	}

	int q;
	cin >> q;
	for (int qq = 0; qq < q; qq++) {
		int l, r;
		cin >> l >> r;
		Mint ans = 0;
		for (int j = 0; j < k; j++) ans += isto[l - 1][j] * sto[r][j];
		cout << ans.v << '\n';
	}
}

int main() {
	setIO("nondec");
	solve(); // single case
}

/**
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
const int MXN = (int)5e4+5; // N <= 5e4
const int MAXK = 20;        // K <= 20

/*
 (1) Problem recap in simple terms:
Count non-decreasing subsequences inside many subarray queries [L,R] of A[1..N] (empty allowed),
modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
 - With values in [1..K], we can track DP by last value (size K) and each element x is a fixed
   linear transform M_x on that vector.
 - For a range, the transform is a product of these matrices; using prefix products STO[i] and
   inverse prefixes ISTO[i] lets us answer [L,R] with ISTO[L-1] * STO[R].
 - We only need row-sums of STO[R] and the first row of ISTO[L-1], not full matrices per query.
 - Updating STO by one x only needs row range-sums in columns [j..x], so Fenwick-per-row gives
   O(K log K) per element.
 - Updating ISTO by one x is column-wise: for each column c>=x, halve ISTO[x][c] and subtract it
   from rows <x. This is point-update + range-add, so Fenwick-per-column gives O(K log K).

 (3) Solution outline:
 - Maintain STO in K Fenwicks (one per row): point add + range sum.
 - Maintain ISTO in K range-add/point-query Fenwicks (one per column).
 - For each prefix i, read sto[i][j] = sum_{t=j..K-1} STO[j][t] and isto[i][j] = ISTO[0][j].
 - Answer each query by dot product sum_j isto[L-1][j] * sto[R][j].
 - Time complexity: O(N*K*logK + Q*K); Memory complexity: O(N*K + K^2).
 *[/]

struct Mint {
	int v;
	Mint(ll _v = 0) {
		_v %= MOD;
		if (_v < 0) _v += MOD;
		v = int(_v);
	}
	Mint& operator+=(const Mint& o) {
		v += o.v;
		if (v >= MOD) v -= MOD;
		return *this;
	}
	Mint& operator-=(const Mint& o) {
		v -= o.v;
		if (v < 0) v += MOD;
		return *this;
	}
	Mint& operator*=(const Mint& o) {
		v = int((ll)v * o.v % MOD);
		return *this;
	}
	friend Mint operator+(Mint a, const Mint& b) { return a += b; }
	friend Mint operator-(Mint a, const Mint& b) { return a -= b; }
	friend Mint operator*(Mint a, const Mint& b) { return a *= b; }
};

struct Fenwick {
	int n = 0;
	V<Mint> bit;
	Fenwick() {}
	Fenwick(int _n) { init(_n); }

	void init(int _n) {
		n = _n;
		bit.assign(n + 1, 0);
	}

	void add(int idx, Mint val) {
		for (idx++; idx <= n; idx += idx & -idx) bit[idx] += val;
	}

	Mint sumPrefix(int idx) {
		Mint res = 0;
		for (idx++; idx > 0; idx -= idx & -idx) res += bit[idx];
		return res;
	}

	Mint sumRange(int l, int r) {
		if (l > r) return 0;
		Mint res = sumPrefix(r);
		if (l) res -= sumPrefix(l - 1);
		return res;
	}
};

struct RUPQ {
	int n = 0;      // valid indices: 0..n-1
	Fenwick fw;     // built on size n+1 so we can write at index n

	void init(int _n) {
		n = _n;
		fw.init(n + 1);
	}

	void addRange(int l, int r, Mint val) {
		if (l > r) return;
		fw.add(l, val);
		fw.add(r + 1, Mint(0) - val);
	}

	Mint queryPoint(int idx) {
		return fw.sumPrefix(idx);
	}
};

inline void solve() {
	int n, k;
	cin >> n >> k;
	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	V<Fenwick> stoFw(k);
	for (int j = 0; j < k; j++) stoFw[j].init(k);

	V<RUPQ> istoFw(k);
	for (int c = 0; c < k; c++) istoFw[c].init(k);

	for (int j = 0; j < k; j++) stoFw[j].add(j, 1);
	for (int c = 0; c < k; c++) istoFw[c].addRange(c, c, 1);

	const Mint inv2 = Mint((MOD + 1) / 2);

	V<AR<Mint, MAXK>> sto(n + 1), isto(n + 1);

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j < k; j++) {
			sto[i][j] = stoFw[j].sumRange(j, k - 1);
			isto[i][j] = istoFw[j].queryPoint(0);
		}
		if (i == n) break;

		int x = a[i] - 1;

		for (int j = 0; j <= x; j++) {
			Mint delta = stoFw[j].sumRange(j, x);
			stoFw[j].add(x, delta);
		}

		for (int c = x; c < k; c++) {
			Mint cur = istoFw[c].queryPoint(x);
			Mint half = cur * inv2;
			istoFw[c].addRange(x, x, half - cur);
			if (x) istoFw[c].addRange(0, x - 1, Mint(0) - half);
		}
	}

	int q;
	cin >> q;
	for (int qq = 0; qq < q; qq++) {
		int l, r;
		cin >> l >> r;
		Mint ans = 0;
		for (int j = 0; j < k; j++) ans += isto[l - 1][j] * sto[r][j];
		cout << ans.v << '\n';
	}
}

int main() {
	setIO("nondec");
	solve(); // single case
}
*/