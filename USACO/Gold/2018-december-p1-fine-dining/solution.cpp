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
1) Problem recap in simple terms:
   For each pasture i (except the barn), decide if there exists a path to the barn that stops at >=1 haybale
   such that (path length - hay yumminess) is <= the shortest path length (no hay constraint).

2) Things to notice and step-by-step derivation:
   Let dist[i] = shortest distance from i to barn (run Dijkstra from the barn).
   If we take a haybale at node j with yum y[j], then for any i:
     best[i] = min over hay nodes j of (dist[j] - y[j] + shortestDist(i, j)).
   If best[i] <= dist[i], then i can reach the barn with "effective cost" no worse than the true shortest path,
   meaning there is a qualifying route (take that haybale somewhere along the way).
   Computing best[] is just another multi-source Dijkstra:
     initialize source j with distance (dist[j] - y[j]) and relax edges normally.

3) Solution outline:
   - Build undirected graph.
   - Dijkstra from barn -> dist[].
   - Multi-source Dijkstra seeded by each hay node j with key dist[j]-yum[j] -> best[].
   - Output 1 for i!=barn iff best[i] <= dist[i], else 0.
*/

inline void solve() {
	int n, m, k;
	cin >> n >> m >> k;

	V<vpi> g(n);
	for (int i = 0; i < m; i++) {
		int a, b, w;
		cin >> a >> b >> w;
		--a, --b;
		g[a].pb({b, w});
		g[b].pb({a, w});
	}

	vi yum(n, 0);
	for (int i = 0; i < k; i++) {
		int idx, y;
		cin >> idx >> y;
		--idx;
		yum[idx] = max(yum[idx], y);
	}

	auto dijkstra = [&](vl &d, pqg<pl> &pq) {
		while (sz(pq)) {
			auto [du, u] = pq.top(); pq.pop();
			if (du > d[u]) continue;
			for (auto [v, w] : g[u]) {
				ll nv = du + (ll)w;
				if (nv < d[v]) {
					d[v] = nv;
					pq.push({nv, v});
				}
			}
		}
	};

	// dist[i] = shortest distance from i to barn
	vl dist(n, BIG);
	pqg<pl> pq1;
	dist[n - 1] = 0;
	pq1.push({0, n - 1});
	dijkstra(dist, pq1);

	// best[i] = min_j (dist[j] - yum[j] + shortestDist(i, j)) over hay nodes j
	vl best(n, BIG);
	pqg<pl> pq2;
	for (int i = 0; i < n; i++) if (yum[i] > 0) {
		ll start = dist[i] - (ll)yum[i];
		if (start < best[i]) {
			best[i] = start;
			pq2.push({start, i});
		}
	}
	dijkstra(best, pq2);

	for (int i = 0; i < n - 1; i++) {
		cout << (best[i] <= dist[i]) << '\n';
	}
}

int main() {
	setIO("dining");
	solve(); // single case
}