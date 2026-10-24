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
     For each query [l,r] and starting difference x (Bessie minus Elsie), simulate giving each haybale
     to the cow with less total (ties to Bessie) and output the final difference.

 (2) Things to notice and step-by-step derivation:
     - Track only the difference (Bessie - Elsie). Giving a haybale of size a changes the difference
       by +a if Bessie receives it, or -a if Elsie receives it.
     - The rule is based on the sign of the current difference: if diff <= 0, Bessie gets the next
       bale (ties to Bessie), otherwise Elsie gets it.
     - Using 2*values and setting x := 2x-1 turns the tie case into a strict sign check on an odd
       integer, so we can move boundaries with "< 0" and "> 0" cleanly.
     - Maintain four ordered sets of active queries by their current "key" value, with lazy shifts as
       we process each haybale, and union equal keys inside the same set.

 (3) Solution outline:
     - Read a[i], scale by 2. For each query, store x[i]=2x-1 and bucket by left/right endpoints.
     - Process i=1..N, inserting new queries into a small set based on the sign of x.
     - Move elements between small/big sets when they cross thresholds ±a[i], then apply lazy shifts
       and swap small sets to reflect the sign flip after processing a[i].
     - When reaching r, the answer is lazy + stored_key for the query's DSU representative.
     - Time complexity: O((N+Q) log Q). Memory complexity: O(Q).
 */

struct DSU {
	vi p;
	vl storVal;

	void init(int n) {
		p.assign(n, -1);
		storVal.assign(n, 0);
	}

	int get(int x) {
		if (p[x] < 0) return x;
		return p[x] = get(p[x]);
	}

	bool unite(int a, int b) {
		a = get(a), b = get(b);
		if (a == b) return 0;
		if (p[a] > p[b]) swap(a, b);
		p[a] += p[b];
		p[b] = a;
		return 1;
	}
};

struct LazySet {
	map<ll, int> repByVal; // key is (actualVal - lazy)
	ll lazy = 0;
};

static inline void eraseBeg(LazySet* s) {
	s->repByVal.erase(bg(s->repByVal));
}

static inline void eraseEnd(LazySet* s) {
	s->repByVal.erase(prev(s->repByVal.end()));
}

static inline P<ll,int> getBeg(LazySet* s) {
	auto it = bg(s->repByVal);
	return mp(it->fi + s->lazy, it->se);
}

static inline P<ll,int> getEnd(LazySet* s) {
	auto it = prev(s->repByVal.end());
	return mp(it->fi + s->lazy, it->se);
}

inline void solve() {
	int n;
	cin >> n;

	vl a(n + 1);
	for (int i = 1; i <= n; i++) {
		cin >> a[i];
		a[i] *= 2;
	}

	int q;
	cin >> q;

	vi l(q + 1), r(q + 1);
	vl x(q + 1);
	for (int i = 1; i <= q; i++) {
		cin >> l[i] >> r[i] >> x[i];
		x[i] = 2 * x[i] - 1; // make the "tie goes to Bessie" rule become a strict sign check
	}

	V<vi> leftQ(n + 2), rightQ(n + 2);
	for (int i = 1; i <= q; i++) {
		leftQ[l[i]].pb(i);
		rightQ[r[i]].pb(i);
	}

	DSU dsu;
	dsu.init(q + 1);

	V<LazySet*> insideSet(q + 1, nullptr);

	LazySet negSmallObj, posSmallObj, negBigObj, posBigObj;
	LazySet* negSmall = &negSmallObj;
	LazySet* posSmall = &posSmallObj;
	LazySet* negBig = &negBigObj;
	LazySet* posBig = &posBigObj;

	auto addVal = [&](LazySet* s, ll val, int rep) -> void {
		rep = dsu.get(rep);
		val -= s->lazy;

		auto it = s->repByVal.find(val);
		if (it == s->repByVal.end()) {
			s->repByVal[val] = rep;
			dsu.storVal[rep] = val;
			insideSet[rep] = s;
			return;
		}

		int curRep = it->se;
		if (curRep == rep) return;

		dsu.unite(curRep, rep);
		int newRep = dsu.get(rep);

		it->se = newRep;
		dsu.storVal[newRep] = val;
		insideSet[newRep] = s;
	};

	vl ans(q + 1, 0);

	for (int i = 1; i <= n; i++) {
		for (int qq : leftQ[i]) {
			int rep = dsu.get(qq);
			if (x[qq] > 0) addVal(posSmall, x[qq], rep);
			else addVal(negSmall, x[qq], rep);
		}

		while (sz(negSmall->repByVal)) {
			auto cur = getBeg(negSmall);
			if (cur.fi < -a[i]) {
				eraseBeg(negSmall);
				addVal(negBig, cur.fi, cur.se);
			} else break;
		}

		while (sz(posSmall->repByVal)) {
			auto cur = getEnd(posSmall);
			if (cur.fi > a[i]) {
				eraseEnd(posSmall);
				addVal(posBig, cur.fi, cur.se);
			} else break;
		}

		negSmall->lazy += a[i];
		posSmall->lazy -= a[i];
		swap(negSmall, posSmall);

		negBig->lazy += a[i];
		posBig->lazy -= a[i];

		while (sz(negBig->repByVal)) {
			auto cur = getEnd(negBig);
			if (cur.fi > -a[i]) {
				eraseEnd(negBig);
				addVal(negSmall, cur.fi, cur.se);
			} else break;
		}

		while (sz(posBig->repByVal)) {
			auto cur = getBeg(posBig);
			if (cur.fi < a[i]) {
				eraseBeg(posBig);
				addVal(posSmall, cur.fi, cur.se);
			} else break;
		}

		for (int qq : rightQ[i]) {
			int rep = dsu.get(qq);
			LazySet* s = insideSet[rep];
			ll res = s->lazy + dsu.storVal[rep];
			ans[qq] = res;
		}
	}

	for (int i = 1; i <= q; i++) {
		assert((ans[i] & 1LL) == 1LL);
		cout << (ans[i] + 1) / 2 << "\n";
	}
}

int main() {
	setIO();
	solve(); // single case
}
