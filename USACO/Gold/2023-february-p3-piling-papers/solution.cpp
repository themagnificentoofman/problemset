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
     You have a fixed sequence of digits. For each query [l, r], you take digits l..r in order
     and build a number by inserting each next digit either to the left end or the right end.
     Count how many numbers you can make that fall in [a, b], modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - Use inclusion-exclusion: count(<=b) - count(<=a-1).
     - For a fixed limit string L, we can DP while streaming digits: each digit goes to the
       leftmost unfilled spot or the rightmost unfilled spot of a length-|L| layout.
     - Track comparison state (<, =, >) against L based on the most significant decided digit.

 (3) Solution outline:
     - Implement solveForLimit(L): for every subarray [j, k], compute ways to form a number
       <= L using a DP over remaining interval [x..y] in L and comparison state.
     - Precompute answers for L=b and L=a-1, then answer each query by subtraction mod MOD.
     - Time: O(n^2 * m^2 + q); Memory: O(n^2 + m^2), where m = number of digits in L.
 */

inline int cmp(char a, char b) {
	if (a < b) return 0;
	if (a == b) return 1;
	return 2;
}

static V<vl> solveForLimit(const str& limit, const str& digits) {
	int m = sz(limit), n = sz(digits);
	V<vl> ans(n, vl(n, 0));
	const ll MODL = MOD;

	for (int j = 0; j < n; j++) {
		V<V<AR<ll, 3>>> dp(m, V<AR<ll, 3>>(m, {0, 0, 0}));

		for (int k = j; k < n; k++) {
			char d = digits[k];

			for (int x = 0; x < m; x++) {
				for (int y = m - 1; y > x; y--) {
					if (d > limit[x]) {
						ll add = (dp[x + 1][y][0] + dp[x + 1][y][1] + dp[x + 1][y][2]) % MODL;
						dp[x][y][2] = (dp[x][y][2] + add) % MODL;
					} else if (d == limit[x]) {
						for (int c = 0; c <= 2; c++) {
							dp[x][y][c] += dp[x + 1][y][c];
							if (dp[x][y][c] >= MODL) dp[x][y][c] -= MODL;
						}
					} else {
						ll add = (dp[x + 1][y][0] + dp[x + 1][y][1] + dp[x + 1][y][2]) % MODL;
						dp[x][y][0] = (dp[x][y][0] + add) % MODL;
					}

					dp[x][y][2] += dp[x][y - 1][2];
					dp[x][y][0] += dp[x][y - 1][0];
					dp[x][y][cmp(d, limit[y])] += dp[x][y - 1][1];

					for (int c = 0; c <= 2; c++) dp[x][y][c] %= MODL;
				}
			}

			for (int x = 0; x < m; x++) {
				dp[x][x][cmp(d, limit[x])] += 2;
				for (int c = 0; c <= 2; c++) dp[x][x][c] %= MODL;
			}

			for (int x = 0; x < m; x++) {
				ll add = (dp[x][m - 1][0] + dp[x][m - 1][1]) % MODL;
				if (x != 0) add = (add + dp[x][m - 1][2]) % MODL;
				ans[j][k] += add;
				ans[j][k] %= MODL;
			}
		}
	}
	return ans;
}

inline void solve() {
	int n; ll a, b;
	cin >> n >> a >> b;

	str digits;
	digits.reserve(n);
	for (int i = 0; i < n; i++) {
		char ch; cin >> ch; // skips whitespace, works for spaced or unspaced digits
		digits.pb(ch);
	}

	str B = to_string(b);
	auto rightAns = solveForLimit(B, digits);

	V<vl> leftAns(n, vl(n, 0));
	if (a > 0) {
		str A = to_string(a - 1);
		leftAns = solveForLimit(A, digits);
	}

	int q; cin >> q;
	while (q--) {
		int l, r; cin >> l >> r;
		--l; --r;
		ll res = (rightAns[l][r] - leftAns[l][r]) % MOD;
		if (res < 0) res += MOD;
		cout << res << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
