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
(1) Problem recap (simple):
- There are N Bessie pies and N Elsie pies, each with (B-value, E-value).
- When a cow receives a pie, she returns one of her pies whose value (in her eyes) is in [x, x + D],
  where x is how tasty she thought the pie she just received was.
- The exchange ends happily when someone receives a pie they value as 0.
- For each Bessie pie i, find the minimum number of gifts until happiness, or -1 if impossible.

(2) Key observations / derivation:
- Think of each pie as a node; edges alternate sides.
  Bessie pie i -> Elsie pie j if E(j) ∈ [E(i), E(i)+D]
  Elsie pie j -> Bessie pie i if B(i) ∈ [B(j), B(j)+D]
- Do a multi-source BFS on the reverse graph:
  Start from "happy-ending" pies (Bessie's pies with E=0, Elsie's pies with B=0) with dist=1.
- Reverse transitions become range queries:
  If we are at a Bessie pie i, its predecessors are Elsie pies j with B(j) ∈ [B(i)-D, B(i)].
  If we are at an Elsie pie j, its predecessors are Bessie pies i with E(i) ∈ [E(j)-D, E(j)].
- Maintain two ordered sets of unvisited pies keyed by the value used in the range query, so each pie
  is removed once overall (total O(N log N)).

(3) Solution outline:
- Read pies. Initialize dist=-1.
- Put all terminal pies in BFS queue with dist=1; put the rest into:
  setB = (E(i), i) for unvisited Bessie pies, setE = (B(j), j) for unvisited Elsie pies.
- BFS:
  pop v; range-erase from the opposite set and set dist[u]=dist[v]+1 for all hits.
- Output dist for Bessie pies 0..N-1.
- Complexity: O(N log N) time, O(N) memory.
*/

inline void solve() {
	int n; ll d;
	cin >> n >> d;
	int m = 2 * n;

	vl B(m), E(m);
	for (int i = 0; i < m; i++) cin >> B[i] >> E[i];

	vi dist(m, -1);
	queue<int> q;

	using pli = P<ll,int>;
	set<pli> unB; // unvisited Bessie pies keyed by E(i)
	set<pli> unE; // unvisited Elsie pies keyed by B(j)

	for (int i = 0; i < n; i++) {
		if (E[i] == 0) {
			dist[i] = 1;
			q.push(i);
		} else {
			unB.ins(mp(E[i], i));
		}
	}
	for (int j = n; j < m; j++) {
		if (B[j] == 0) {
			dist[j] = 1;
			q.push(j);
		} else {
			unE.ins(mp(B[j], j));
		}
	}

	while (!q.empty()) {
		int v = q.ft; q.pop();

		if (v < n) {
			// predecessors are Elsie pies with B in [B[v]-d, B[v]]
			ll L = B[v] - d, R = B[v];
			auto it = unE.lb(mp(L, -1));
			while (it != unE.end() && it->fi <= R) {
				int u = it->se;
				it = unE.erase(it);
				dist[u] = dist[v] + 1;
				q.push(u);
			}
		} else {
			// predecessors are Bessie pies with E in [E[v]-d, E[v]]
			ll L = E[v] - d, R = E[v];
			auto it = unB.lb(mp(L, -1));
			while (it != unB.end() && it->fi <= R) {
				int u = it->se;
				it = unB.erase(it);
				dist[u] = dist[v] + 1;
				q.push(u);
			}
		}
	}

	for (int i = 0; i < n; i++) cout << dist[i] << "\n";
}

int main() {
	setIO("piepie");
	solve(); // single case
}
