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
#define pp pop
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
     We have N axis-aligned boxes with integer coordinates. In mode M=2, for each box i we need to
     decide if it is "blocked" by some other active box using the rule in edgeOut(). In mode M=1,
     we must output an order to remove all boxes by repeatedly removing a box only when it is not
     blocked (and removing blockers first if needed).

 (2) Things to notice and step-by-step derivation:
     - For a fixed box i, the only thing we ever query is: among active boxes with x1 < x2[i],
       which one has the smallest y1, and does that y1 < y2[i]?
     - That suggests a segment tree over x1. Each active box contributes value (y1, id) at index x1,
       and a prefix query [0, x2[i]) returns the minimum y1 (and its id).
     - In mode M=1, when a box i is blocked by box y, we must remove y before i. If we always
       recurse/stack into the current blocker, each "blocked" check that fails immediately pushes a
       new box onto the stack. When a box becomes unblocked, we remove it and pop.

 (3) Solution outline:
     - Build a segment tree keyed by x1 that stores (min y1, id) for active boxes.
     - edgeOut(i): temporarily deactivate i, query prefix [0, x2[i]), and test y1 < y2[i].
     - If M=2: for i=0..N-1, output '1' if edgeOut(i)==-1 else '0', then permanently remove i.
     - If M=1: run an explicit stack DFS: at top x, if edgeOut(x) returns y, push y; otherwise
       remove x and append it to the answer.
     - Time: O(N log N) segment ops (amortized). Memory: O(N).
 */

struct S {
	int y;
	int label;
};

static inline S op(const S& a, const S& b) {
	if (a.y != b.y) return (a.y < b.y ? a : b);
	return (a.label < b.label ? a : b);
}

static inline S e() {
	return {(int)1e9, -1};
}

struct Segtree {
	int n = 1, sz_ = 0;
	V<S> d;
	Segtree() {}
	Segtree(int n_) { init(n_); }

	void init(int n_) {
		sz_ = n_;
		n = 1;
		while (n < sz_) n <<= 1;
		d.assign(2 * n, e());
	}

	void setVal(int p, S x) {
		d[p += n] = x;
		for (p >>= 1; p >= 1; p >>= 1) d[p] = op(d[p << 1], d[p << 1 | 1]);
	}

	S prod(int l, int r) {
		if (l < 0) l = 0;
		if (r > sz_) r = sz_;
		if (l >= r) return e();
		S vl = e(), vr = e();
		for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
			if (l & 1) vl = op(vl, d[l++]);
			if (r & 1) vr = op(d[--r], vr);
		}
		return op(vl, vr);
	}
};

inline void solve(int mode) {
	int n;
	cin >> n;

	V<AR<AR<int, 2>, 2>> boxes(n);
	for (auto &bx : boxes) {
		for (auto &pt : bx) {
			for (auto &v : pt) {
				cin >> v;
				--v;
			}
		}
	}

	vi x1(n), y1(n), x2(n), y2(n);
	for (int i = 0; i < n; i++) {
		x1[i] = boxes[i][0][0];
		y1[i] = boxes[i][0][1];
		x2[i] = boxes[i][1][0];
		y2[i] = boxes[i][1][1];
	}

	int lim = 2 * n + 5;
	Segtree seg(lim);

	auto rem = [&](int id) -> void {
		seg.setVal(x1[id], e());
	};
	auto add = [&](int id) -> void {
		seg.setVal(x1[id], S{y1[id], id});
	};

	auto edgeOut = [&](int id) -> int {
		rem(id);
		S best = seg.prod(0, x2[id]);
		int ret = (best.y < y2[id] ? best.label : -1);
		add(id);
		return ret;
	};

	for (int i = 0; i < n; i++) add(i);

	if (mode == 1) {
		V<char> active(n, 1);
		V<char> onStack(n, 0);
		vi ans;
		ans.reserve(n);

		for (int i = 0; i < n; i++) {
			if (!active[i]) continue;

			vi st;
			st.pb(i);
			onStack[i] = 1;

			while (!st.empty()) {
				int x = st.bk;

				int y = edgeOut(x);
				if (y == -1) {
					active[x] = 0;
					onStack[x] = 0;
					rem(x);
					ans.pb(x);
					st.ppb();
					continue;
				}

				if (!active[y]) continue;
				if (onStack[y]) {
					// Should not happen on valid inputs (no cycles in the blocking relation).
					// Still, avoid infinite loops in local testing.
					active[y] = 0;
					rem(y);
					onStack[y] = 0;
					ans.pb(y);
					continue;
				}

				st.pb(y);
				onStack[y] = 1;
			}
		}

		for (int i = 0; i < n; i++) {
			if (i) cout << ' ';
			cout << ans[i] + 1;
		}
		cout << '\n';
	} else {
		str ans;
		ans.reserve(n);
		for (int i = 0; i < n; i++) {
			int x = edgeOut(i);
			ans += (x == -1 ? '1' : '0');
			rem(i);
		}
		cout << ans << '\n';
	}
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	int m; cin >> m;
	while (t--) solve(m);
}
