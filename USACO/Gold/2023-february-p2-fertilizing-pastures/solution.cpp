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
     We walk a rooted tree starting at node 1. When we first visit a node, its fertilizer rate
     turns on forever. Each edge walk takes 1 time, so total fertilizer is (active rate sum)
     integrated over time. We choose child order; if t=1 we may skip returning on one root-leaf
     path. Output total edges walked and the minimum total fertilizer.

 (2) Things to notice and step-by-step derivation:
     - A child subtree behaves like a "job": it takes p = 2 * subSz time to fully explore and
       come back, and after that its total rate sumW is fully active for the rest of the tour.
     - While we explore a child, already-finished subtrees keep producing fertilizer, so the
       waiting cost at a node is sum(p_child * sumW_done_before).
     - The best order of child-jobs is by increasing (sumW / subSz) using cross-multiplication.
     - For t=1, the no-return path must go through a deepest child; if that child is visited
       last, we can remove one return edge per level (save depth edges).

 (3) Solution outline:
     - Build the rooted tree (parents are guaranteed to be smaller than children).
     - Bottom-up (u=n..1): compute subSz[u], sumW[u], dp0[u], save[u]; sort children by
       increasing sumW/subSz and add the local waiting contributions to dp0[u].
     - Also compute dp1[u] by trying each deepest child v as the last child and switching
       dp0[v] -> dp1[v], adjusting only by prefix sums before v in the sorted order.
     - If t=0: edges=2(n-1), cost=dp0[1]. If t=1: edges=2(n-1)-save[1], cost=dp1[1].
     - Time: O(n log n); Memory: O(n).
 */

inline void solve() {
	int n, t; cin >> n >> t;

	V<vi> child(n + 1);
	vl sumW(n + 1, 0), subSz(n + 1, 0);
	vl dp0(n + 1, 0), dp1(n + 1, 0);
	vi save(n + 1, 0);

	for (int i = 2; i <= n; i++) {
		int p; ll r;
		cin >> p >> r;
		child[p].pb(i);
		sumW[i] = r;
	}

	for (int u = n; u >= 1; u--) {
		subSz[u] = 1;
		dp0[u] = 0;
		dp1[u] = 0;
		save[u] = 0;

		for (int v : child[u]) {
			subSz[u] += subSz[v];
			sumW[u] += sumW[v];
			dp0[u] += dp0[v];
			save[u] = max(save[u], save[v] + 1);
		}

		sort(all(child[u]), [&](int a, int b) {
			i128 lhs = (i128)sumW[a] * (i128)subSz[b];
			i128 rhs = (i128)sumW[b] * (i128)subSz[a];
			if (lhs != rhs) return lhs < rhs;
			return a < b;
		});

		ll currW = 0;
		for (int v : child[u]) {
			dp0[u] += (2LL * subSz[v]) * currW;
			currW += sumW[v];
		}
		dp0[u] += currW; // one "return edge" from each child back to u

		if (sz(child[u]) > 0) {
			dp1[u] = BIG;

			ll prefW = 0; // sumW of children before v
			ll prefP = 0; // sum (2*subSz) of children before v
			for (int v : child[u]) {
				if (save[v] + 1 == save[u]) {
					i128 cand = (i128)dp0[u]
						- (i128)(2LL * subSz[v]) * (i128)prefW
						+ (i128)prefP * (i128)sumW[v]
						- (i128)dp0[v] + (i128)dp1[v];
					dp1[u] = min(dp1[u], (ll)cand);
				}
				prefW += sumW[v];
				prefP += 2LL * subSz[v];
			}
		}
	}

	ll edges = 2LL * (n - 1);
	if (t == 0) {
		cout << edges << ' ' << dp0[1] << '\n';
	} else {
		cout << edges - (ll)save[1] << ' ' << dp1[1] << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
