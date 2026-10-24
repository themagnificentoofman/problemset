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
const int LG = 60;
const ll LIMIT = (ll)1e18;

/*
 (1) Problem recap in simple terms:
     There are N cities. City i has a portal schedule of length T_i (a power of 2). If we are in
     city i on day t, we jump to c[i][t mod T_i]. For each query (v, t, Δ), starting at city v on
     day t, repeat Δ times: take the portal, then wait 1 day. Output the final city.

 (2) Things to notice and step-by-step derivation:
     - The true state is (city, time) where time = day mod T_city. Total states is sum(T_i) <= 1e5.
     - T_i are powers of 2, so we can group cities into "tiers" by their period length.
     - From a state, if we jump into a city with period <= current period, we stay in the same-or-
       lower tier. We can keep following these jumps until we either reach a strictly higher tier
       (larger period) or we get stuck in a loop that never reaches a higher tier.
     - Precompute firstHigher[state] = the first state whose city has a larger period, and how many
       days it takes. Also, if a jump stays in the same tier, record it as a 1-step edge for binary
       lifting inside that tier.

 (3) Solution outline:
     - Build arrays for portal transitions c[i][t].
     - For every state (i, t), compute firstHigher and the tier-internal 1-step edge via memo DFS.
     - Build binary lifting tables for tier-internal edges, but only when a higher-tier jump does
       not happen sooner than the lift length.
     - Answer each query by greedily taking higher-tier jumps when possible, then binary lifting
       inside the current tier, then a single step.
     - Time complexity: O((sum T_i) * LG + Q * LG). Memory complexity: O((sum T_i) * LG).
 */

static inline ll clampAdd(ll a, ll b) {
	if (a >= LIMIT || b >= LIMIT) return LIMIT;
	if (a > LIMIT - b) return LIMIT;
	return a + b;
}

struct Solver {
	int n, q;
	V<vi> nxt;            // nxt[city][t] = next city after taking portal at time t mod T_city
	vi per;               // per[city] = T_city
	V<vi> firstHigher;    // firstHigher[city][t] = next city with strictly larger period (or 0)
	V<vl> firstHigherDays;// days to reach firstHigher (valid if firstHigher != 0)

	V<V<AR<int, LG>>> up; // tier-internal binary lift: city
	V<V<AR<ll, LG>>> upD; // tier-internal binary lift: days

	V<V<char>> state;     // 0 = unvisited, 1 = in stack, 2 = done

	void calc(int city, int t) {
		if (state[city][t] == 2) return;
		if (state[city][t] == 1) return; // loop within <= tier => treat as no higher exit
		state[city][t] = 1;

		int baseLen = per[city];
		int laterCity = nxt[city][t];
		ll days = 1;

		while (laterCity != 0 && per[laterCity] <= baseLen) {
			int laterLen = per[laterCity];
			int laterT = int((t + (days % laterLen)) % laterLen);

			calc(laterCity, laterT);

			if (per[laterCity] == baseLen) {
				up[city][t][0] = laterCity;
				upD[city][t][0] = days;
			}

			int nh = firstHigher[laterCity][laterT];
			if (nh == 0) {
				laterCity = 0;
				break;
			}
			days = clampAdd(days, firstHigherDays[laterCity][laterT]);
			laterCity = nh;
		}

		if (laterCity != 0) {
			firstHigher[city][t] = laterCity;
			firstHigherDays[city][t] = days;
		}

		state[city][t] = 2;
	}

	void buildLift() {
		for (int k = 1; k < LG; k++) {
			ll need = (k >= 62 ? LIMIT : (1LL << k));
			for (int city = 1; city <= n; city++) {
				int len = per[city];
				for (int t = 0; t < len; t++) {
					/*
					 Only build k-lift if we won't be forced to jump to a higher tier earlier than
					 this length (or if no higher-tier jump exists at all).
					 */
					if (firstHigher[city][t] != 0 && firstHigherDays[city][t] <= need) continue;

					int midCity = up[city][t][k - 1];
					if (midCity == 0) continue;

					ll d1 = upD[city][t][k - 1];
					int midT = int((t + (d1 % len)) % len);

					int endCity = up[midCity][midT][k - 1];
					if (endCity == 0) continue;

					ll d2 = upD[midCity][midT][k - 1];
					ll tot = clampAdd(d1, d2);
					if (tot > LIMIT) continue;

					up[city][t][k] = endCity;
					upD[city][t][k] = tot;
				}
			}
		}
	}

	int stepOnce(int city, ll& day) {
		int t = int(day % per[city]);
		day++;
		return nxt[city][t];
	}

	int query(int city, ll day, ll dlt) {
		while (dlt > 0) {
			for (;;) {
				int t = int(day % per[city]);
				if (firstHigher[city][t] == 0) break;
				ll need = firstHigherDays[city][t];
				if (dlt < need) break;
				day += need;
				dlt -= need;
				city = firstHigher[city][t];
			}

			for (int k = LG - 1; k >= 0; k--) {
				int t = int(day % per[city]);
				int to = up[city][t][k];
				if (to == 0) continue;
				ll need = upD[city][t][k];
				if (dlt < need) continue;
				day += need;
				dlt -= need;
				city = to;
			}

			if (dlt > 0) {
				city = stepOnce(city, day);
				dlt--;
			}
		}
		return city;
	}
};

inline void solve() {
	Solver s;
	cin >> s.n >> s.q;

	s.per.assign(s.n + 1, 0);
	for (int i = 1; i <= s.n; i++) cin >> s.per[i];

	s.nxt.assign(s.n + 1, {});
	for (int i = 1; i <= s.n; i++) {
		s.nxt[i].rsz(s.per[i]);
		for (int t = 0; t < s.per[i]; t++) cin >> s.nxt[i][t];
	}

	s.firstHigher.assign(s.n + 1, {});
	s.firstHigherDays.assign(s.n + 1, {});
	s.up.assign(s.n + 1, {});
	s.upD.assign(s.n + 1, {});
	s.state.assign(s.n + 1, {});

	for (int i = 1; i <= s.n; i++) {
		int len = s.per[i];
		s.firstHigher[i].assign(len, 0);
		s.firstHigherDays[i].assign(len, 0);
		s.up[i].assign(len, AR<int, LG>{});
		s.upD[i].assign(len, AR<ll, LG>{});
		s.state[i].assign(len, 0);
		for (int t = 0; t < len; t++) {
			s.up[i][t].fill(0);
			s.upD[i][t].fill(0);
		}
	}

	for (int city = 1; city <= s.n; city++) {
		for (int t = 0; t < s.per[city]; t++) s.calc(city, t);
	}

	s.buildLift();

	for (int qq = 0; qq < s.q; qq++) {
		int v;
		ll t, dlt;
		cin >> v >> t >> dlt;
		cout << s.query(v, t, dlt) << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
