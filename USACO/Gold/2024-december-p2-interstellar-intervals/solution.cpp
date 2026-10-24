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
template <class T1, class T2> using P = pair<T1, T2>;
#define mp make_pair
#define fi first
#define se second
using pi = P<int, int>;
using pl = P<ll, ll>;
using pd = P<db, db>;

// vectors
template <class T> using V = vector<T>;
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
template <class T, size_t SZ> using AR = array<T, SZ>;
template <class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template <class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
template <class T> using pq = priority_queue<T>;
template <class T> using pqg = priority_queue<T, V<T>, greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s) { freopen(s.c_str(), "r", stdin); }
	void setOut(string s) { freopen(s.c_str(), "w", stdout); }
	void setIO(string s = "") {
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if breaks things
		if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
	}
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = 4e18;
const int MOD = (int)1e9 + 7;
const int MXN = (int)5e5 + 5;

/*
(1) Problem recap
You have N points in a line, initially white. You may perform operations on an uncolored segment:
choose i and x, color points i..i+x-1 red and i+x..i+2x-1 blue (so you paint 2x consecutive points,
first half red, second half blue), and operations must be on disjoint points.
Given a string s of length N with characters 'R', 'B', or 'X' (meaning the point must end up red,
must end up blue, or can be anything), count the number of final colorings you can produce, modulo 1e9+7.

(2) Things to notice and step-by-step derivation
- Let dp[i] be the number of ways to finish coloring points 1..i.
- If s[i] == 'X', we can leave point i white: dp[i] += dp[i-1].
- Otherwise, point i must be blue (or at least not forced red) if we end an operation at i. An operation
  ending at i has even length 2x, so it transitions from dp[j] where j = i-2x and j has the same parity as i.
- For a fixed i, the valid j are “active” indices (not forbidden by any 'B' constraints) within a moving
  lower bound determined by the nearest forced 'R' to the left. We maintain parity-sums of dp[j] over
  active j in the current window.
- 'B' constraints create “bad intervals of j” whose left endpoints shift left by 1 each step; we
  simulate this by deactivating each j at most once using a moving frontier list (overall O(N) deactivations).

(3) Solution outline
- Scan i = 1..N; maintain:
  - dp[i] (mod),
  - isActive[j] for start indices j,
  - parSum[0/1] = sum of dp[j] over active j in the current window with given parity,
  - lp = left edge of the current window from the most recent 'R',
  - a list of moving frontiers that deactivate new j due to 'B'.
- Update deactivations for 'B', update lp based on the latest 'R', then compute dp[i] from dp[i-1] (if 'X')
  and parSum[i%2], and finally add dp[i] into parSum.
- Time complexity: O(N). Memory complexity: O(N).
*/

inline void solve() {
	int n;
	cin >> n;

	V<char> s(n + 1);
	vl dp(n + 1, 0);
	vb isActive(n + 1, true);

	ll parSum[2] = {0, 0};
	int lp = 0;
	int lastR = 0;
	vi front;

	dp[0] = 1;
	isActive[0] = true;
	parSum[0] = 1;

	for (int i = 1; i <= n; i++) {
		cin >> s[i];
		isActive[i] = true;

		if (i - 1 >= 1 && s[i - 1] == 'B') front.pb(i - 1);

		vi nextFront;
		for (int p : front) {
			int j = p - 1;
			if (j >= 0 && isActive[j]) {
				isActive[j] = false;
				if (j >= lp) {
					parSum[j & 1] = (parSum[j & 1] - dp[j] + MOD) % MOD;
				}
				nextFront.pb(j);
			}
		}
		front = move(nextFront);

		if (s[i] == 'R') lastR = i;

		int bound = max(2 * lastR - i - 1, 0);
		while (lp < bound) {
			parSum[lp & 1] = (parSum[lp & 1] - dp[lp] * (isActive[lp] ? 1LL : 0LL) + MOD) % MOD;
			++lp;
		}
		while (lp > bound) {
			--lp;
			parSum[lp & 1] = (parSum[lp & 1] + dp[lp] * (isActive[lp] ? 1LL : 0LL)) % MOD;
		}

		dp[i] = 0;
		if (s[i] == 'X') dp[i] = (dp[i] + dp[i - 1]) % MOD;
		dp[i] = (dp[i] + parSum[i & 1]) % MOD;

		parSum[i & 1] = (parSum[i & 1] + dp[i]) % MOD;
	}

	cout << dp[n] << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
