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
     Apples land at (t, x). Cow groups appear at (t, x), can move at speed 1, and each cow catches
     at most one apple (then leaves). Find the maximum number of apples that can be caught.

 (2) Things to notice and step-by-step derivation:
     - A cow at (tc, xc) can catch an apple at (ta, xa) iff ta - tc >= |xa - xc|.
     - This is equivalent to two inequalities:
       - xa - ta <= xc - tc   and   xa + ta >= xc + tc.
     - Let u = x - t and v = x + t. Then catch is possible iff u_apple <= u_cow and v_apple >= v_cow.
     - If we process events by increasing u, then when we see a cow, all eligible apples by u are
       already seen; we only need to pick one with v >= v_cow, best done greedily with the smallest v.

 (3) Solution outline:
     - Sort all events by u = x - t increasing; on ties, process apples before cows.
     - Maintain a map keyed by v = x + t storing how many apples are waiting at each v.
     - For a cow group with v0, repeatedly take apples from the smallest key >= v0 until cows run out.
     - Time: O(N log N); Memory: O(N).
 */

struct Event {
	int q;    // 1 = cows, 2 = apples
	ll t, x;
	int cnt;
};

inline void solve() {
	int n;
	cin >> n;

	V<Event> ev(n);
	for (int i = 0; i < n; i++) cin >> ev[i].q >> ev[i].t >> ev[i].x >> ev[i].cnt;

	sort(all(ev), [&](const Event& a, const Event& b) {
		ll ua = a.x - a.t, ub = b.x - b.t;
		if (ua != ub) return ua < ub;
		return a.q > b.q; // apples first when u ties
	});

	map<ll, ll> apples; // v -> count
	ll ans = 0;

	for (auto &e : ev) {
		ll v = e.x + e.t;
		if (e.q == 2) {
			apples[v] += e.cnt;
			continue;
		}

		int need = e.cnt;
		for (; need > 0;) {
			auto it = apples.lb(v);
			if (it == apples.end()) break;

			ll take = min<ll>(need, it->se);
			ans += take;
			need -= (int)take;

			it->se -= take;
			if (it->se == 0) apples.erase(it);
		}
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
