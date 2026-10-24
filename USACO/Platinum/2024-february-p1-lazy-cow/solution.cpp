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
     Each minute, we can produce a tests for cost 3^(a-1). For each demand (m, b),
     by minute m we must have produced at least b total tests. After each new
     demand arrives, output the minimum total energy needed so far, mod 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - If we must produce dy tests over dx minutes, the cheapest way is to spread
       tests as evenly as possible because 3^(a-1) grows very fast.
     - The full requirement "tests by time" can be viewed as points (m, b). The
       tight constraints form an upper convex hull; points below the hull never
       matter.
     - When the hull changes, the answer changes only on edges next to the
       inserted/removed point, so we can maintain the total by local updates.

 (3) Solution outline:
     - Maintain the upper hull of points (x, y) with x increasing using a set of
       x-coordinates, and store y for each x.
     - Keep ans = sum of segmentCost(prevX, curX) over consecutive hull points.
     - For each new demand, insert/update its point, delete any now-redundant
       hull points, and print ans.
     - Time: O(D log D) amortized. Memory: O(D).
 */

struct mi {
	int v;
	mi(ll v_ = 0) {
		v_ %= MOD;
		if (v_ < 0) v_ += MOD;
		v = int(v_);
	}

	explicit operator int() const { return v; }

	friend ostream& operator<<(ostream& os, const mi& a) { return os << a.v; }
	friend istream& operator>>(istream& is, mi& a) {
		ll x; is >> x;
		a = mi(x);
		return is;
	}

	friend bool operator==(const mi& a, const mi& b) { return a.v == b.v; }
	friend bool operator!=(const mi& a, const mi& b) { return a.v != b.v; }

	mi& operator+=(const mi& o) {
		v += o.v;
		if (v >= MOD) v -= MOD;
		return *this;
	}
	mi& operator-=(const mi& o) {
		v -= o.v;
		if (v < 0) v += MOD;
		return *this;
	}
	mi& operator*=(const mi& o) {
		v = int((ll)v * o.v % MOD);
		return *this;
	}

	friend mi operator+(mi a, const mi& b) { return a += b; }
	friend mi operator-(mi a, const mi& b) { return a -= b; }
	friend mi operator*(mi a, const mi& b) { return a *= b; }

	mi pow(ll e) const {
		mi a = *this, r = 1;
		while (e > 0) {
			if (e & 1) r *= a;
			a *= a;
			e >>= 1;
		}
		return r;
	}
	mi inv() const { return pow(MOD - 2); }
	mi& operator/=(const mi& o) { return (*this) *= o.inv(); }
	friend mi operator/(mi a, const mi& b) { return a /= b; }
};

inline void solve() {
	int d; cin >> d;

	mi ans = 0;
	map<int, ll> yAt;
	set<int> hullX;

	yAt[0] = 0;
	hullX.ins(0);

	auto badMiddle = [&](int x1, int x2, int x3) -> bool {
		// true if point (x2, y2) is not needed on the upper hull.
		return (yAt[x2] - yAt[x1]) * (ll)(x3 - x2) <= (yAt[x3] - yAt[x2]) * (ll)(x2 - x1);
	};

	auto segCost = [&](int x1, int x2) -> mi {
		int dx = x2 - x1;
		ll dy = yAt[x2] - yAt[x1];
		if (dy <= 0) return 0;

		ll k = dy / dx;
		int r = int(dy % dx);

		// r minutes with (k+1) tests, (dx-r) minutes with k tests.
		// cost(a tests in one minute) = 3^(a-1), and 0 tests costs 0.
		if (r) {
			mi hi = mi(3).pow(k); // k+1 tests -> 3^k
			mi lo = (k ? mi(3).pow(k - 1) : mi(0));
			return hi * r + lo * (dx - r);
		} else {
			return (k ? mi(3).pow(k - 1) * dx : mi(0));
		}
	};

	auto applyPoint = [&](int x, bool del) -> void {
		auto it = hullX.lb(x);
		assert(it != hullX.end() && *it == x);
		assert(it != hullX.begin());

		int xPrev = *prev(it);
		if (del) {
			ans -= segCost(xPrev, x);
			auto itN = next(it);
			if (itN != hullX.end()) {
				int xNext = *itN;
				ans += segCost(xPrev, xNext) - segCost(x, xNext);
			}
			hullX.erase(x);
			yAt[x] = 0;
		} else {
			ans += segCost(xPrev, x);
			auto itN = next(it);
			if (itN != hullX.end()) {
				int xNext = *itN;
				ans += segCost(x, xNext) - segCost(xPrev, xNext);
			}
		}
	};

	auto insertPoint = [&](int x, ll y) -> void {
		if (yAt[x] && yAt[x] >= y) return; // old point is stronger
		if (yAt[x]) applyPoint(x, true);

		yAt[x] = y;
		hullX.ins(x);
		applyPoint(x, false);

		for (;;) {
			auto it = hullX.lb(x);
			bool changed = 0;

			for (int rep = 0; rep < 3; rep++) {
				if (it != hullX.end() && next(it) != hullX.end() && next(next(it)) != hullX.end()) {
					int a = *it, b = *next(it), c = *next(next(it));
					if (badMiddle(a, b, c)) {
						changed = 1;
						applyPoint(b, true);
						break;
					}
				}
				if (it == hullX.begin()) break;
				--it;
			}

			if (!changed) break;
		}
	};

	for (int i = 0; i < d; i++) {
		int m; ll b;
		cin >> m >> b;
		insertPoint(m, b);
		cout << ans << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
