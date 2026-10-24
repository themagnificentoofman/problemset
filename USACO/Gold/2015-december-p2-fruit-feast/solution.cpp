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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better, basically
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;

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
const ll BIG = 4e18; // large enough for ll
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   You want to get as full as possible (<= T). You can eat fruits of size A or B any number of times.
   Exactly once, you may drink water, which halves your current fullness (floor).
   Find the maximum fullness you can reach.

2) Things to notice + step-by-step:
   - Think of two states: before drinking water (dry) and after drinking water (wet).
   - From either state, you can add A or B (if it stays <= T).
   - Only from dry, you can "drink water" and move to wet at fullness i/2.
   - This becomes a simple reachability DP over fullness 0..T.

3) Solution outline:
   - Compute reachable dry[] with transitions +A, +B, and also mark wet[i/2] from each reachable dry[i].
   - Then compute reachable wet[] with transitions +A, +B.
   - Answer is max i where dry[i] or wet[i] is true.
   - Time: O(T).
*/

inline void solve() {
	int T, A, B;
	cin >> T >> A >> B;

	vb dry(T + 1, false), wet(T + 1, false);
	dry[0] = true;

	for (int i = 0; i <= T; i++) {
		if (!dry[i]) continue;
		if (i + A <= T) dry[i + A] = true;
		if (i + B <= T) dry[i + B] = true;
		wet[i / 2] = true; // drink water once
	}

	for (int i = 0; i <= T; i++) {
		if (!wet[i]) continue;
		if (i + A <= T) wet[i + A] = true;
		if (i + B <= T) wet[i + B] = true;
	}

	for (int i = T; i >= 0; i--) {
		if (dry[i] || wet[i]) {
			cout << i << "\n";
			return;
		}
	}
}

int main() {
	setIO("feast");
	solve(); // single case
}
