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
const i128 INF = (i128)BIG;

/*
 (1) Problem recap in simple terms:
     We have plants in a line. Canal i can be used k times to give k water to plants i and i+1,
     costing c_i*k. For each prefix 1..i, find the minimum total cost to meet all water needs.

 (2) Things to notice and step-by-step derivation:
     - Let x_i be how many times we use canal i. Then plant 1 gets x_1, plant i gets x_{i-1},
       and middle plant j gets x_{j-1}+x_j.
     - A DP over the "current rightmost x" needs queries like min over all prev >= (w - x).
       This is a suffix-min, and the (w - x) makes the axis flip each step.
     - The DP function is piecewise-linear and convex. We keep it as segments in a deque, plus a
       lazy shift applied to all segment values.
     - Each step does: add the linear cost c*x, then enforce the new plant's requirement and flip
       direction for the next transition.

 (3) Solution outline:
     - Keep segments (len, val) in a deque; tot is total len, lazy shifts all vals, and sumDiff
       tracks the current "difference" so we can update the minimum start fast.
     - addCost(p) applies canal cost by shifting lazy (sign depends on direction) and
       updating sumDiff.
     - applyNeed(w) trims the function down to size w, flips direction, and restores valid signs.
     - After processing plant i (i>=2), start is the minimum cost for plants 1..i, so we print it.
     - Time: O(N). Memory: O(N).
 */

inline void solve() {
	int n;
	cin >> n;
	vl w(n), c(n - 1);
	for (int i = 0; i < n; i++) cin >> w[i];
	for (int i = 0; i < n - 1; i++) cin >> c[i];

	struct Seg {
		i128 len;
		i128 val;
	};

	deque<Seg> dq;
	i128 tot = INF;
	i128 start = 0;
	i128 sumDiff = INF * INF;
	i128 lazy = 0;
	int revDir = 0;

	auto toStr = [&](i128 x) -> str {
		if (x == 0) return "0";
		bool neg = x < 0;
		if (neg) x = -x;
		str s;
		for (; x > 0; x /= 10) s.pb(char('0' + int(x % 10)));
		if (neg) s.pb('-');
		reverse(all(s));
		return s;
	};

	auto applyNeed = [&](ll needLl) {
		i128 need = (i128)needLl;

		if (!revDir) {
			Seg last{-1, -1};
			while (tot > need && sz(dq)) {
				last = dq.bk;
				dq.ppb();

				if (last.val + lazy <= 0) {
					revDir = 1;
					start += sumDiff;
					sumDiff = 0;
					tot = INF;
					lazy = 0;
					dq.clear();
					dq.pb({INF, 0});
					return;
				}

				tot -= last.len;
				sumDiff -= last.len * (last.val + lazy);
			}

			if (tot < need) {
				dq.pb({need - tot, last.val});
				sumDiff += (need - tot) * (last.val + lazy);
			}

			revDir = 1;
			start += sumDiff;
			sumDiff = -sumDiff;
			tot = need;

			while (sz(dq) && dq.ft.val + lazy < 0) {
				tot -= dq.ft.len;
				sumDiff += dq.ft.len * (dq.ft.val + lazy);
				dq.ppf();
			}

			dq.pf({INF, -lazy});
			tot += INF;
		} else {
			Seg last{-1, -1};
			while (tot > need && sz(dq)) {
				last = dq.ft;
				dq.ppf();

				if (last.val + lazy >= 0) {
					revDir = 0;
					start += sumDiff;
					sumDiff = 0;
					tot = INF;
					lazy = 0;
					dq.clear();
					dq.pb({INF, 0});
					return;
				}

				tot -= last.len;
				sumDiff += last.len * (last.val + lazy);
			}

			if (tot < need) {
				dq.pf({need - tot, last.val});
				sumDiff -= (need - tot) * (last.val + lazy);
			}

			revDir = 0;
			start += sumDiff;
			sumDiff = -sumDiff;
			tot = need;

			while (sz(dq) && dq.bk.val + lazy > 0) {
				tot -= dq.bk.len;
				sumDiff -= dq.bk.len * (dq.bk.val + lazy);
				dq.ppb();
			}

			dq.pb({INF, -lazy});
			tot += INF;
		}
	};

	auto addCost = [&](ll p) {
		i128 pp = (i128)p;
		if (!revDir) lazy += pp;
		else lazy -= pp;
		sumDiff += pp * tot;
	};

	dq.pb({INF, INF});
	tot = INF;
	sumDiff = INF * INF;
	start = 0;
	revDir = 0;

	applyNeed(w[0]);
	for (int i = 1; i < n; i++) {
		addCost(c[i - 1]);
		applyNeed(w[i]);
		cout << toStr(start) << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
