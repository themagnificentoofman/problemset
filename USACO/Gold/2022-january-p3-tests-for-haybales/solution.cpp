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
     Given j[1..N], we must construct some K and values a[1..N] such that for each i, j[i] is
     exactly the largest index with a[j[i]] <= a[i] + K.

 (2) Things to notice and step-by-step derivation:
     - Build a tree on nodes 1..N+1 with parent(i) = j[i] + 1, where node N+1 is the root.
     - Along any parent->child edge, we want values to differ by about K, so define
       a[u] = height[u] * K + x[u], where height drops by 1 per edge, and x breaks ties.
     - Choose K = N + 1 so we can assign all x[u] as distinct numbers in [0..K].
     - Do a DFS from the root, visiting children in decreasing index order, and assign x[u]
       as a strictly decreasing counter; this enforces the needed ordering within a layer.

 (3) Solution outline:
     - Read j[i] and add directed edge (j[i] + 1) -> i to form a rooted tree at N+1.
     - Set K = N + 1. Traverse the tree in the required DFS order, assigning:
       height[root] = K, height[child] = height[parent] - 1, and x[u] = next decreasing id.
     - Output K and a[i] = height[i] * K + x[i] for i = 1..N.
     - Time: O(N); Memory: O(N).
 */

inline void solve() {
	int n;
	cin >> n;

	int k = n + 1;
	V<vi> ch(n + 2);
	for (int i = 1; i <= n; i++) {
		int j;
		cin >> j;
		ch[j + 1].pb(i);
	}
	for (int u = 1; u <= n + 1; u++) sor(ch[u]);

	vi x(n + 2, 0);
	vl h(n + 2, 0);
	int ti = k;

	vpi st;
	st.eb(n + 1, k);
	while (sz(st)) {
		auto [u, a] = st.bk;
		st.ppb();
		h[u] = a;
		x[u] = ti--;

		for (int idx = 0; idx < sz(ch[u]); idx++) {
			int v = ch[u][idx];
			st.eb(v, a - 1);
		}
	}

	cout << k << '\n';
	for (int i = 1; i <= n; i++) {
		cout << h[i] * (ll)k + (ll)x[i] << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
