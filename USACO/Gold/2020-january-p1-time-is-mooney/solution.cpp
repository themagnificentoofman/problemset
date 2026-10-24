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
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b) - bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s){freopen(s.c_str(),"r",stdin);}
	void setOut(string s){freopen(s.c_str(),"w",stdout);}
	void setIO(string s = ""){
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if breaks things
		if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
	}
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

// problem-specific constants
const int MAX_DAYS = 1000; // we only need to simulate up to ~1000 days

/*
 (1) Problem recap in simple terms:
     There are N cities with "mooney" values and directed roads. Bessie starts at city 1,
     travels one edge per day, and earns the mooney of the city she arrives at each day.
     If she returns to city 1 after t days, she pays a cost c * t^2. We want the maximum
     possible profit (total mooney earned minus this quadratic cost).

 (2) Things to notice and step-by-step derivation:
     - Each extra day gives at most 1000 more mooney, but the penalty grows like t^2.
       Beyond about 1000 days, the penalty dominates any extra reward, so we can cap
       the number of days we care about at a small constant (~1000).
     - This suggests a DP over time and city: dp[t][i] = maximum total mooney we can
       have if we are at city i after exactly t days.
     - Transition: from dp[t][i], we move along any outgoing edge i -> j and update
       dp[t+1][j] with dp[t][i] + mooney[j].
     - Whenever we are at the starting city (city 1) on some day t, we can compute a
       candidate answer: dp[t][start] - c * t^2.

 (3) Solution outline:
     - Read graph and mooney values, convert cities to 0-based indices.
     - Initialize dp[0][0] = 0 (start at city 0 with 0 mooney) and all other states
       to -1 (unreachable).
     - For t from 0 to MAX_DAYS - 1, relax all reachable dp[t][i] to their neighbors
       on day t + 1.
     - For each t in [0, MAX_DAYS], if city 0 is reachable on day t, update the global
       answer with dp[t][0] - c * t * t.
     - Output the maximum answer.
*/

inline void solve() {
	int n, m, c;
	cin >> n >> m >> c;

	vi moonies(n);
	for (int i = 0; i < n; i++) cin >> moonies[i];

	V<vi> adj(n);
	for (int i = 0; i < m; i++) {
		int a, b;
		cin >> a >> b;
		--a; --b;              // convert to 0-based
		adj[a].pb(b);          // directed edge a -> b
	}

	// dp[t][i] = max total mooney when at city i after exactly t days
	// -1 means unreachable
	V<vi> dp(MAX_DAYS + 1, vi(n, -1));
	dp[0][0] = 0;              // start at city 0 (original city 1) on day 0 with 0 mooney

	for (int t = 0; t < MAX_DAYS; t++) {
		for (int i = 0; i < n; i++) {
			if (dp[t][i] < 0) continue; // skip unreachable states

			// try taking any outgoing road i -> j and move to day t + 1
			for (int j : adj[i]) {
				int cand = dp[t][i] + moonies[j];
				if (cand > dp[t + 1][j]) dp[t + 1][j] = cand;
			}
		}
	}

	ll ans = 0; // doing nothing (0 days, 0 reward, 0 cost) yields profit 0

	for (int t = 0; t <= MAX_DAYS; t++) {
		if (dp[t][0] < 0) continue; // must be able to come back to city 0 on day t
		ll profit = (ll)dp[t][0] - 1LL * c * t * t;
		if (profit > ans) ans = profit;
	}

	cout << ans << '\n';
}

int main() {
	setIO("time");
	solve(); // single case
}
