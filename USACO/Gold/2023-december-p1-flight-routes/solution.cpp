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
     We are given, for every i<j, whether the number of increasing flight routes from i to j is
     odd or even. The true set of direct flights is uniquely determined; we must count them.

 (2) Things to notice and step-by-step derivation:
     - A non-direct route from i to j must end with some last hop k->j where i<k<j.
     - For a fixed last hop k->j, the number of routes i->j using that last hop equals the
       number of routes i->k (then append the edge k->j).
     - Working mod 2, parity(i->j, excluding direct i->j) =
       XOR over k in (i..j): (parity(i->k) AND direct(k->j)).
       Then direct(i->j) = parity(i->j) XOR parity_excluding_direct.

 (3) Solution outline:
     - Read the given parities parity[i][j] for all i<j.
     - Compute direct edges in order of i from n-1 down to 1, and j from i+1 up to n:
       use already-known direct[k][j] (k>i) to get parity_excluding_direct, then solve for
       direct[i][j].
     - Sum all direct[i][j] to get the answer.
     - Time: O(n^3); Memory: O(n^2).
 */

inline void solve() {
	int n; cin >> n;

	V<vi> parity(n, vi(n, 0));
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			char ch; cin >> ch; // works for both spaced and unspaced 0/1 input
			parity[i][j] = ch - '0';
		}
	}

	V<vi> direct(n, vi(n, 0));
	auto parityNoDirect = [&](int i, int j) -> int {
		int ret = 0;
		for (int k = i + 1; k < j; k++) ret ^= (parity[i][k] & direct[k][j]);
		return ret;
	};

	int ans = 0;
	for (int i = n - 1; i >= 0; i--) {
		for (int j = i + 1; j < n; j++) {
			// parity[i][j] = direct[i][j] XOR (routes that end with some k->j)
			direct[i][j] = parity[i][j] ^ parityNoDirect(i, j);
			ans += direct[i][j];
		}
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
