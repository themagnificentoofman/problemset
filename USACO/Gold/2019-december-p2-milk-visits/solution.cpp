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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a), b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a), b) - bg(a)); }
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
const ll BIG = 4e18;          // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7;   // 998244353;
const int MXN = (int)1e5+5;   // max N, M, types

/*
 (1) Problem recap in simple terms:
     We have a tree of N farms, each farm has a milk type. For each query (a, b, c), we need to know if the path between
     farms a and b contains at least one farm whose type is exactly c.

 (2) Things to notice and step-by-step derivation:
     - The graph is a tree, so there is exactly one simple path between any two farms.
     - We can root the tree and do an Euler-style DFS to give each node a subtree interval [in, out]; this lets us quickly
       test whether one node is an ancestor of another.
     - We process all queries offline with a DFS:
         • Maintain the current root-to-node path in a vector `ord`.
         • For each milk type t, keep a stack of (node, index in ord) giving all nodes of type t on the current path.
         • Attach each query to both of its endpoints; when we visit an endpoint x, we look at the stack for the required
           type c. If it’s non-empty, we have a closest node y of type c on the path root → x.
         • If y == x, we already have type c on the path. Otherwise we check whether y lies on the path between the two
           endpoints using subtree-interval (ancestor) tests and a small trick with the child after y on the root → x path.

 (3) Solution outline:
     - Root the tree at node 1 and run a DFS to assign each node a subtree interval [in, out].
     - For each query (a, b, c), store it at both endpoints a and b.
     - Run a second DFS from the root:
         • When entering a node x, push (x, position_in_path) into the stack of its milk type and add x to the path `ord`.
         • For every query attached to x, check the stack for its required type:
             - If empty, we cannot satisfy it here.
             - Otherwise take the last node y in that stack (closest to x). If y == x, mark query as satisfiable.
             - If y != x, take the child Y after y on the path root → x, and let w be the other endpoint of the query.
               If Y is not an ancestor of w, then the path between endpoints passes through y, so the query is satisfiable.
         • Recurse into children, then pop x from the type stack and from the path `ord`.
     - Output 1 for satisfiable queries and 0 otherwise.
*/

// problem-specific globals
int milkType[MXN];          // milkType[i]: type of farm i
int desiredType[MXN];       // desiredType[q]: required milk type for query q
bool ok[MXN];               // ok[q]: does query q have at least one matching type on the path?
vi adj[MXN];                // adjacency list of the tree
pi endpoints[MXN];          // endpoints[q] = (a, b) for query q
vi queriesAtNode[MXN];      // queriesAtNode[x]: indices of queries that have x as an endpoint
pi rangePos[MXN];           // rangePos[x] = [in, out] from Euler DFS
int curTime = 0;

vpi stackByType[MXN];       // stackByType[t] = stack of (node, index_in_ord) for type t on current root->node path
vi ord;                     // current root-to-node path (sequence of nodes)

// DFS to assign Euler intervals [in, out] for each node's subtree.
inline void dfsEuler(int node, int parent) {
    rangePos[node].fi = curTime++;
    for (int nxt : adj[node]) {
        if (nxt == parent) continue;
        dfsEuler(nxt, node);
    }
    rangePos[node].se = curTime - 1;
}

// return true if a is an ancestor of b in the rooted tree (if a's Euler interval contains b's Euler interval)
inline bool isAncestor(int a, int b) {
    return (rangePos[a].fi <= rangePos[b].fi &&
            rangePos[b].se <= rangePos[a].se);
}

// second DFS: maintain path and per-type stacks, process queries at endpoints
inline void dfsSolve(int node, int parent) {
    int t = milkType[node];

    // push this node onto the path and onto the stack for its milk type
    stackByType[t].pb({node, sz(ord)});
    ord.pb(node);

    // process all queries that have this node as an endpoint
    for (int qi : queriesAtNode[node]) {
        int wanted = desiredType[qi];
        if (!stackByType[wanted].empty()) {
            auto last = stackByType[wanted].back(); // closest node y of type wanted on root->node path
            int yNode = last.fi;
            int idxInOrd = last.se;

            if (yNode == node) {
                // current endpoint has the right type
                ok[qi] = true;
            } else {
                // yNode is above node on the root->node path
                int childOnPath = ord[idxInOrd + 1];          // child after yNode towards node
                int other = endpoints[qi].fi + endpoints[qi].se - node; // other endpoint of the query

                // x must be one endpoint for this to hold
                assert(endpoints[qi].fi == node || endpoints[qi].se == node);

                // if childOnPath is NOT an ancestor of the other endpoint,
                // then the path between the two endpoints must go through yNode
                if (!isAncestor(childOnPath, other)) {
                    ok[qi] = true;
                }
            }
        }
    }

    // continue DFS
    for (int nxt : adj[node]) {
        if (nxt == parent) continue;
        dfsSolve(nxt, node);
    }

    // remove this node from the path and from the type stack
    stackByType[t].ppb();
    ord.ppb();
}

inline void solve() {
    int n, m;
    cin >> n >> m;

    for (int i = 1; i <= n; i++) {
        cin >> milkType[i];
    }

    for (int i = 0; i < n - 1; i++) {
        int a, b;
        cin >> a >> b;
        adj[a].pb(b);
        adj[b].pb(a);
    }

    // preprocess Euler intervals for ancestor checks
    dfsEuler(1, 0);

    // read queries and attach each to both endpoints
    for (int i = 0; i < m; i++) {
        int a, b, c;
        cin >> a >> b >> c;
        endpoints[i] = {a, b};
        desiredType[i] = c;
        queriesAtNode[a].pb(i);
        queriesAtNode[b].pb(i);
    }

    // main DFS that processes queries using type stacks along paths
    dfsSolve(1, 0);

    // output answers as 0/1 string
    for (int i = 0; i < m; i++) {
        cout << (ok[i] ? '1' : '0');
    }
}

int main() {
    setIO("milkvisits");
    solve(); // single case
}
