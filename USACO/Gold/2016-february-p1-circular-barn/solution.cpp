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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   There are N rooms in a circle, with c[i] cows starting in room i and total cows = N.
   We choose one door to open; then cows walk clockwise to fill rooms (1 cow per room).
   Energy is the sum of (distance walked)^2 over all cows. Minimize the energy.

2) Things to notice + step-by-step:
   - Fix a door, then a greedy clockwise simulation is forced: each room keeps 1 cow, extras flow right.
   - If we let cur be the number of “extra” cows arriving to the current room (from the left),
     then cows starting here end up with travel distances cur, cur+1, ..., cur+c[i]-1.
   - For a chosen door, total energy can be computed in O(N) using prefix sums of squares.
   - We first rotate to a valid door position (gas-station style) so the greedy never “needs cows from the future”.

3) Solution outline:
   - Read c[]. Find the rotation index for a valid door, rotate the array once.
   - Simulate once with cur and add sum_{k=cur}^{cur+c[i]-1} k^2 using a closed form.
   - Output the energy. Time: O(N).
*/

static inline ll sumsq(ll v) {
	// 0^2 + 1^2 + ... + v^2
	if (v <= 0) return 0;
	return v * (v + 1) * (2 * v + 1) / 6;
}

inline void solve() {
	int n;
	cin >> n;

	vl c(n);
	ll cur = 0;
	for (int i = 0; i < n; i++) {
		cin >> c[i];
		cur = max(0LL, cur + c[i] - 1);
	}

	// find a rotation index where this "carry" becomes 0
	for (int i = 0; i < n; i++) {
		if (cur == 0) {
			rotate(bg(c), bg(c) + i, end(c));
			break;
		}
		cur = max(0LL, cur + c[i] - 1);
	}

	ll ans = 0;
	cur = 0;
	for (ll x : c) {
		// add cur^2 + (cur+1)^2 + ... + (cur+x-1)^2
		ans += sumsq(cur + x - 1) - sumsq(cur - 1);
		cur = max(0LL, cur + x - 1);
	}

	cout << ans << "\n";
}

int main() {
	setIO("cbarn");
	solve(); // single case
}
