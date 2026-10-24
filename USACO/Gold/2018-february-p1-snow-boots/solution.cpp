// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t;
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
1) Problem recap (simple terms):
   For each boot (max snow depth s, max step d), decide if you can go from tile 1 to tile N
   stepping only on tiles with snow <= s, and with each jump length <= d.

2) Things to notice + derivation:
   If we fix a boot, the “allowed” tiles are exactly those with depth <= s.
   You can traverse iff the maximum gap between consecutive allowed tiles (including endpoints)
   is <= d. So for each boot, we just need that max gap.

   If we process boots in decreasing s, we can maintain the allowed tiles by deleting tiles
   that are too deep as s decreases. When deleting a tile, only the gap between its neighbors
   can increase, so we can update the max gap in O(1) given predecessor/successor.

3) Solution outline:
   - Sort boots by s descending (remember original indices).
   - Sort interior tiles by depth descending.
   - Maintain a sorted set of “still allowed” tile indices (start with all tiles).
   - As we move through boots, delete all tiles with depth > current s, updating max gap.
   - Boot is feasible iff d >= current max gap.
*/

struct Boot {
	int s, d, idx;
};

inline void solve() {
	int n, b; cin >> n >> b;
	vi f(n);
	for (int i = 0; i < n; i++) cin >> f[i];

	V<Boot> boots(b);
	for (int i = 0; i < b; i++) {
		cin >> boots[i].s >> boots[i].d;
		boots[i].idx = i;
	}

	sort(all(boots), [&](const Boot& a, const Boot& c) {
		return a.s > c.s;
	});

	vi ord_tiles;
	for (int i = 1; i <= n - 2; i++) ord_tiles.pb(i); // interior only; endpoints always depth 0
	sort(all(ord_tiles), [&](int a, int c) {
		return f[a] > f[c];
	});

	set<int> alive;
	for (int i = 0; i < n; i++) alive.ins(i);

	int ptr = 0;
	int max_gap = 1; // at least need to step from 0 to 1 (or 0 to n-1 when n=2)
	vb ok(b, 0);

	for (auto &bt : boots) {
		while (ptr < sz(ord_tiles) && f[ord_tiles[ptr]] > bt.s) {
			int x = ord_tiles[ptr++];
			auto it = alive.find(x);
			int L = *prev(it);
			int R = *next(it);
			max_gap = max(max_gap, R - L);
			alive.erase(it);
		}
		ok[bt.idx] = (bt.d >= max_gap);
	}

	for (int i = 0; i < b; i++) cout << ok[i] << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
