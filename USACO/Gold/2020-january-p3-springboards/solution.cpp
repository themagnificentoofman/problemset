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
		cin.exceptions(cin.failbit);
		if (sz(s)) setIn(s+".in"), setOut(s+".out");
	}
}

// ----- global constants only -----
const db PI = acos((db)-1);
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
 (1) problem recap in simple terms:
     We walk from (0,0) to (n,n) on a grid. Moving right or up costs 1. Some springboards
     teleport us from (x1,y1) to (x2,y2) for free. We want the minimum walking distance.
 (2) things to notice and step-by-step derivation:
     - Without springboards, the cost is always 2n.
     - Let dp[i] be the min walk to reach board i's start (x1,y1). If board j ends at
       (x2,y2) with y2 ≤ y1, using j then walking to (x1,y1) costs:
         dp[j] + (x1 - x2) + (y1 - y2)
       so:
         dp[i] = x1 + y1 + min over j with y2 ≤ y1 of (dp[j] - x2 - y2).
     - Sort all board endpoints by x (then y) and sweep. Maintain a structure over y
       storing minimal (dp - x - y) for processed ends, supporting:
         * query best value for y' ≤ y,
         * insert new (y, value) while deleting dominated points.
 (3) solution outline:
     - Build "start" and "end" events for each board and sort by (x,y).
     - Map<int,ll> best: key = y, value = minimal (dp - x - y) among ends at that y,
       maintained as a lower envelope (no dominated entries).
     - Start event: dp[i] = x + y + best_value_for_y≤y.
       End event: insert (y, dp[i] - x - y) into best, pruning worse later entries.
     - After all events, best.rbegin()->second gives the best (dp - x - y) up to y ≤ n,
       so answer = 2n + that value.
*/

// type alias for an event: ((x, y), (index, type)), type: 0 = start, 1 = end
using BoardEvent = P<pi,pi>;

// insert (y, v) into map while keeping only non-dominated points
inline void insertState(map<int,ll> &best, int y, ll v) {
	auto it = prev(best.ub(y)); // largest key <= y
	if (it->se <= v) return;    // existing value already better or equal
	++it;                       // now smallest key > y
	// erase later entries that are strictly worse (value > v)
	while (it != best.end() && it->se > v) {
		it = best.erase(it);
	}
	best[y] = v;
}

inline void solve() {
	int n, p;
	cin >> n >> p;

	// best[y] = minimal (dp - x - y) for processed board ends with that y
	map<int,ll> best;
	best[0] = 0LL; // base state: at (0,0) with cost 0

	vl dp(p);                 // dp[i] = min distance to start of board i
	V<BoardEvent> events;     // all start/end events
	events.reserve(2 * p);

	for (int i = 0; i < p; ++i) {
		int x1, y1, x2, y2;
		cin >> x1 >> y1 >> x2 >> y2;
		events.pb({{x1, y1}, {i, 0}}); // start event for board i
		events.pb({{x2, y2}, {i, 1}}); // end event for board i
	}

	sor(events); // sort by x, then y

	for (auto &e : events) {
		int x = e.fi.fi;
		int y = e.fi.se;
		int idx = e.se.fi;
		int type = e.se.se;

		if (!type) {
			// starting board idx: query best value for y' <= y
			auto it = prev(best.ub(y));
			ll bestVal = it->se;
			dp[idx] = x + y + bestVal;
		} else {
			// ending board idx: we can now contribute to future starts
			ll v = dp[idx] - x - y;
			insertState(best, y, v);
		}
	}

	// best.rbegin()->second corresponds to best (dp - x - y) up to largest y
	ll ans = 2LL * n + best.rbegin()->se;
	cout << ans << '\n';
}

int main() {
	setIO("boards");
	solve(); // single case
}