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
   We have hay bales on a number line. We can start an explosion at any position with power R.
   It triggers bales within distance R; those bales explode next with power R-1, etc.
   Find the minimum R to explode all bales (answer can be .5).

2) Things to notice + step-by-step:
   - Scale positions by 2 so half-steps become integers; power then decreases by 2 each “hop”.
   - For a fixed power R, whether we can reach the left end from a start position is monotone in start position,
     and similarly for reaching the right end.
   - So for each R, we can:
       (a) find the rightmost start position that can still reach the leftmost bale,
       (b) check if that same start can also reach the rightmost bale.

3) Solution outline:
   - Binary search R (scaled by 2).
   - For each R, binary search the start position s, using a greedy simulation to see if we can reach the left end.
   - Then greedily simulate to the right from that s.
   - Time: O(logC * (logC * J)), where J is the number of “hops” (<= N in practice); overall fast enough.
*/

static inline bool reachLeft(ll pos, ll p, vl& x) {
	// greedy: always jump to the leftmost bale we can still reach
	while (true) {
		if (pos - p <= x[0]) return true;
		ll need = pos - p;
		int idx = lwb<ll>(x, need); // leftmost bale >= need
		if (idx >= sz(x)) return false;
		ll nxt = x[idx];
		if (nxt >= pos) return false; // no progress left
		pos = nxt;
		p -= 2;
		if (p < 0) return false;
	}
}

static inline bool reachRight(ll pos, ll p, vl& x) {
	// greedy: always jump to the rightmost bale we can still reach
	while (true) {
		if (pos + p >= x.bk) return true;
		ll need = pos + p;
		int idx = upb<ll>(x, need) - 1; // rightmost bale <= need
		if (idx < 0) return false;
		ll nxt = x[idx];
		if (nxt <= pos) return false; // no progress right
		pos = nxt;
		p -= 2;
		if (p < 0) return false;
	}
}

inline void solve() {
	int n;
	cin >> n;

	vl x(n);
	for (int i = 0; i < n; i++) {
		cin >> x[i];
		x[i] *= 2; // scale by 2 to avoid .5
	}
	sor(x);

	auto can = [&](ll p) -> bool {
		ll lo = x[0], hi = x.bk;
		ll best = x[0] - 1;

		while (lo <= hi) {
			ll mid = lo + (hi - lo) / 2;
			if (reachLeft(mid, p, x)) {
				best = mid;
				lo = mid + 1;
			} else {
				hi = mid - 1;
			}
		}

		if (best < x[0]) return false;
		return reachRight(best, p, x);
	};

	ll lo = 0, hi = x.bk - x[0]; // scaled answer is within the total span
	while (lo < hi) {
		ll mid = lo + (hi - lo) / 2;
		if (can(mid)) hi = mid;
		else lo = mid + 1;
	}

	cout << fixed << setprecision(1) << (db)lo / 2.0 << "\n";
}

int main() {
	setIO("angry");
	solve(); // single case
}
