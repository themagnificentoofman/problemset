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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   We have a laser at point A, a barn at point B, and N fence posts where we can place mirrors.
   Each mirror turns the beam 90 degrees. Find the minimum number of mirrors needed to hit the barn.

2) Things to notice + step-by-step:
   - The beam always travels along a vertical line (fixed x) or horizontal line (fixed y).
   - Putting a mirror at a post (x, y) switches between the x-line and the y-line.
   - So we can build a bipartite graph:
       left nodes = x-values, right nodes = y-values,
       and each fence post adds an edge between its x and y.
   - Starting with 0 mirrors, we can start on the laser’s x-line or y-line; hitting the barn means
     reaching the barn’s x-line or y-line.

3) Solution outline:
   - Coordinate-compress all x’s and y’s from laser/barn/posts.
   - Build adjacency lists x -> (list of y) and y -> (list of x) using only the fence posts.
   - BFS on this unweighted graph from both start lines with distance 0 (mirrors used so far).
   - Answer = min(dist[x_barn], dist[y_barn]) (or -1 if unreachable).
   - Time: O((N + #unique coords)).
*/

inline void solve() {
	int n;
	cin >> n;

	vpi pts(n + 2);
	for (int i = 0; i < n + 2; i++) cin >> pts[i].fi >> pts[i].se;

	pi laser = pts[0], barn = pts[1];

	vi xs, ys;
	xs.reserve(n + 2);
	ys.reserve(n + 2);
	for (int i = 0; i < n + 2; i++) {
		xs.pb(pts[i].fi);
		ys.pb(pts[i].se);
	}
	sor(xs);
	xs.erase(unique(all(xs)), end(xs));
	sor(ys);
	ys.erase(unique(all(ys)), end(ys));

	unordered_map<int, int> xid, yid;
	xid.reserve(sz(xs) * 2 + 10);
	yid.reserve(sz(ys) * 2 + 10);
	for (int i = 0; i < sz(xs); i++) xid[xs[i]] = i;
	for (int i = 0; i < sz(ys); i++) yid[ys[i]] = i;

	V<vi> xToY(sz(xs)), yToX(sz(ys));

	// only fence posts (not laser/barn) create edges
	for (int i = 2; i < n + 2; i++) {
		int xi = xid[pts[i].fi];
		int yi = yid[pts[i].se];
		xToY[xi].pb(yi);
		yToX[yi].pb(xi);
	}

	int xL = xid[laser.fi], yL = yid[laser.se];
	int xB = xid[barn.fi], yB = yid[barn.se];

	vi distX(sz(xs), -1), distY(sz(ys), -1);
	queue<pi> q; // {type, id} where type 0=x, 1=y

	distX[xL] = 0;
	distY[yL] = 0;
	q.push({0, xL});
	q.push({1, yL});

	while (!q.empty()) {
		auto [tp, id] = q.ft; q.pop();

		if (tp == 0) {
			for (int ny : xToY[id]) {
				if (distY[ny] != -1) continue;
				distY[ny] = distX[id] + 1;
				q.push({1, ny});
			}
		} else {
			for (int nx : yToX[id]) {
				if (distX[nx] != -1) continue;
				distX[nx] = distY[id] + 1;
				q.push({0, nx});
			}
		}
	}

	int ans = -1;
	if (distX[xB] != -1) ans = distX[xB];
	if (distY[yB] != -1) ans = (ans == -1 ? distY[yB] : min(ans, distY[yB]));
	cout << ans << "\n";
}

int main() {
	setIO("lasers");
	solve(); // single case
}
