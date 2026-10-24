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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   N rooms are in a circle, with a[i] cows in room i (sum a[i] = N).
   We choose K doors (rooms with exits). Cows walk clockwise to the nearest chosen door.
   Minimize total distance walked.

2) Things to notice + step-by-step:
   - If we “cut” the circle at the first door, the problem becomes linear on rooms [0..N-1]
     where a door must be at room 0.
   - If we place doors at i and then next door at j, cows in rooms [i..j-1] walk (r-i) steps,
     so cost(i,j) = sum_{r=i}^{j-1} a[r] * (r-i).
   - After rotating, we can do DP over door positions:
       dp[d][j] = min cost to cover rooms [0..j-1] using d doors, with the last segment ending at j.
     Transition: dp[d][j] = min_{i<j} dp[d-1][i] + cost(i,j).

3) Solution outline:
   - Try all N rotations as the “cut” (first door location).
   - For each rotation, precompute cost(i,j) in O(1) using prefix sums of a[r] and a[r]*r.
   - Run O(K*N^2) DP, take best over rotations.
   - Time: O(N^3*K) with small constants (N is small), effectively O(N * K * N^2).
*/

static inline ll costRange(const vl& prefA, const vl& prefAR, int i, int j) {
	// cost(i,j) = sum_{r=i}^{j-1} a[r]*(r-i)
	ll sumA = prefA[j] - prefA[i];
	ll sumAR = prefAR[j] - prefAR[i];
	return sumAR - (ll)i * sumA;
}

inline void solve() {
	int n, k;
	cin >> n >> k;

	vi a0(n);
	for (int i = 0; i < n; i++) cin >> a0[i];

	ll ans = BIG;

	for (int start = 0; start < n; start++) {
		vi a(n);
		for (int i = 0; i < n; i++) a[i] = a0[(start + i) % n];

		vl prefA(n + 1, 0), prefAR(n + 1, 0);
		for (int i = 0; i < n; i++) {
			prefA[i + 1] = prefA[i] + a[i];
			prefAR[i + 1] = prefAR[i] + (ll)a[i] * i;
		}

		V<vl> dp(k + 1, vl(n + 1, BIG));
		dp[1][0] = 0;
		for (int j = 1; j <= n; j++) dp[1][j] = costRange(prefA, prefAR, 0, j);

		for (int d = 2; d <= k; d++) {
			dp[d][0] = 0;
			for (int j = 1; j <= n; j++) {
				ll best = BIG;
				for (int i = 0; i < j; i++) {
					if (dp[d - 1][i] == BIG) continue;
					best = min(best, dp[d - 1][i] + costRange(prefA, prefAR, i, j));
				}
				dp[d][j] = best;
			}
		}

		ans = min(ans, dp[k][n]);
	}

	cout << ans << "\n";
}

int main() {
	setIO("cbarn2");
	solve(); // single case
}
