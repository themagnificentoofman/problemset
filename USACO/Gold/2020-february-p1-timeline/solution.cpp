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
const int MXN = (int)1e5+5; // modify as needed

/*
1) Problem recap in simple terms.
   We have n events with initial earliest years s[i].
   Each constraint says event v must happen at least w years after event u.
   We must adjust s[i] upwards minimally so all constraints are satisfied, and still s[i] <= m.

2) Things to notice and step-by-step derivation.
   - Each constraint "v at least w after u" is an inequality: s[v] >= s[u] + w.
   - This is exactly a longest-path style DP on a DAG: we want to maximize s[i] subject to these constraints.
   - Model each cow/event as a node, and each constraint as a directed edge u -> v with weight w.
   - If we topologically sort the graph, we can relax edges in topo order and propagate larger lower bounds on s[i].
   - The problem guarantees the constraints form a DAG and that the final s[i] do not exceed m.

3) Solution outline.
   - Build a directed graph: for each constraint u v w, add edge u -> v with weight w, and increase indegree of v.
   - Run Kahn's algorithm to get a topological order of all nodes.
   - In topo order, for each edge u -> v with weight w, update s[v] = max(s[v], s[u] + w).
   - After processing all nodes, s[i] is the maximum feasible value respecting all constraints.
   - Output all s[i]. The asserts check that we processed a DAG and values stay within the given bounds.
*/

int s[MXN], in[MXN];
bool vis[MXN];
vpi adj[MXN]; // adj[u] holds edges u -> v with weight w meaning s[v] >= s[u] + w.

inline void solve() {
    int n, m, c;
    cin >> n >> m >> c;
    
    // Read initial earliest possible years for each event.
    for (int i = 1; i <= n; i++) cin >> s[i];
    
    // Read constraints: v must be at least w years after u.
    // Add directed edge u -> v with weight w and track indegree of v.
    for (int i = 0; i < c; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].pb({v,w});
        in[v]++;
    }
    
    // Kahn's algorithm: start with all nodes with indegree 0 (no prerequisites).
    queue<int> q;
    for (int i = 1; i <= n; i++) if (!in[i]) q.push(i);
    
    // Process nodes in topological order.
    while (sz(q)) {
        int u = q.ft; q.pop();
        vis[u] = 1; 
        
        // Problem guarantees final answer does not exceed m.
        // This assert ensures our DP respects that promise.
        assert(s[u] <= m);
        
        // Relax all outgoing edges u -> v with weight w.
        // s[v] must be at least s[u] + w, and we want the maximum possible lower bound.
        for (auto& [v, w] : adj[u]) {
            s[v] = max(s[v], s[u] + w);
            
            // Decrease indegree of v; when it hits 0, v is ready to process.
            if (!(--in[v])) q.push(v);
        }
    }
    
    // If some node was never visited, the graph had a cycle, which should not happen by problem guarantee.
    for (int i = 1; i <= n; i++) {
        assert(vis[i]);
        cout << s[i] << '\n';
    }
}

int main() {
    setIO("timeline");
    solve(); // single case
}