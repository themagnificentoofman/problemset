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
     We have T test cases; the sample always passes, and each other test is correct with
     probability 1/2 independently on each submission. We may submit at most K times and
     can stop early after seeing how many tests the last submission passed. We want the
     maximum possible expected final score.

 (2) Things to notice and step-by-step derivation:
     - Let X be the random number of non-sample tests passed on one submission.
       Then X ~ Binomial(T-1, 1/2).
     - With a fixed "continue value" E, the best rule is: stop if X > E, else submit
       again (since if X <= E, continuing is at least as good as stopping).
     - For a given cross = floor(E), the update becomes linear:
       E' = P(X <= cross) * E + sum_{x > cross} P(X = x) * x
       because when X <= cross we keep the expected value E, otherwise we take x.
     - cross only changes when E crosses an integer, so while floor(E) is constant we can
       fast-forward many steps using the closed form of a linear recurrence.

 (3) Solution outline:
     - Subtract the always-passed sample, so T := T-1 and we work on X in [0..T].
     - Build the Binomial(T, 1/2) distribution with a Pascal-style DP, then prefix sums:
       prefProb[i] = P(X <= i), prefExp[i] = sum_{x<=i} P(X=x)*x.
     - Maintain E = optimal expected X with the remaining submissions; apply the transform
       E' = aE + b where a = prefProb[cross], b = totalExp - prefExp[cross].
     - Use binary search + closed form to jump as many steps as possible while floor(E)
       stays equal to cross, decrementing K accordingly.
     - Output E + 1 (add back the sample).
     - Time complexity O(T^2 + T log K), memory complexity O(T).
 */

static inline db jump(db a, db b, db E, ll x) {
	// E_{t+x} = a^x * E_t + b * (a^x - 1) / (a - 1)
	if (x == 0) return E;
	db ax = powl(a, (db)x);
	return ax * E + b * (ax - 1) / (a - 1);
}

inline void solve() {
	int T;
	ll K;
	cin >> T >> K;

	// Sample always passes, so optimize only over the remaining tests.
	T--;

	// p[j] = P(X = j), where X ~ Binomial(T, 1/2).
	vd p(T + 1, 0);
	p[0] = 1;
	for (int i = 1; i <= T; i++) {
		for (int j = i; j >= 0; j--) {
			db v = p[j] / 2;
			if (j) v += p[j - 1] / 2;
			p[j] = v;
		}
	}

	vd prefProb(T + 1, 0), prefExp(T + 1, 0);
	prefProb[0] = p[0];
	for (int i = 1; i <= T; i++) prefProb[i] = prefProb[i - 1] + p[i];

	prefExp[0] = 0;
	for (int i = 1; i <= T; i++) prefExp[i] = prefExp[i - 1] + p[i] * (db)i;

	db totalExp = prefExp[T];
	db E = 0;

	while (K > 0) {
		ll cross = (ll)floor(E);

		// Already at the maximum possible expected value for X.
		if (cross >= T) { E = (db)T; break; }

		db a = prefProb[cross];
		db b = totalExp - prefExp[cross];

		// Find max steps we can take while floor(E) stays equal to cross.
		ll lo = 1, hi = K;
		while (lo < hi) {
			ll mid = (lo + hi + 1) / 2;
			db Em = jump(a, b, E, mid);
			if ((ll)floor(Em) == cross) lo = mid;
			else hi = mid - 1;
		}

		E = jump(a, b, E, lo);
		K -= lo;
	}

	cout << fixed << setprecision(20) << (E + 1) << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
