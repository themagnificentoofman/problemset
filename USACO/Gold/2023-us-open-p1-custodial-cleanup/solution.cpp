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
const int MXN = (int)2e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     We have stalls connected by corridors. Stall i has color C[i], and you can only enter a
     non-1 stall if you currently hold a key of that color. Each stall starts with key S[i],
     and we want to end with key F[i] in stall i (S is a permutation of F). For each test
     case, decide if Farmer John can rearrange the keys and finish back at stall 1.

 (2) Things to notice and step-by-step derivation:
     - Having more keys never hurts, so when exploring reachable stalls, greedily pick up
       every key as soon as the stall becomes reachable.
     - Any stall never visited in this greedy "pick up" process can never have its key moved,
       so it must already satisfy S[i] = F[i].
     - Placing keys forward is tricky, but if a valid placing process exists, reversing it
       looks like a similar greedy process on the remaining subgraph.
     - In the reversed process, the movement condition becomes: a stall can be added once we
       either already have a key of its color, or its final key equals its color (so after
       picking up inside, we can keep moving).

 (3) Solution outline:
     - Build the graph.
     - Stage 1: greedy BFS to pick up keys S and mark reachable stalls.
     - If a stall is unreachable in stage 1 and S[i] != F[i], answer is NO.
     - Stage 2: run the same BFS idea on the stage-1 subgraph using keys F, with the extra
       rule that stalls with C[i] = F[i] are immediately usable.
     - Answer YES iff every stage-1 stall is also reachable in stage 2.
     - Time: O(n + m) per test case; Memory: O(n + m).
 */

inline void solve() {
	int n, m; cin >> n >> m;

	vi color(n), stKey(n), finKey(n);
	for (int i = 0; i < n; i++) cin >> color[i];
	for (int i = 0; i < n; i++) cin >> stKey[i];
	for (int i = 0; i < n; i++) cin >> finKey[i];

	V<vi> g(n);
	for (int i = 0; i < m; i++) {
		int u, v; cin >> u >> v;
		--u; --v;
		g[u].pb(v);
		g[v].pb(u);
	}

	auto runStage = [&](const vi& keyIn, const vb* allowed, bool special) -> vb {
		vb haveKey(n + 1, false);

		// waiting lists by needed stall color (singly-linked lists in arrays)
		vi head(n + 1, -1), nxt(n, -1);

		queue<int> q;
		vb seen(n, false), reach(n, false);

		q.push(0);
		seen[0] = true;
		haveKey[keyIn[0]] = true;

		while (!q.empty()) {
			int v = q.ft;
			q.pop();
			reach[v] = true;

			int got = keyIn[v];
			if (!haveKey[got]) {
				haveKey[got] = true;
				for (int cur = head[got]; cur != -1; cur = nxt[cur]) q.push(cur);
				head[got] = -1;
			}

			for (int u : g[v]) {
				if (seen[u]) continue;
				if (allowed && !(*allowed)[u]) continue;
				seen[u] = true;

				if (haveKey[color[u]] || (special && color[u] == keyIn[u])) {
					q.push(u);
				} else {
					nxt[u] = head[color[u]];
					head[color[u]] = u;
				}
			}
		}
		return reach;
	};

	vb reach1 = runStage(stKey, nullptr, false);

	bool ok = true;
	for (int i = 0; i < n; i++) {
		if (!reach1[i] && stKey[i] != finKey[i]) ok = false;
	}
	if (!ok) {
		cout << "NO\n";
		return;
	}

	vb reach2 = runStage(finKey, &reach1, true);

	for (int i = 0; i < n; i++) {
		if (reach1[i] && !reach2[i]) ok = false;
	}
	cout << (ok ? "YES\n" : "NO\n");
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while(t--) solve();
}
