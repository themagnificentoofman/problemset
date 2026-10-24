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
     We start from a pair (a,b). One move is either a += b or b += a. For each query,
     find the minimum moves to reach (c,d), or say it's impossible.

 (2) Things to notice and step-by-step derivation:
     - Running the process backward looks like a "batched Euclid" reduction: undo many additions at once.
     - Zeros are special: if one value is 0, the next move forces it to become the other value.
     - Signs split the problem into cases (same sign / opposite sign / mixed); some cases reduce directly,
       and the hardest mixed-sign case is solved by intersecting two compressed step sequences.

 (3) Solution outline:
     - Normalize forced zero-moves on the start and target pairs (if possible).
     - If the pair is in an easy sign case, do batched Euclid-style reductions and count moves.
     - Otherwise (mixed signs), build two lists of reachable "ranges" (as arithmetic progressions) for
       the forward/backward processes and find a compatible meeting point with minimum total cost.
     - O(log^2(max(|a|,|b|,|c|,|d|))) time per query, O(log(max(|a|,|b|,|c|,|d|))) memory.
 */

inline bool sgn(ll x) { return x > 0; }

struct StepRange {
	bool isFixed;
	ll cost, inc, st, en;

	StepRange(ll x, ll y) {
		isFixed = true;
		st = x;
		en = x;
		cost = y;
		inc = 0;
	}

	StepRange(ll a, ll b, ll c, ll d) {
		isFixed = false;
		st = a;
		inc = b;
		en = c;
		cost = d;
		if (st == en) {
			isFixed = true;
			inc = 0;
		}
	}
};

static ll compat(StepRange a, StepRange b, ll tryVal = -1) {
	if (a.isFixed != b.isFixed) {
		if (!a.isFixed) swap(a, b);
	}
	if (a.isFixed && b.isFixed) {
		if (tryVal >= 0 && tryVal != a.st) return -1;
		return (a.st == b.st ? a.st : -1);
	} else if (a.isFixed || b.isFixed) {
		bool didSwap = false;
		if (b.isFixed) {
			didSwap = true;
			swap(a, b);
		}
		if (tryVal >= 0 && a.st != tryVal) return -1;

		ll l = min(b.st, b.en), r = max(b.st, b.en);
		ll ret = -1;
		ll step = llabs(b.inc);
		if (a.st >= l && a.st <= r && (a.st % step) == (l % step)) ret = a.st;

		if (didSwap) swap(a, b);
		return ret;
	} else {
		ll aL = min(a.st, a.en), aR = max(a.st, a.en);
		ll bL = min(b.st, b.en), bR = max(b.st, b.en);

		if (tryVal >= 0) {
			if (aL % llabs(a.inc) != (tryVal % llabs(a.inc))) return -1;
			if (bL % llabs(b.inc) != (tryVal % llabs(b.inc))) return -1;
			if (tryVal < aL || tryVal < bL || tryVal > aR || tryVal > bR) return -1;
			assert(a.inc < 0);
			return tryVal;
		} else {
			if (llabs(a.inc) != llabs(b.inc)) return -1;
			if ((aL % llabs(a.inc)) != (bL % llabs(a.inc))) return -1;

			tryVal = min(aR, bR);
			if (tryVal < min(aL, bL)) return -1;
			return tryVal;
		}
	}
}

static ll solveOne(AR<ll, 2> x, AR<ll, 2> y) {
	ll cnt = 0;

	bool done = true;
	for (int i = 0; i < 2; i++) done &= (x[i] == y[i]);
	if (done) return 0;

	for (int i = 0; i < 2; i++) {
		if (x[i] == 0) {
			x[i] += x[1 - i];
			cnt++;
		}
	}
	if (cnt == 2) return -1;

	done = true;
	for (int i = 0; i < 2; i++) done &= (x[i] == y[i]);
	if (done) return cnt;

	int tmpY = 0;
	for (int i = 0; i < 2; i++) {
		if (y[i] == 0) {
			y[i] -= y[1 - i];
			cnt++;
			tmpY++;
		}
	}
	if (tmpY == 2) return -1;

	done = true;
	for (int i = 0; i < 2; i++) done &= (x[i] == y[i]);
	if (done) return cnt;

	// case 1: x has same sign in both coords
	if ((x[0] > 0) == (x[1] > 0)) {
		if (x[0] < 0) {
			for (int i = 0; i < 2; i++) x[i] = -x[i], y[i] = -y[i];
		}
		if (min(y[0], y[1]) <= 0) return -1;

		while (y[0] >= x[0] && y[1] >= x[1] && (x[0] + x[1]) != (y[0] + y[1]) &&
		       min(y[0], y[1]) > 0) {
			if (y[0] == y[1]) break;
			if (y[0] > y[1]) {
				ll can = (y[0] - max(x[0], y[1]) - 1) / y[1] + 1;
				y[0] -= y[1] * can;
				cnt += can;
			} else {
				ll can = (y[1] - max(x[1], y[0]) - 1) / y[0] + 1;
				y[1] -= y[0] * can;
				cnt += can;
			}
		}
		return (x[0] == y[0] && x[1] == y[1]) ? cnt : -1;
	}

	// case 2: opposite signs in x
	if (sgn(x[0]) != sgn(y[0]) && sgn(x[1]) != sgn(y[1])) return -1;

	if (sgn(x[0]) == sgn(y[0]) && sgn(x[1]) == sgn(y[1])) {
		if (x[0] < 0) {
			for (int i = 0; i < 2; i++) x[i] = -x[i], y[i] = -y[i];
		}
		assert(x[0] > 0 && y[0] > 0 && x[1] < 0 && y[1] < 0);

		x[1] = llabs(x[1]);
		y[1] = llabs(y[1]);
		while (y[0] <= x[0] && y[1] <= x[1] && (x[0] + x[1]) != (y[0] + y[1])) {
			if (x[0] == x[1]) break;
			if (x[0] > x[1]) {
				ll can = max(0LL, x[0] - max(x[1], y[0]) - 1) / x[1] + 1;
				x[0] -= x[1] * can;
				cnt += can;
			} else {
				ll can = max(0LL, x[1] - max(x[0], y[1]) - 1) / x[0] + 1;
				x[1] -= x[0] * can;
				cnt += can;
			}
		}
		return (x[0] == y[0] && x[1] == y[1]) ? cnt : -1;
	}

	// case 3: mixed; normalize so x0>0,y0>0,x1<0,y1>0
	if (sgn(x[0]) != sgn(y[0])) {
		swap(x[0], x[1]);
		swap(y[0], y[1]);
	}
	if (x[0] < 0) {
		for (int i = 0; i < 2; i++) x[i] = -x[i], y[i] = -y[i];
	}
	assert(x[0] > 0 && y[0] > 0 && x[1] < 0 && y[1] > 0);

	V<P<StepRange, StepRange>> posList;
	while (y[0] > 0 && y[1] > 0) {
		if (y[0] == y[1]) {
			posList.pb(mp(StepRange(y[0], cnt), StepRange(y[1], cnt)));
			break;
		} else if (y[0] > y[1]) {
			ll can = (y[0] - y[1] - 1) / y[1] + 1;
			StepRange l(y[0], -y[1], y[0] - y[1] * (can - 1), cnt);
			StepRange r(y[1], cnt);
			posList.pb(mp(l, r));
			y[0] -= y[1] * can;
			cnt += can;
		} else {
			ll can = (y[1] - y[0] - 1) / y[0] + 1;
			StepRange l(y[0], cnt);
			StepRange r(y[1], -y[0], y[1] - y[0] * (can - 1), cnt);
			posList.pb(mp(l, r));
			y[1] -= y[0] * can;
			cnt += can;
		}
	}

	V<P<StepRange, StepRange>> mixList;
	cnt = 0;
	while (x[0] > 0) {
		if (x[1] >= 0) {
			if (x[1] == 0) {
				cnt++;
				x[1] = x[0];
			}
			mixList.pb(mp(StepRange(x[0], cnt), StepRange(x[1], cnt)));
			break;
		}
		if (llabs(x[0]) == llabs(x[1])) {
			x[1] = x[0];
			cnt += 2;
			mixList.pb(mp(StepRange(x[0], cnt), StepRange(x[1], cnt)));
			break;
		}
		if (x[0] > llabs(x[1])) {
			ll can = (x[0] - llabs(x[1]) - 1) / llabs(x[1]) + 1;
			StepRange l(x[0], x[1], x[0] + x[1] * (can - 1), cnt + 1);
			StepRange r(x[1] + x[0], x[1], x[1] + x[0] + x[1] * (can - 1), cnt + 1);
			x[0] += x[1] * can;
			cnt += can;
			mixList.pb(mp(l, r));
		} else {
			ll can = (llabs(x[1]) - x[0] - 1) / x[0] + 1;
			x[1] += x[0] * can;
			cnt += can;
		}
	}

	ll bestAns = BIG;
	int ptr = 0;
	for (int i = 0; i < sz(posList); i++) {
		for (int j = ptr; j < sz(mixList); j++) {
			auto pos = posList[i];
			auto mix = mixList[j];

			ll posMin = pos.fi.en + pos.se.en;
			ll posMax = pos.fi.st + pos.se.st;
			ll mixMin = mix.fi.en + mix.se.en;
			ll mixMax = mix.fi.st + mix.se.st;

			if (posMin > mixMax) break;
			if (j == ptr && mixMin > posMax) {
				ptr++;
				continue;
			}

			int doFirst = pos.se.isFixed ? 1 : 0;
			if (doFirst) {
				swap(pos.fi, pos.se);
				swap(mix.fi, mix.se);
			}
			assert(pos.fi.isFixed);

			ll got = compat(pos.fi, mix.fi);
			if (got == -1) {
				if (doFirst) {
					swap(pos.fi, pos.se);
					swap(mix.fi, mix.se);
				}
				continue;
			}

			AR<ll, 2> meets{};
			meets[0] = pos.fi.st;

			ll meet = -1;
			if (!mix.fi.isFixed) {
				ll numIters = (meets[0] - mix.fi.st) / mix.fi.inc;
				meet = mix.se.st + numIters * mix.se.inc;
				got = compat(pos.se, mix.se, meet);
				if (got == -1) {
					if (doFirst) {
						swap(pos.fi, pos.se);
						swap(mix.fi, mix.se);
					}
					continue;
				}
				meets[1] = meet;
			} else {
				meet = compat(pos.se, mix.se);
				if (meet == -1) {
					if (doFirst) {
						swap(pos.fi, pos.se);
						swap(mix.fi, mix.se);
					}
					continue;
				}
				meets[1] = meet;
			}

			ll curCost = pos.fi.cost + mix.fi.cost;

			AR<ll, 2> moveAmt{};
			moveAmt[0] = moveAmt[1] = 0;
			if (pos.fi.inc != 0) moveAmt[0] = (meets[0] - pos.fi.st) / pos.fi.inc;
			else if (pos.se.inc != 0) moveAmt[0] = (meets[1] - pos.se.st) / pos.se.inc;

			if (mix.fi.inc != 0) moveAmt[1] = (meets[0] - mix.fi.st) / mix.fi.inc;
			else if (mix.se.inc != 0) moveAmt[1] = (meets[1] - mix.se.st) / mix.se.inc;

			curCost += moveAmt[0] + moveAmt[1];
			bestAns = min(bestAns, curCost);

			if (doFirst) {
				swap(pos.fi, pos.se);
				swap(mix.fi, mix.se);
			}
		}
	}

	return (bestAns == BIG ? -1 : bestAns);
}

inline void solve() {
	ll a, b, c, d;
	cin >> a >> b >> c >> d;
	cout << solveOne({a, b}, {c, d}) << '\n';
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while(t--) solve();
}
