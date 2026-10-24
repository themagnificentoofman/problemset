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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
template<class T> using pq = priority_queue<T>;
template<class T> using pqg = priority_queue<T, V<T>, greater<T>>;

// input & output
inline namespace FileIO {
    void setIn(string s){ freopen(s.c_str(), "r", stdin); }
    void setOut(string s){ freopen(s.c_str(), "w", stdout); }
    void setIO(string s = ""){
        cin.tie(0)->sync_with_stdio(0);
        cin.exceptions(cin.failbit);
        if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
    }
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
 (1) Problem recap in simple terms:
     - We have n farms and m pipes. Each pipe has a cost (length) and a flow rate.
     - We need a path from farm 1 to farm n that maximizes:
           (minimum flow along the path) / (total cost of the path),
       then multiply this ratio by 1e6 and take the floor.

 (2) Things to notice and step-by-step derivation:
     - For any fixed minimum flow F, we are only allowed to use pipes whose flow >= F.
     - Under that restriction, the best path is simply the path from 1 to n with the
       smallest total cost (a standard shortest-path problem).
     - So for a fixed F, we can:
          * Build a subgraph using only edges with flow >= F.
          * Run Dijkstra on this subgraph to get the minimum cost C from 1 to n.
          * If such a path exists, the score for this F is floor(F / C * 1e6).
     - Pipe flows come from the input, so it is enough to try each distinct flow value
       from the edges as a candidate F.

 (3) Solution outline:
     - Read n, m and the list of pipes; store the graph and all flow values.
     - Sort and deduplicate the list of flow values.
     - For each candidate flow F:
          * Run Dijkstra on the full graph, but when relaxing edges, only consider
            edges with flow >= F.
          * If we can reach node n with total cost C, update the answer with
            floor(F / C * 1e6).
     - Output the maximum answer over all candidate flows.
*/

struct Edge {
    int to;
    int cost;
    int flow;
};

// Dijkstra with a minimum-flow constraint on edges.
int dijkstra(const V<V<Edge>>& adj, int minFlow, int n) {
    const int INF = INT_MAX;
    vi dist(n, INF);
    pqg<pi> pq; // (distance, node), min-heap by distance.

    dist[0] = 0;
    pq.push({0, 0}); // start from farm 0 (which is farm 1 in 1-based indexing).

    while (!pq.empty()) {
        auto cur = pq.top();
        pq.pop();
        int d = cur.fi;
        int u = cur.se;

        if (d != dist[u]) continue; // skip outdated states.

        for (const auto& e : adj[u]) {
            if (e.flow < minFlow) continue; // edge doesn't meet flow requirement.

            int nd = d + e.cost;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                pq.push({nd, e.to});
            }
        }
    }

    if (dist[n - 1] == INF) return -1; // no valid path with this minFlow.
    return dist[n - 1];
}

inline void solve() {
    int n, m;
    cin >> n >> m;

    V<V<Edge>> adj(n);
    vi flows;
    flows.reserve(m);

    for (int i = 0; i < m; i++) {
        int u, v, c, fl;
        cin >> u >> v >> c >> fl;
        --u; --v; // convert to 0-based.
        adj[u].pb({v, c, fl});
        adj[v].pb({u, c, fl});
        flows.pb(fl);
    }

    // Only need to try each distinct flow value once.
    sor(flows);
    flows.erase(unique(all(flows)), flows.end());

    int ans = 0; // answer is non-negative; 0 is safe initial value.
    for (int flow : flows) {
        int cost = dijkstra(adj, flow, n);
        if (cost == -1) continue; // no path with this flow.

        db ratio = (db)flow / (db)cost;
        int score = (int)(ratio * 1e6); // floor by cast to int.
        if (score > ans) ans = score;
    }

    cout << ans << '\n';
}

int main() {
    setIO("pump");
    solve(); // single case
}
