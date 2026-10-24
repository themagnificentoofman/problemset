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
Problem recap (simple):
- We have M ordering constraints; each constraint is a list like a1 < a2 < ... < ak.
- We want the largest prefix of these constraints (first x constraints) that can all be satisfied.
- Output one valid cow ordering for that maximum x; if multiple, output the lexicographically smallest.

Things to notice + derivation:
- A prefix of constraints works => any smaller prefix also works (monotonic), so we can binary search x.
- For a fixed x, constraints become directed edges ai -> a(i+1). We just need to check if the graph has a topological order.
- If it’s a DAG, the lexicographically smallest valid ordering comes from Kahn’s algorithm using a min-heap.

Solution outline:
- Binary search x (max feasible prefix).
- For each mid, build the graph from constraints [0..mid-1], run lexicographically-smallest topo sort.
- Keep the topo order from the best feasible mid, and print it.
*/

struct Topo {
	int n = 0;
	V<vi> adj;
	vi indeg;

	Topo(int _n = 0) { init(_n); }

	void init(int _n) {
		n = _n;
		adj.assign(n + 1, {});
		indeg.assign(n + 1, 0);
	}

	void ae(int a, int b) {
		adj[a].pb(b);
		indeg[b]++;
	}

	bool sort_lex(vi& out) {
		out.clear();
		pqg<int> q;
		for (int i = 1; i <= n; i++) if (indeg[i] == 0) q.push(i);

		while (sz(q)) {
			int u = q.top(); q.pop();
			out.pb(u);
			for (int v : adj[u]) {
				if (--indeg[v] == 0) q.push(v);
			}
		}
		return sz(out) == n;
	}
};

inline void solve() {
	int n, m;
	cin >> n >> m;
	V<vi> ord(m);
	for (int i = 0; i < m; i++) {
		int k; cin >> k;
		ord[i].rsz(k);
		for (int& x : ord[i]) cin >> x;
	}

	auto chk = [&](int x, vi& out) -> bool {
		Topo T(n);
		for (int i = 0; i < x; i++) {
			for (int j = 0; j + 1 < sz(ord[i]); j++) {
				T.ae(ord[i][j], ord[i][j + 1]);
			}
		}
		return T.sort_lex(out);
	};

	vi best;
	chk(0, best); // always feasible

	int l = 0, r = m;
	while (r - l > 1) {
		int mid = l + (r - l + 1) / 2; // upper-mid for "max true"
		vi cur;
		if (chk(mid, cur)) {
			l = mid;
			best = cur;
		} else {
			r = mid;
		}
	}

	for (int i = 0; i < n; i++) {
		cout << best[i] << (i + 1 == n ? '\n' : ' ');
	}
}

int main() {
	setIO("milkorder");
	solve(); // single case
}
