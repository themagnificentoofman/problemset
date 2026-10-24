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
     Find the minimum time to travel from a to b for each query, either by walking directly |a-b|
     or by using one slingshot (x,y,t): |a-x| + t + |b-y|.

 (2) Things to notice and step-by-step derivation:
     - Split slingshots by whether x is on the left or right of a (two sweeps); then |a-x| becomes
       a linear term in a and x.
     - For a fixed sweep, each slingshot contributes a value of form (t ± x ± y), and each query
       needs a minimum over y in either y<=b (prefix) or y>=b (suffix).
     - Coordinate-compress all y/b values and maintain two segment trees of these best values.

 (3) Solution outline:
     - Read slingshots and queries as events; coordinate-compress all positions used for y/b.
     - Do two sweeps over events (one for each side of x vs a), inserting slingshots into two
       segment trees and answering each query with 2 range minimum queries.
     - Take the minimum over both sweeps and the direct walk for each query.
     - Time: O((n+q) log(n+q)), Memory: O(n+q)
 */

struct MinSeg {
	int n;
	vl seg;
	MinSeg(int n) : n(n), seg(2 * n, BIG) {}

	ll get(int i) { return seg[n + i]; }

	void updMin(int i, ll v) {
		i += n;
		seg[i] = min(seg[i], v);
		for (i >>= 1; i; i >>= 1) seg[i] = min(seg[2 * i], seg[2 * i + 1]);
	}

	ll query(int l, int r) { // [l, r)
		ll res = BIG;
		for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
			if (l & 1) res = min(res, seg[l++]);
			if (r & 1) res = min(res, seg[--r]);
		}
		return res;
	}
};

inline void solve() {
	int n, q;
	cin >> n >> q;

	V<AR<int, 4>> ev;
	vi coords;
	coords.pb(0);
	coords.pb(INT_MAX);

	for (int i = 0; i < n; i++) {
		int x, y, t;
		cin >> x >> y >> t;
		coords.pb(x), coords.pb(y);
		ev.pb({0, x, y, t});
	}
	for (int i = 0; i < q; i++) {
		int a, b;
		cin >> a >> b;
		coords.pb(a), coords.pb(b);
		ev.pb({1, a, b, i});
	}

	sor(coords);
	coords.erase(unique(all(coords)), end(coords));

	auto id = [&](int x) -> int { return lwb(coords, x); };

	vl ans(q, BIG);

	for (int pass = 0; pass < 2; pass++) {
		sort(all(ev), [&](const AR<int, 4>& x, const AR<int, 4>& y) -> bool {
			int v1 = x[1], v2 = y[1];
			if (pass == 0) v1 *= -1, v2 *= -1;
			if (v1 != v2) return v1 < v2;
			return x[0] < y[0]; // slingshots (0) before queries (1)
		});

		MinSeg pref(sz(coords)), suff(sz(coords));

		for (auto e : ev) {
			int tp = e[0], a = e[1], b = e[2], c = e[3];
			int bi = id(b);

			if (tp == 0) {
				ll add1 = pass ? (ll)c - a - b : (ll)c + a - b; // for y>=b
				ll add2 = pass ? (ll)c - a + b : (ll)c + a + b; // for y<=b
				pref.updMin(bi, add1);
				suff.updMin(bi, add2);
			} else {
				ll res1 = min(BIG, pref.query(0, bi + 1));
				ll res2 = min(BIG, suff.query(bi, sz(coords)));

				if (pass == 0) {
					res1 += (ll)b - a;
					res2 -= (ll)a + b;
				} else {
					res1 += (ll)a + b;
					res2 += (ll)a - b;
				}

				ll direct = llabs((ll)a - b);
				ans[c] = min(ans[c], min(direct, min(res1, res2)));
			}
		}
	}

	for (int i = 0; i < q; i++) cout << ans[i] << "\n";
}

int main() {
	setIO("slingshot");
	solve(); // single case
}
