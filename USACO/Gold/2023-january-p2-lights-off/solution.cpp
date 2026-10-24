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
     We have n lights in a circle (a bitmask). For each query, we are given a target mask x and
     a starting pad mask y that rotates by 1 each step. We want the smallest i such that after
     i steps, x is reachable from all-off, using the rotating pad and allowed moves.

 (2) Things to notice and step-by-step derivation:
     - Rotating the whole board doesn't change reachability, so we can group masks by rotation
       and store only a canonical representative for each rotation class.
     - There is a fixed "canonical" sequence of toggle masks that can be simulated for 0..3n
       steps; if we close each step under rotation classes, we cover all orientations we need.
     - For a query, we simulate the cumulative XOR from the rotating pad; at step i, the
       remaining difference is (cur ^ fin). If its rotation class is reachable in i canonical
       steps, then i is a valid answer.

 (3) Solution outline:
     - Precompute rep[mask] = canonical representative of mask's rotation class.
     - Precompute can[i][rep] for i = 0..3n, marking which rotation classes are reachable in
       exactly i canonical steps.
     - For each query, simulate cur for i = 0..3n and output the first i with
       can[i][rep[cur ^ fin]] true.
     - Time: O(3n * 2^n + t * n); Memory: O(3n * 2^n).
 */

inline int rot1(int x, int n) {
	x <<= 1;
	if (x & (1 << n)) x ^= (1 << n) ^ 1;
	return x;
}

inline int toMask(const str& s, int n) {
	int res = 0;
	for (int i = 0; i < n; i++) if (s[i] == '1') res |= (1 << i);
	return res;
}

inline void solve(int n, const vi& rep, const V<vb>& can) {
	str x, y;
	cin >> x >> y;

	int pad = toMask(y, n);
	int fin = toMask(x, n);
	int cur = 0;

	for (int i = 0; i <= 3 * n; i++) {
		if (can[i][rep[cur ^ fin]]) {
			cout << i << '\n';
			return;
		}
		cur ^= pad;
		pad = rot1(pad, n);
	}
}

int main() {
	setIO();
	int t, n; cin >> t >> n;

	int states = 1 << n;

	vi rep(states, -1);
	for (int i = 0; i < states; i++) {
		if (rep[i] != -1) continue;
		int j = i;
		while (rep[j] == -1) {
			rep[j] = i;
			j = rot1(j, n);
		}
	}

	V<vb> can(3 * n + 1, vb(states, false));
	can[0][rep[0]] = true;

	int pad = 0;
	for (int i = 1; i <= 3 * n; i++) {
		pad ^= 1 << ((i - 1) % n);
		for (int mask = 0; mask < states; mask++) {
			if (can[i - 1][rep[mask]]) can[i][rep[pad ^ mask]] = true;
		}
	}

	while (t--) solve(n, rep, can);
}
