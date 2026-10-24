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
const int MXN = 505; // modify as needed

/*
 (1) Problem recap in simple terms:
     We are given two arrays A[1..n] and B[1..n]. We split A into some number of non-empty
     contiguous blocks, and split B into the same number of non-empty contiguous blocks
     (both left-to-right). For every matching pair of blocks, the average of A's block
     must be <= the average of B's block. Count how many ways to do this, modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - Let dp[i][j] be the number of valid ways to partition A[1..i] and B[1..j].
     - If the last cuts are at x < i and c < j, we compare the last-block averages:
       (sumA(i)-sumA(x)) / (i-x) <= (sumB(j)-sumB(c)) / (j-c).
     - For fixed i and j, if we sort all x by avg(A[x+1..i]) and all c by avg(B[c+1..j]),
       then as avgB increases, the valid x set is always a prefix (two-pointer works).
     - We can precompute prefix sums over dp rows in that sorted-x order so each column
       query adds a whole prefix in O(1).

 (3) Solution outline:
     - Build prefix sums preA, preB for fast segment sums.
     - For each i, build ordA[i] = cuts x in [0..i-1] sorted by avg(A[x+1..i]).
     - For each j, build ordB[j] = cuts c in [0..j-1] sorted by avg(B[c+1..j]).
     - DP over i = 1..n:
       - Build psm over ordA[i]: psm[r][c] = sum(dp[ordA[i][0..r]][c]).
       - For each j = 1..n, two-pointer over ordB[j] while growing a prefix of ordA[i]
         whose avgA <= current avgB, and add psm[prefixEnd][c] into dp[i][j].
     - O(n^3) time, O(n^2) memory.
 */

inline bool avgLeA(
	const vl& preA, int i, int x,
	const vl& preB, int j, int c
) {
	ll sumA = preA[i] - preA[x];
	ll sumB = preB[j] - preB[c];
	int lenA = i - x;
	int lenB = j - c;
	return sumA * (ll)lenB <= sumB * (ll)lenA;
}

inline void solve() {
	int n; cin >> n;

	vl preA(n + 1), preB(n + 1);
	for (int i = 1; i <= n; i++) {
		ll x; cin >> x;
		preA[i] = preA[i - 1] + x;
	}
	for (int i = 1; i <= n; i++) {
		ll x; cin >> x;
		preB[i] = preB[i - 1] + x;
	}

	V<vi> ordA(n + 1), ordB(n + 1);
	for (int i = 1; i <= n; i++) {
		ordA[i].rsz(i);
		ordB[i].rsz(i);
		for (int x = 0; x < i; x++) ordA[i][x] = ordB[i][x] = x;

		auto cmpA = [&](int x, int y) -> bool {
			ll sx = preA[i] - preA[x], sy = preA[i] - preA[y];
			int lx = i - x, ly = i - y;
			return sx * (ll)ly < sy * (ll)lx;
		};
		auto cmpB = [&](int x, int y) -> bool {
			ll sx = preB[i] - preB[x], sy = preB[i] - preB[y];
			int lx = i - x, ly = i - y;
			return sx * (ll)ly < sy * (ll)lx;
		};
		sort(all(ordA[i]), cmpA);
		sort(all(ordB[i]), cmpB);
	}

	V<vi> dp(n + 1, vi(n + 1));
	dp[0][0] = 1;

	for (int i = 1; i <= n; i++) {
		vi& R = ordA[i];

		V<vi> psm(i, vi(n + 1));
		for (int rr = 0; rr < i; rr++) {
			for (int c = 0; c <= n; c++) {
				int v = dp[R[rr]][c];
				if (rr) v += psm[rr - 1][c];
				if (v >= MOD) v -= MOD;
				psm[rr][c] = v;
			}
		}

		for (int j = 1; j <= n; j++) {
			vi& C = ordB[j];
			int rr = 0;

			for (int c : C) {
				while (rr < i && avgLeA(preA, i, R[rr], preB, j, c)) rr++;
				if (!rr) continue;

				int add = psm[rr - 1][c];
				dp[i][j] += add;
				if (dp[i][j] >= MOD) dp[i][j] -= MOD;
			}
		}
	}

	cout << dp[n][n] << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
