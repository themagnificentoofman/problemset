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
     Cows and packages are placed on the number line at positions spaced by M inside given intervals.
     Each second, we can move exactly one cow by 1 unit left or right. If a cow reaches a package
     position, it picks it up. Find the minimum total time to pick up all packages.

 (2) Things to notice and step-by-step derivation:
     - All cows/packages lie on arithmetic progressions with step M, so each interval is a fixed
       residue (x mod M) plus an integer "layer" floor(x / M).
     - If we sort residues around the circle [0, M), the travel cost between adjacent residues is
       just the gap length, with a small (state-dependent) multiplier.
     - At each residue, there is a small local min-cost transition between 5 boundary states; this
       can be represented as a 5x5 min-plus matrix.
     - A segment tree over the circularly duplicated residues lets us multiply these matrices over
       any arc quickly as events activate/deactivate residues over time layers.
     - Long stretches of empty layers repeat the same full-cycle matrix, so we exponentiate it with
       binary exponentiation.

 (3) Solution outline:
     - Read intervals; sort by residue (L mod M), duplicate by +M, and store consecutive gaps.
     - Create "events" on layer t = floor(x / M): interval start activates a residue, interval end
       deactivates it; process events in increasing t.
     - Maintain a segment tree of 5x5 matrices; query arcs between consecutive events and multiply
       into the running answer, plus exponentiate full-cycle jumps when t skips.
     - Update the segment tree at the event residue (and its duplicate) to reflect activation state.
     - Time: O((N+P) log (N+P)); Memory: O((N+P)).
 */

using DP = AR<AR<ll, 5>, 5>;

static inline void fillInf(DP& a) {
	for (int i = 0; i < 5; i++) for (int j = 0; j < 5; j++) a[i][j] = BIG;
}

static inline DP idDP() {
	DP a; fillInf(a);
	for (int i = 0; i < 5; i++) a[i][i] = 0;
	return a;
}

static inline DP dpTrans(const DP& a, const DP& b) {
	DP c; fillInf(c);
	for (int i = 0; i < 5; i++) {
		for (int k = 0; k < 5; k++) {
			if (a[i][k] >= BIG) continue;
			for (int j = 0; j < 5; j++) {
				if (b[k][j] >= BIG) continue;
				c[i][j] = min(c[i][j], a[i][k] + b[k][j]);
			}
		}
	}
	return c;
}

struct SegTree {
	int n;
	V<DP> st;
	vl* pos;

	SegTree(int n_, vl& pos_) : n(n_), st(4 * n_), pos(&pos_) {}

	void buildLeaf(int idx, int x, DP& a) {
		fillInf(a);

		if (!x) {
			a[0][4] = a[0][0] = a[1][4] = a[1][1] = 0;
			a[4][2] = a[2][2] = a[4][3] = a[3][3] = 0;
		} else if (x & 1) {
			a[4][4] = a[2][1] = a[3][0] = a[2][4] = a[4][0] = 0;
		} else {
			for (int i = 0; i < 5; i++) a[i][i] = 0;
		}

		ll len = (*pos)[idx + 1] - (*pos)[idx];
		for (int i = 0; i < 5; i++) {
			for (int k = 0; k < 4; k++) {
				if (a[i][k] >= BIG) continue;
				ll add = len * (1 + (k & 1));
				a[i][k] = min(BIG, a[i][k] + add);
			}
		}
	}

	void upd(int idx, int x, int i, int l, int r) {
		if (l == r) {
			buildLeaf(idx, x, st[i]);
			return;
		}
		int m = (l + r) >> 1;
		if (idx <= m) upd(idx, x, i << 1, l, m);
		else upd(idx, x, i << 1 | 1, m + 1, r);
		st[i] = dpTrans(st[i << 1], st[i << 1 | 1]);
	}

	void upd(int idx, int x) {
		upd(idx, x, 1, 0, n - 1);
	}

	DP qry(int ql, int qr, int i, int l, int r) {
		if (qr < l || r < ql) return idDP();
		if (ql <= l && r <= qr) return st[i];
		int m = (l + r) >> 1;
		return dpTrans(qry(ql, qr, i << 1, l, m), qry(ql, qr, i << 1 | 1, m + 1, r));
	}

	DP qry(int l, int r) {
		return qry(l, r, 1, 0, n - 1);
	}
};

inline void solve() {
	ll m;
	int n, p;
	cin >> m >> n >> p;

	int tot = n + p;
	vl l(tot), r(tot);
	vpl ps(tot);

	for (int i = 0; i < tot; i++) {
		cin >> l[i] >> r[i];
		ps[i] = mp(l[i] % m, (ll)i);
	}
	sor(ps);

	vl pos(2 * tot + 1);
	for (int i = 0; i < tot; i++) {
		pos[i] = ps[i].fi;
		pos[tot + i] = ps[i].fi + m;
	}
	pos[2 * tot] = pos[0] + 2 * m;

	V<AR<ll, 3>> es(2 * tot + 1);
	for (int i = 0; i < tot; i++) {
		int id = (int)ps[i].se;
		es[i << 1] = AR<ll, 3>{l[id] / m, (ll)i, (ll)(id < n)};
		es[i << 1 | 1] = AR<ll, 3>{r[id] / m + 1, (ll)i, 2};
	}
	es[2 * tot] = AR<ll, 3>{-1, 0, 0};
	sor(es);

	SegTree st(2 * tot, pos);
	for (int i = 0; i < 2 * tot; i++) st.upd(i, 2);

	DP ans; fillInf(ans);
	ans[4][4] = 0;

	for (int i = 1; i <= 2 * tot; i++) {
		ll tPrev = es[i - 1][0], tCur = es[i][0];
		int idxPrev = (int)es[i - 1][1];
		int idxCur = (int)es[i][1];

		int rIdx = idxCur - 1 + (tPrev < tCur ? tot : 0);
		ans = dpTrans(ans, st.qry(idxPrev, rIdx));

		ll skip = tCur - tPrev - 1;
		if (skip > 0) {
			DP b = st.qry(idxCur, idxCur - 1 + tot);
			for (; skip; skip >>= 1) {
				if (skip & 1) ans = dpTrans(ans, b);
				b = dpTrans(b, b);
			}
		}

		int x = (int)es[i][2];
		st.upd(idxCur, x);
		st.upd(idxCur + tot, x);
	}

	cout << ans[4][4] << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
