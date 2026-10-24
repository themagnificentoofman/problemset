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
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // ind of 1st elem >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // ind of 1st elem > b
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
const ll BIG = 4e18; // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple terms):
   Points (x,y) “activate” over time. At each time t, we want the minimum value of:
     (|cx - X| + |cy - Y|) + floor(sqrt(cx^2 + cy^2))
   where (cx,cy) must lie in a gap between two consecutive blocks of the current Pareto staircase.

2) Things to notice / derivation:
   - Maintain the Pareto frontier (staircase) as merged axis-aligned blocks. Neighbor blocks define one
     feasible “gap rectangle” for (cx,cy): cx in [L.xR, R.xL], cy in [L.yR, R.yL].
   - For a gap, the best (cx,cy) is just (X,Y) clamped into those intervals; then add floor(sqrt(cx^2+cy^2)).
   - When inserting a new point, it may violate the staircase order, so we merge with neighbors until valid.
     Only adjacency relationships change, so we can maintain all candidate gap-costs in a multiset.

3) Solution outline:
   - Store frontier as multiset of merged blocks (ordered lexicographically).
   - Store gap costs between adjacent blocks in a multiset; answer is its minimum.
   - For each insertion: erase affected neighbor costs, merge blocks, insert new costs.
   - Time: O((N+T) log N), memory O(N).
*/

static inline ll sq(ll x) { return x * x; }

static inline ll sqrt_safe(ll x) {
	ll r = (ll)sqrt((db)x);
	while ((r + 1) * (r + 1) <= x) ++r;
	while (r * r > x) --r;
	return r;
}

struct Block {
	pi xr; // [xL, xR]
	pi yr; // [yL, yR]
};

static inline bool operator<(const Block& a, const Block& b) {
	if (a.xr != b.xr) return a.xr < b.xr;
	return a.yr < b.yr;
}

inline void solve() {
	int N, T, X, Y;
	cin >> N >> T >> X >> Y;

	multiset<Block> bad;
	multiset<ll> cands;

	auto clamp_into = [&](ll& base, const pi& rng, ll& coord) -> void {
		if (coord < rng.fi) { base += (ll)rng.fi - coord; coord = rng.fi; }
		if (coord > rng.se) { base += coord - (ll)rng.se; coord = rng.se; }
	};

	auto get_cost = [&](const Block& L, const Block& R) -> ll {
		pi x_rng{L.xr.se, R.xr.fi};
		pi y_rng{L.yr.se, R.yr.fi};
		ll cx = X, cy = Y;
		ll ans = 0;
		clamp_into(ans, x_rng, cx);
		clamp_into(ans, y_rng, cy);
		ans += sqrt_safe(sq(cx) + sq(cy));
		return ans;
	};

	auto add_cost = [&](const Block& L, const Block& R, int sgn) -> void {
		ll c = get_cost(L, R);
		if (sgn == +1) cands.insert(c);
		else {
			auto it = cands.find(c);
			if (it != end(cands)) cands.erase(it);
		}
	};

	auto touch_adj = [&](multiset<Block>::iterator it, int sgn) -> void {
		bool has_prev = (it != bg(bad));
		bool has_next = (next(it) != end(bad));
		if (has_prev) add_cost(*prev(it), *it, sgn);
		if (has_next) add_cost(*it, *next(it), sgn);
		if (has_prev && has_next) add_cost(*prev(it), *next(it), -sgn);
	};

	auto merge_range = [&](pi& a, const pi& b) -> void {
		a.fi = min(a.fi, b.fi);
		a.se = max(a.se, b.se);
	};
	auto merge_block = [&](Block& a, const Block& b) -> void {
		merge_range(a.xr, b.xr);
		merge_range(a.yr, b.yr);
	};

	// A <= B in lexicographic order. If not strictly SW, they must merge.
	auto intersect = [&](const Block& A, const Block& B) -> bool {
		return !(A.xr.se <= B.xr.fi && A.yr.se <= B.yr.fi);
	};

	auto insert_point = [&](int x, int y) -> void {
		Block p{{x, x}, {y, y}};
		while (true) {
			auto it = bad.lb(p);

			if (it != end(bad) && intersect(p, *it)) {
				touch_adj(it, -1);
				merge_block(p, *it);
				bad.erase(it);
				continue;
			}
			if (it != bg(bad)) {
				auto it2 = prev(it);
				if (intersect(*it2, p)) {
					touch_adj(it2, -1);
					merge_block(p, *it2);
					bad.erase(it2);
					continue;
				}
			}
			break;
		}
		auto it = bad.insert(p);
		touch_adj(it, +1);
	};

	// Sentinels so every feasible gap is between two adjacent blocks.
	const int INF = 0x3f3f3f3f;
	insert_point(0, 0);
	insert_point(INF, INF);

	V<vpi> updates(T);
	for (int i = 0; i < N; i++) {
		int s, x, y;
		cin >> s >> x >> y;
		updates[s].pb({x, y});
	}

	for (int t = 0; t < T; t++) {
		for (auto [x, y] : updates[t]) insert_point(x, y);
		cout << *bg(cands) << "\n";
	}
}

int main() {
	setIO();
	solve(); // single case
}
