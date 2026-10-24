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
template<class T, size_t SZ> using AR = array<T,SZ>;
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

// constants
const db PI = acos((db)-1);
const ll NEG = -(1LL<<60);

/*
(1) Problem recap in simple terms
- We have n cows; each cow has weight w_i and talent t_i.
- We must pick a subset with total weight >= W to maximize floor(1000 * sum(t_i) / sum(w_i)).
- Output is an integer (the best ratio scaled by 1000).

(2) Things to notice and step-by-step derivation
- Maximizing a ratio is tricky; use the standard "binary search on the answer" trick.
- Suppose the target ratio is y/1000. For any subset S, define adjusted value:
      val(S) = sum(1000*t_i - y*w_i) over i in S.
  If there exists a subset with weight >= W and val(S) >= 0, then the true answer >= y.
- So feasibility(y) reduces to a 0/1 knapsack:
    dp[k] = max val achievable with EXACT total weight k, but we "cap" any k >= W into one bucket W.
- Transition: for each item (w, t) with value = 1000*t - y*w,
    for k from W..0:
      nk = min(W, k + w)
      dp[nk] = max(dp[nk], dp[k] + value)
- If dp[W] >= 0 after processing all items, y is feasible.

(3) Solution outline
- Binary search y in integer space [0, 1e6] (since t_i <= 1000 and w_i >= 1 ⇒ ratio*1000 ≤ 1e6).
- For each mid = y, run the capped-weight knapsack to test feasibility.
- Maintain invariant: lo feasible, hi infeasible. Answer is lo.
*/

inline void solve() {
	int n, W; 
	cin >> n >> W;

	vi w(n), t(n);
	for (int i = 0; i < n; ++i) cin >> w[i] >> t[i];

	/* Feasibility check for a candidate ratio y (scaled by 1000).
	   Returns true if there exists a subset with total weight >= W
	   whose adjusted sum sum(1000*t_i - y*w_i) >= 0. */
	auto feasible = [&](int y) -> bool {
		vl dp(W+1, NEG);
		dp[0] = 0; // base: no items, weight 0, adjusted value 0

		for (int i = 0; i < n; ++i) {
			ll val = 1000LL * t[i] - 1LL * y * w[i]; // adjusted value for this cow
			int wi = w[i];

			// 0/1 knapsack in descending k to avoid reuse within same iteration.
			for (int k = W; k >= 0; --k) if (dp[k] != NEG) {
				int nk = k + wi;
				if (nk > W) nk = W; // cap all weights >= W into index W
				dp[nk] = max(dp[nk], dp[k] + val);
			}
		}
		return dp[W] >= 0; // feasible if we can reach weight >= W with non-negative adjusted value
	};

	// Binary search for the maximum feasible y (answer is in [0, 1e6]).
	int l = 0, r = 1'000'000 + 1; // invariant: l feasible, r infeasible
	while (r - l > 1) {
		int mid = l + (r - l + 1) / 2;
		feasible(mid) ? l = mid : r = mid;
	}

	cout << l << '\n';
}

int main() {
	setIO("talent");
	solve(); // single case
}
