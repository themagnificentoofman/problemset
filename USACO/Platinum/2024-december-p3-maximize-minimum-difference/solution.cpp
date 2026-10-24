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
const int MAXN = 2005;

/*
 (1) Problem recap in simple terms:
     We look at permutations p of [0..N-1] that maximize the minimum |p[i]-p[i+1]|. Given K fixed
     constraints p[i]=j, count how many of those “best” permutations satisfy all constraints.

 (2) Things to notice and step-by-step derivation:
     - The maximum possible minimum difference is floor(N/2).
     - If N is even, there are exactly two best permutations (a fixed zig-zag pattern and its
       reverse), so we only need to check the constraints against both.
     - If N is odd, best permutations have a structured form centered at the middle value N/2.
       Fixing where the middle/extremes go splits the remaining positions into independent parts.
     - Each part becomes a “place numbers in increasing layers” counting problem, which can be
       computed using binomials and powers of 2, while validating fixed constraints.

 (3) Solution outline:
     - Precompute factorials and inverse factorials for nCk modulo MOD.
     - For even N: compare constraints to the 2 fixed best permutations.
     - For odd N: iterate all valid placements of (N/2, N-1, 0) that are consistent with fixed
       constraints, and multiply the counts of the remaining independent parts.
     - Use helper counters (type1/type2) that scan fixed constraints, sort by “layer”, and apply
       nCk choices as the interval shrinks.
     - Time complexity: O(N^2 log N). Memory complexity: O(N).
 */

static int n;
static int pPos[MAXN]; // pPos[i] = value at position i, or -1 if unknown
static ll fact[MAXN], invFact[MAXN];

static ll modPow(ll a, ll e = MOD - 2) {
	ll r = 1 % MOD;
	a %= MOD;
	for (; e > 0; e >>= 1) {
		if (e & 1) r = (r * a) % MOD;
		a = (a * a) % MOD;
	}
	return r;
}

static ll nCk(int nn, int kk) {
	if (kk < 0 || kk > nn) return 0;
	return fact[nn] * invFact[kk] % MOD * invFact[nn - kk] % MOD;
}

static inline void mirrorValues() {
	for (int i = 0; i < n; i++) {
		if (pPos[i] != -1) pPos[i] = (n - 1) - pPos[i];
	}
}

static ll type1Count(int l, int r, int k) {
	vpi v;
	v.reserve(r - l + 1);

	for (int i = l; i <= r; i++) {
		if (pPos[i] == -1) continue;

		if (pPos[i] == n / 2) return 0;

		if (i & 1) {
			if (pPos[i] > n / 2) return 0;
			if (pPos[i] >= k - n / 2) return 0;
			v.pb({pPos[i] + n / 2, i});
		} else {
			if (pPos[i] < n / 2) return 0;
			if (pPos[i] > k) return 0;
			v.pb({pPos[i], i});
		}
	}
	sor(v);

	if (!sz(v)) return modPow(2, (r - l) / 2);

	ll ans = 1;
	int curr = n / 2;

	for (int i = 0; i + 1 < sz(v); i++) {
		if (v[i].fi == v[i + 1].fi) {
			if (abs(v[i + 1].se - v[i].se) != 1) return 0;

			if (v[i].se % 2 == l % 2) {
				int num = v[i].fi - curr;
				int s = (v[i].se - l) / 2 + 1;
				ans = ans * nCk(num - 1, s - 1) % MOD;
				curr = v[i].fi;
				l += 2 * s;
				r -= 2 * (num - s);
			} else {
				int num = v[i].fi - curr;
				int s = (r - v[i].se) / 2 + 1;
				ans = ans * nCk(num - 1, s - 1) % MOD;
				curr = v[i].fi;
				l += 2 * (num - s);
				r -= 2 * s;
			}

			i++;
			if (i == sz(v) - 1) return ans * modPow(2, (r - l) / 2) % MOD;
			continue;
		}

		if (v[i].se < l || v[i].se > r) return 0;

		if (v[i + 1].se > v[i].se) {
			int num = v[i].fi - curr;
			int s = (v[i].se - l) / 2 + 1;
			ans = ans * nCk(num - 1, s - 1) % MOD;
			curr = v[i].fi;
			l += 2 * s;
			r -= 2 * (num - s);
		} else {
			int num = v[i].fi - curr;
			int s = (r - v[i].se) / 2 + 1;
			ans = ans * nCk(num - 1, s - 1) % MOD;
			curr = v[i].fi;
			l += 2 * (num - s);
			r -= 2 * s;
		}
	}

	if (v.bk.se > r || v.bk.se < l) return 0;

	if (v.bk.fi == k) {
		return ans * nCk((r - l) / 2, (v.bk.se - l) / 2) % MOD;
	}

	ll ways = (nCk(v.bk.fi - curr - 1, (v.bk.se - l) / 2)
	          + nCk(v.bk.fi - curr - 1, (r - v.bk.se) / 2)) % MOD;

	ans = ans * ways % MOD;
	return ans * modPow(2, k - v.bk.fi - 1) % MOD;
}

static ll type2Count(int l, int r, int k) {
	vpi v;

	for (int i = l; i < k - 1; i++) {
		if (pPos[i] == -1) continue;
		if (pPos[i] == n / 2) return 0;

		if (i % 2 == l % 2) {
			if (pPos[i] < n / 2) return 0;
			v.pb({pPos[i], i});
		} else {
			if (pPos[i] > n / 2) return 0;
			v.pb({pPos[i] + n / 2, i});
		}
	}
	for (int i = r; i < n; i++) {
		if (pPos[i] == -1) continue;
		if (pPos[i] == n / 2) return 0;

		if (i % 2 == r % 2) {
			if (pPos[i] < n / 2) return 0;
			v.pb({pPos[i], i});
		} else {
			if (pPos[i] > n / 2) return 0;
			v.pb({pPos[i] + n / 2, i});
		}
	}

	sor(v);

	ll ans = 1;
	int curr = n / 2;

	for (int i = 0; i < sz(v); i++) {
		if (i > 0 && v[i].fi == v[i - 1].fi) {
			if (v[i].se - v[i - 1].se != 1) return 0;
			if (v[i].se < k) {
				if (v[i].se % 2 == l % 2) return 0;
			} else {
				if (v[i].se % 2 == r % 2) return 0;
			}
			continue;
		}

		if (v[i].se < l || (v[i].se >= k && v[i].se < r)) return 0;

		if (v[i].se < k) {
			int num = v[i].fi - curr;
			int s = (v[i].se - l) / 2 + 1;
			ans = ans * nCk(num - 1, s - 1) % MOD;
			curr = v[i].fi;
			l += 2 * s;
			r += 2 * (num - s);
		} else {
			int num = v[i].fi - curr;
			int s = (v[i].se - r) / 2 + 1;
			ans = ans * nCk(num - 1, s - 1) % MOD;
			curr = v[i].fi;
			l += 2 * (num - s);
			r += 2 * s;
		}
	}

	ans = ans * nCk((k - 2 - l + 1) / 2 + (n - 1 - r + 1) / 2, (k - 2 - l + 1) / 2) % MOD;
	return ans;
}

inline void solve() {
	int k;
	cin >> k;

	for (int i = 0; i < n; i++) pPos[i] = -1;

	for (int i = 0; i < k; i++) {
		int a, b;
		cin >> a >> b;
		assert(pPos[a] == -1);
		pPos[a] = b;
	}

	if (n % 2 == 0) {
		vi best(n, -1);
		for (int i = 0; i < n; i++) {
			if (i % 2 == 0) best[i] = n / 2 + i / 2;
			else best[i] = i / 2;
		}

		int ok1 = 1, ok2 = 1;
		for (int i = 0; i < n; i++) {
			if (pPos[i] == -1) continue;
			if (pPos[i] != best[i]) ok1 = 0;
			if (pPos[i] != best[n - i - 1]) ok2 = 0;
		}
		cout << ok1 + ok2 << "\n";
		return;
	}

	ll ans = 0;

	for (int it = 0; it < 2; it++) {
		if (pPos[0] == n / 2 || pPos[0] == -1) {
			if (pPos[1] == 0 || pPos[1] == -1) {
				ans = (ans + type1Count(2, n - 1, n - 1)) % MOD;
			}
			if (pPos[1] == n - 1 || pPos[1] == -1) {
				mirrorValues();
				ans = (ans + type1Count(2, n - 1, n - 1)) % MOD;
				mirrorValues();
			}
		}

		for (int i = 1; i < n - 1; i++) {
			if (pPos[i] != n / 2 && pPos[i] != -1) continue;
			if (pPos[i - 1] != n - 1 && pPos[i - 1] != -1) continue;
			if (pPos[i + 1] != 0 && pPos[i + 1] != -1) continue;

			int keepL = (pPos[i - 1] == n - 1);
			int keepM = (pPos[i] == n / 2);
			int keepR = (pPos[i + 1] == 0);

			pPos[i - 1] = n - 1;
			pPos[i] = n / 2;
			pPos[i + 1] = 0;

			if (i % 2 == 1) {
				ans = (ans + type2Count(0, i + 2, i)) % MOD;
			} else {
				ll a = type1Count(i + 2, n - 1, n - i / 2 - 1);

				mirrorValues();
				ll b = type1Count(0, i - 2, n / 2 + i / 2);
				mirrorValues();

				ans = (ans + a * b) % MOD;
			}

			if (!keepL) pPos[i - 1] = -1;
			if (!keepM) pPos[i] = -1;
			if (!keepR) pPos[i + 1] = -1;
		}

		reverse(pPos, pPos + n);
	}

	cout << ans % MOD << "\n";
}

inline void init() {
    fact[0] = 1;
	for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * i % MOD;
	invFact[n] = modPow(fact[n]);
	for (int i = n - 1; i >= 0; i--) invFact[i] = invFact[i + 1] * (i + 1) % MOD;
}

int main() {
	setIO();
	int t;
	cin >> t >> n;
    init();
	while (t--) solve();
}
