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
1) Problem recap (simple):
   Bessie starts at node K in a tree and moves 1 edge per minute. Farmers can start at any leaf and also move 1 edge per minute.
   Find the minimum number of farmers needed so Bessie is guaranteed to be caught.

2) Things to notice / key idea:
   Let distB[u] = distance from Bessie's start K to u.
   Let distF[u] = distance from the nearest leaf to u (multi-source BFS from all leaves).
   If distF[u] <= distB[u], then some farmer can reach u no later than Bessie, so Bessie cannot safely pass through u.
   If we root the tree at K and walk outward, whenever we first hit such a node u, we must "place" a farmer there and stop exploring below u.

3) Solution outline:
   - BFS from K to get distB.
   - Multi-source BFS from all leaves to get distF.
   - DFS/stack from K with parent pointers: if distF[u] <= distB[u], count 1 farmer and do not go deeper; else continue to children.
   - Time complexity: O(n).
*/

inline void solve() {
	const int INF = (int)1e9;

	int n, k; 
	cin >> n >> k;
	--k;

	if (n == 1) { // only node is both start and a leaf
		cout << 1 << "\n";
		return;
	}

	V<vi> adj(n);
	for (int i = 0; i < n-1; i++) {
		int a, b; cin >> a >> b;
		--a; --b;
		adj[a].pb(b);
		adj[b].pb(a);
	}

	vi distB(n, INF), distF(n, INF);
	queue<int> q;

	// BFS from Bessie's start to get distB
	distB[k] = 0;
	q.push(k);
	while (sz(q)) {
		int u = q.ft; q.pop();
		for (int v : adj[u]) {
			if (distB[v] > distB[u] + 1) {
				distB[v] = distB[u] + 1;
				q.push(v);
			}
		}
	}

	// multi-source BFS from all leaves to get distF
	for (int i = 0; i < n; i++) {
		if (sz(adj[i]) == 1) { // leaf in a tree with n>1
			distF[i] = 0;
			q.push(i);
		}
	}
	while (sz(q)) {
		int u = q.ft; q.pop();
		for (int v : adj[u]) {
			if (distF[v] > distF[u] + 1) {
				distF[v] = distF[u] + 1;
				q.push(v);
			}
		}
	}

	int ans = 0;

	// walk the rooted tree at k; cut branches when a farmer can intercept
	V<pi> st;
	st.pb({k, -1});
	while (sz(st)) {
		auto [u, p] = st.bk; st.ppb();

		if (distF[u] <= distB[u]) {
			ans++;
			continue; // farmer blocks here; no need to explore deeper
		}
		for (int v : adj[u]) if (v != p) {
			st.pb({v, u});
		}
	}

	cout << ans << "\n";
}

int main() {
	setIO("atlarge");
	solve(); // single case
}
