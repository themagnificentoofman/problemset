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
     We have barns at positions x[i]. For each query (a,b), delivering at position y costs
     a * sum_{x[i]<y}(y-x[i]) + b * sum_{x[i]>y}(x[i]-y). Find the minimum cost.

 (2) Things to notice and step-by-step derivation:
     - After sorting x, the cost as a function of y is convex (slopes only increase), so the
       minimum over allowed y is unimodal.
     - If we only consider y equal to some barn position, then duplicates would create flat
       stretches; using distinct positions avoids ambiguity for "first increasing" search.
     - With prefix sums, we can compute left/right distance sums to a chosen x[k] in O(1).

 (3) Solution outline:
     - Sort x and build prefix sums.
     - Build a list of indices for distinct x values.
     - For each query (a,b), define cost(idx) for delivering at x[uniq[idx]] in O(1).
     - Binary search the first idx where cost(idx) <= cost(idx+1), then output cost(idx).
     - Time: O(n log n + q log n); Memory: O(n).
 */

inline void solve() {
	int n; cin >> n;
	vl x(n);
	for (int i = 0; i < n; i++) cin >> x[i];
	sor(x);

	vl px(n + 1, 0);
	for (int i = 0; i < n; i++) px[i + 1] = px[i] + x[i];

	auto rangeSum = [&](int l, int r) -> ll {
		if (l > r) return 0;
		return px[r + 1] - px[l];
	};

	vi uniq;
	uniq.pb(0);
	for (int i = 1; i < n; i++) {
		if (x[i] != x[i - 1]) uniq.pb(i);
	}

	int q; cin >> q;
	while (q--) {
		ll a, b; cin >> a >> b;

		auto cost = [&](int idx) -> ll {
			int k = uniq[idx];
			ll y = x[k];

			ll leftDist = y * k - rangeSum(0, k - 1);
			ll rightDist = rangeSum(k + 1, n - 1) - y * (n - k - 1);

			return leftDist * a + rightDist * b;
		};

		int lo = 0, hi = sz(uniq) - 1;
		while (lo < hi) {
			int mid = lo + (hi - lo) / 2;
			if (cost(mid) <= cost(mid + 1)) hi = mid;
			else lo = mid + 1;
		}

		cout << cost(lo) << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
