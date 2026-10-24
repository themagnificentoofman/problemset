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
   - We have a tree with N nodes (N-1 edges).
   - For each K from 1 to N-1, we need to check if we can partition all edges into disjoint simple paths of length exactly K.

2) Things to notice and step-by-step derivation.
   - If (N-1) is not divisible by K, we cannot split edges into groups of size K, so the answer for K is 0 immediately.
   - Root the tree at 1. For each node v, consider every incident edge:
       * For a child u, cutting edge (v,u) gives a component of size sub[u].
       * For the parent side, cutting the edge to v’s parent gives a component of size N - sub[v].
   - For a fixed K, only the sizes modulo K matter.
       * A component of size x contributes remainder r = x % K.
       * Remainder r = 0 is already "perfect" (forms complete K-paths), so we can ignore those.
       * Other remainders must pair as r and K-r at that vertex so that partial paths passing through v can be glued into full paths of length K.
   - The editorial’s condition: for K to work, at every vertex, these remainders must be pairable into r + (K-r) ≡ 0 (mod K), with no leftover.

3) Solution outline.
   - Do one DFS to compute subtree sizes sub[v].
       * For each node v, store in compSizes[v] all incident component sizes:
           - sub[child] for each child.
           - N - sub[v] if v is not the root (the parent side).
   - For each K from 1 to N-1:
       * First, check (N-1) % K == 0. If not, K fails.
       * Otherwise, for every vertex v:
           - Look at compSizes[v], and take x % K for each x.
           - Skip remainders 0 (they already give full K-paths).
           - Use a small bucket array cur[0..K-1] to greedily pair remainders:
                • If we see remainder r and there is an unmatched K-r in cur, pair them.
                • Otherwise mark r as currently unmatched.
           - If after processing v there is any unmatched remainder, then K fails.
       * If all vertices pass, output 1 for K; else 0.
   - Overall we reuse the same precomputed compSizes, so for each K we just re-check the local pairing condition at every vertex.
*/

int N;
int sub[MXN];        // subtree sizes.
vi adj[MXN];         // adjacency list of the tree.
vi compSizes[MXN];   // for each node v, sizes of all components incident to v.
int cur[MXN];        // bucket array for remainders modulo K at a single node.

/*
 (1) DFS: compute subtree sizes and incident component sizes for each node.
 (2) For node v:
     - Add sub[child] for each child edge.
     - If v is not the root, also add N - sub[v] for the parent side.
*/
void dfs(int v, int p) {
	sub[v] = 1; // count self.
	for (int to : adj[v]) {
		if (to == p) continue;
		dfs(to, v);
		sub[v] += sub[to];
		compSizes[v].pb(sub[to]); // component size below edge (v,to).
	}
	if (sub[v] != N) {
		// Component size on the parent side (only exists if v is not the root).
		compSizes[v].pb(N - sub[v]);
	}
}

/*
 Check if a given K is valid.
 - Uses the precomputed compSizes for every node.
*/
bool ok(int K) {
	// Total number of edges must be divisible by K.
	if ((N - 1) % K) return false;

	// Clear buckets for remainders 0..K-1 for this K.
	for (int r = 0; r < K; ++r) cur[r] = 0;

	// Check each node independently.
	for (int v = 1; v <= N; ++v) {
		int cnt = 0; // number of unmatched segments at this node (sum of cur[*]).

		for (int x : compSizes[v]) {
			int rem = x % K;
			if (!rem) continue; // already contributes perfect K-paths.

			int need = K - rem; // we want rem + need ≡ 0 (mod K).

			if (cur[need]) {
				// We have an unmatched segment with remainder 'need' at this node.
				// Pair it with this 'rem' segment into a multiple of K.
				cur[need]--;
				cnt--;
			} else {
				// No complement yet; mark this remainder as unmatched for now.
				cur[rem]++;
				cnt++;
			}
		}

		// If any remainder is left unmatched at this node, K is invalid.
		// Thanks to the pairing logic, cnt == 0 ⇔ all cur[*] == 0 again.
		if (cnt) return false;
	}

	return true;
}

inline void solve() {
	cin >> N;

	// Build the tree.
	for (int i = 1; i <= N; ++i) {
		adj[i].clear();
		compSizes[i].clear();
	}

	for (int i = 0; i < N - 1; ++i) {
		int a, b;
		cin >> a >> b;
		adj[a].pb(b);
		adj[b].pb(a);
	}

	// Precompute subtree sizes and incident component sizes.
	dfs(1, 0);

	// For each K from 1..N-1, check if it works.
	for (int K = 1; K < N; ++K) cout << (ok(K) ? 1 : 0);
	cout << '\n';
}

int main() {
	setIO("deleg");
	solve(); // single case
}