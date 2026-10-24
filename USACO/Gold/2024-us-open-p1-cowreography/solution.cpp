// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default)
using db = long double;  // change to double if TL is tight (almost never)

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
template<class T, size_t SZ> using AR = array<T,SZ>;
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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap
You are given two binary strings s and t of length n (with the same number of '1's), and an
integer k. In one move, you may choose two positions whose indices differ by at most k and
swap the characters at those positions. Find the minimum number of moves to transform s into t.

2) Things to notice and step-by-step derivation
- Each mismatch position contributes either a +1 "extra 1" (s[i]=1,t[i]=0) or a -1 "missing 1"
  (s[i]=0,t[i]=1). These units must be transported across the line via allowed swaps.
- An optimal strategy can be viewed as moving a running balance forward; each time that balance
  is carried across a k-step "gap", it costs |balance| moves.
- We maintain a deque of active balances anchored at certain indices. As i increases:
  - If the front anchor is behind i, we "advance" it by k and pay |balance|.
  - If s[i]!=t[i], we update the front balance by +/-1, creating/removing a block as needed.

3) Solution outline
- Scan i = 0..n-1 once.
- Maintain a deque of blocks (pos, bal). When pos < i, add |bal| to answer and shift pos += k.
- On mismatch at i, add dif = +1 (if s[i]=='1') else -1 to the appropriate front block.
- Time: O(n), Memory: O(n).
*/

inline void solve() {
	int n, k;
	cin >> n >> k;
	str s, t;
	cin >> s >> t;

	deque<pi> dq; // (pos, balance)
	ll ans = 0;

	for (int i = 0; i < n; i++) {
		if (!dq.empty() && dq.ft.fi < i) {
			ans += (ll)abs(dq.ft.se);
			dq.pb({dq.ft.fi + k, dq.ft.se});
			dq.ppf();
		}

		if (s[i] == t[i]) continue;

		int dif = (s[i] == '1' ? 1 : -1);

		if (dq.empty() || abs(dq.ft.se + dif) > abs(dq.ft.se)) {
			if (dq.empty() || dq.ft.fi != i) dq.pf({i, 0});
			dq.ft.se += dif;
		} else {
			dq.ft.se += dif;
			if (dq.ft.se == 0) dq.ppf();
		}
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
