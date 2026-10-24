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
     We have two programs (Bessie and Elsie), each a string of instructions. We interleave them
     (keeping each program's internal order) and execute from 0, building an expression. We
     need the number of distinct final expressions over all interleavings (mod 1e9+7).

 
 (2) Things to notice and step-by-step derivation:
     - Multiplying by 1 never changes the expression, so we can delete all '1' instructions.
     - Multiplying by 0 resets everything to 0, so only the part after the last '0' matters.
     - After cleaning, we only care about instruction *type*: '+' vs '×d' (for any d != 0,1),
       so we map all remaining digits to a single symbol (say '2').
     - Two interleavings give the same expression iff we can swap adjacent same-type ops that
       belong to different cows to transform one into the other. So each equivalence class has
       a unique "canonical" interleaving that avoids an adjacent pattern (Elsie then Bessie)
       of the same type.

 
 (3) Solution outline:
     - Read and normalize both program strings: drop '1', cut prefix before last '0',
       map every non-'+' digit to '2'.
     - DP on prefixes: dp[i][j][k] = number of canonical interleavings using first i of A and
       first j of B, where k=0 means last op came from A, k=1 means last op came from B.
     - Always can append Elsie's next op. Can append Bessie's next op unless last was Elsie and
       the two types would become an invalid adjacent (same-type) pair.
     - Sum dp at the full lengths to get the answer.
     - Overall time and memory complexity: O(|A||B|) time, O(|A||B|) memory.
 */

inline void modAdd(int& a, int b) {
	a += b;
	if (a >= MOD) a -= MOD;
}

inline void readProg(str& s) {
	str raw; cin >> raw;
	s.clear();
	for (char c : raw) {
		if (c == '1') continue;
		if (c == '0') s.clear();
		if (c != '+') c = '2';
		s += c;
	}
}

inline void solve() {
	int n; cin >> n;
	str a, b;
	readProg(a);
	readProg(b);

	int na = sz(a), nb = sz(b);
	V<V<AR<int, 2>>> dp(na + 1, V<AR<int, 2>>(nb + 1, {0, 0}));

	auto upd = [&](int i, int j, int k, int v) {
		if (i <= na && j <= nb) modAdd(dp[i][j][k], v);
	};

	dp[0][0][0] = 1;
	int ans = 0;

	for (int i = 0; i <= na; i++) {
		for (int j = 0; j <= nb; j++) {
			for (int k = 0; k <= 1; k++) {
				int v = dp[i][j][k];
				if (!v) continue;

				if (i == na && j == nb) {
					modAdd(ans, v);
					continue;
				}

				// Append Elsie's next instruction.
				upd(i, j + 1, 1, v);

				// Append Bessie's next instruction if it keeps the interleaving canonical.
				if (k == 0) {
					upd(i + 1, j, 0, v);
				} else {
					if (i < na && j > 0 && b[j - 1] != a[i]) upd(i + 1, j, 0, v);
				}
			}
		}
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while(t--) solve();
}
