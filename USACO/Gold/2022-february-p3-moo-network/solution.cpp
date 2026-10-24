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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // 1st >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // 1st > b
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

const int MAX_Y = 11;

/*
 (1) Problem recap in simple terms:
     We have N cows at distinct integer points (x, y), where y is only 0..10. Building a link
     between two cows costs squared distance. We want the minimum total cost so all cows can
     communicate, i.e., the MST cost.

 (2) Things to notice and step-by-step derivation:
     - The full graph has ~N^2 edges, too big.
     - Since y only has 11 values, we can build a small candidate edge set.
     - Sort cows by x. While sweeping left to right, keep last[y] = the most recent cow we
       have seen in each row y (so it has the largest x <= current x in that row).
     - For the current cow, connecting to last[y] is the cheapest possible edge from this cow
       to row y among cows with x' <= x (same dy, smallest dx).
     - Any edge from the current cow to an earlier cow in the same row is worse than the edge
       to last[y] (bigger dx, same dy), so we never need those worse edges.

 (3) Solution outline:
     - Read all cows, sort indices by x.
     - Sweep in increasing x:
       - For each row y with last[y] existing, add edge (cur, last[y]).
       - Update last[y_cur] = cur.
     - Run Kruskal on these O(11N) edges using DSU and output the MST cost.
     - Time: O(N log N). Memory: O(N).
 */

struct DSU {
	vi e;
	DSU(int n = 0) { init(n); }
	void init(int n) { e.assign(n, -1); }
	int get(int x) { return e[x] < 0 ? x : e[x] = get(e[x]); }
	int size(int x) { return -e[get(x)]; }
	bool unite(int a, int b) {
		a = get(a), b = get(b);
		if (a == b) return false;
		if (e[a] > e[b]) swap(a, b);
		e[a] += e[b];
		e[b] = a;
		return true;
	}
};

struct Edge {
	ll w;
	int u, v;
	bool operator<(const Edge& o) const { return w < o.w; }
};

static inline ll dist2(int i, int j, const vi& x, const vi& y) {
	ll dx = (ll)x[i] - (ll)x[j];
	ll dy = (ll)y[i] - (ll)y[j];
	return dx * dx + dy * dy;
}

inline void solve() {
	int n;
	cin >> n;

	vi x(n), y(n);
	for (int i = 0; i < n; i++) cin >> x[i] >> y[i];

	vi ord(n);
	iota(all(ord), 0);
	sort(all(ord), [&](int a, int b) {
		if (x[a] != x[b]) return x[a] < x[b];
		return y[a] < y[b];
	});

	vi last(MAX_Y, -1);
	V<Edge> ed;
	ed.reserve(11LL * n);

	for (int id : ord) {
		for (int yy = 0; yy < MAX_Y; yy++) {
			if (last[yy] == -1) continue;
			ed.pb({dist2(id, last[yy], x, y), id, last[yy]});
		}
		last[y[id]] = id;
	}

	sor(ed);
	DSU dsu(n);

	ll ans = 0;
	int used = 0;
	for (auto &e : ed) {
		if (dsu.unite(e.u, e.v)) {
			ans += e.w;
			used++;
			if (used == n - 1) break;
		}
	}
	cout << ans << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
