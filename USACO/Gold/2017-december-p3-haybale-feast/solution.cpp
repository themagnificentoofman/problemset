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
template <class T1, class T2> using P = pair<T1, T2>;
#define mp make_pair
#define fi first
#define se second
using pi = P<int, int>;
using pl = P<ll, ll>;
using pd = P<db, db>;

// vectors
template <class T> using V = vector<T>;
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
template <class T, size_t SZ> using AR = array<T, SZ>;
template <class T> int lwb(V<T> &a, const T &b) { return int(lb(all(a), b) - bg(a)); }
template <class T> int upb(V<T> &a, const T &b) { return int(ub(all(a), b) - bg(a)); }
template <class T> using pq = priority_queue<T>;
template <class T> using pqg = priority_queue<T, V<T>, greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s) { freopen(s.c_str(), "r", stdin); }
	void setOut(string s) { freopen(s.c_str(), "w", stdout); }
	void setIO(string s = "") {
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if breaks things
		if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
	}
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = 4e18;
const int MOD = (int)1e9 + 7;
const int MXN = (int)2e5 + 5;

/*
1) Problem recap (simple):
   - You have N haybales in a line; each has (flavor, spiciness).
   - Pick a contiguous segment with total flavor >= M, and minimize the maximum spiciness in that segment.

2) Things to notice + step-by-step:
   - If we fix a left end l, the best right end r is the smallest r that makes flavor sum >= M (two pointers).
   - As we slide the window, we need the window’s maximum spiciness quickly.
   - Maintain a multiset of spiciness values inside the current window; its largest element is the window max.

3) Solution outline + complexity:
   - Use two pointers (l, r) with a running flavor sum; expand r until sum >= M.
   - Track spiciness values in a multiset; update answer with *rbegin() when sum >= M.
   - Remove l as we advance; overall time: O(N log N).
*/

inline void solve() {
	int n;
	ll m;
	cin >> n >> m;

	V<P<ll, int>> a(n);
	for (auto &x : a) cin >> x.fi >> x.se;

	ll sum = 0;
	multiset<int> ms;
	int ans = INT32_MAX;

	int r = 0;
	for (int l = 0; l < n; l++) {
		while (r < n && sum < m) {
			sum += a[r].fi;
			ms.ins(a[r].se);
			r++;
		}
		if (sum >= m) {
			ans = min(ans, *ms.rbegin());
		}
		sum -= a[l].fi;
		auto it = ms.find(a[l].se);
		if (it != ms.end()) ms.erase(it);
	}

	cout << ans << '\n';
}

int main() {
	setIO("hayfeast");
	solve(); // single case
}
