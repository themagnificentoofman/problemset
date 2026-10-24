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

/*
 (1) Problem recap (simple):
    - Directed graph with N nodes, M edges (u -> v).
    - Constraint: for every node u, ALL nodes in its outgoing list must end up the SAME color.
    - Goal: assign a color to each node satisfying all constraints, using the minimum number of colors
      (equivalently, compress nodes into the fewest equivalence classes implied by the constraint).

 (2) Things to notice & step-by-step derivation:
    - For a node u with outgoing neighbors a, b, c, ... the rule says: color(a) = color(b) = color(c) = ...
      ⇒ We can UNION any pair of out-neighbors of the same u.
    - These unions are transitive: merging a and b may force more merges elsewhere. After we repeatedly apply
      "merge out-neighbors of a node with outdeg ≥ 2", every remaining component will have outdeg ≤ 1.
    - After compression, the quotient graph (by components) has at most one outgoing edge per node (a functional graph).
      In such graphs, each weak component is a directed cycle with in-trees feeding into it; nodes that eventually
      flow to the same cycle must share the same color. Our unions exactly produce these color groups.

 (3) Solution outline:
    - Maintain DSU components as the evolving color groups.
    - Keep an adjacency list PER COMPONENT (concatenation of member adj lists). If a component has outdeg ≥ 2,
      push it into a queue.
    - While the queue is nonempty:
        * Take a component x whose adjacency has ≥ 2 targets.
        * Repeatedly take the last two neighbors a, b of x. If their DSU reps differ, merge those two components,
          and also merge their adjacency lists. If the merged component now has outdeg ≥ 2, enqueue it.
        * Continue until x has ≤ 1 outgoing target.
    - Finally, assign incremental color IDs to DSU representatives and print each node’s color.
    - Complexity: near O(N + M). We always merge smaller member lists into bigger ones to keep total work linear-ish.
*/

// --- DSU with explicit member lists so we can reassign parents in O(size of smaller) and merge adjacency ---
struct DSU {
	vi par;          // parent / representative
	V<vi> members;   // members[rep] = list of nodes in this component
	V<vi> adj;       // adj[rep] = concatenated adjacency (edges out of all members of this component)

	DSU(int n = 0) { init(n); }

	void init(int n) {
		par.assign(n+1, 0);
		members.assign(n+1, {});
		adj.assign(n+1, {});
		for (int i = 1; i <= n; ++i) {
			par[i] = i;
			members[i].pb(i);
		}
	}

	int find(int x) { return par[x]; } // representatives are maintained explicitly

	/*
	 merge(a, b):
	 - Union the components of a and b (using size heuristic on 'members').
	 - Also concatenates adjacency lists: adj[repA] += adj[repB].
	 - Returns the representative of the merged component.
	*/
	int merge(int a, int b) {
		a = find(a), b = find(b);
		if (a == b) return a;
		// always merge smaller into larger (by members size)
		if (sz(members[a]) < sz(members[b])) swap(a, b);

		// reparent all nodes of b to a
		for (int v : members[b]) par[v] = a;
		// move members and adjacency
		members[a].ins(members[a].end(), all(members[b]));
		members[b].clear();

		adj[a].ins(adj[a].end(), all(adj[b]));
		adj[b].clear();

		return a;
	}
};

inline void solve() {
	int N, M; 
	cin >> N >> M;

	DSU dsu(N);
	for (int i = 0; i < M; ++i) {
		int u, v; 
		cin >> u >> v;
		dsu.adj[u].pb(v); // initial edges start from singleton reps
	}

	queue<int> q;
	for (int i = 1; i <= N; ++i)
		if (sz(dsu.adj[i]) > 1) q.push(i); // components with outdeg >= 2 need processing

	/*
	 Process:
	 - Take a component x. While it has >= 2 outgoing neighbors:
	   * Compare the DSU reps of the last two neighbors a and b.
	   * If different, union them. The union's adjacency becomes bigger; if it reaches >= 2, we enqueue it.
	   * We keep x at the front until its outdeg <= 1, then pop it and move on.
	*/
	while (sz(q)) {
		int x = q.ft;
		if (sz(dsu.adj[x]) <= 1) { q.pop(); continue; }

		// Take two neighbors from the back.
		int a = dsu.adj[x].bk; dsu.adj[x].ppb();
		int b = dsu.adj[x].bk;

		int ra = dsu.find(a), rb = dsu.find(b);
		if (ra == rb) continue; // already same component; try again next iteration

		int r = dsu.merge(ra, rb);       // merge the two target components
		if (sz(dsu.adj[r]) >= 2) q.push(r); // if the merged target now has >=2 out-neighbors, it needs processing
	}

	// Assign compact color IDs per representative, then print each node's color.
	vi rep2color(N+1);
	int colors = 0;
	for (int i = 1; i <= N; ++i) {
		int r = dsu.find(i);
		if (!rep2color[r]) rep2color[r] = ++colors;
	}
	for (int i = 1; i <= N; ++i)
		cout << rep2color[dsu.find(i)] << '\n';
}

int main() {
	setIO("fcolor");
	solve(); // single case
}