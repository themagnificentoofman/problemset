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
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // 1st idx with a[idx] >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // 1st idx with a[idx] > b
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
1) Problem recap (simple terms):
   We have a graph on N (N<=16). We consider partitioning vertices into groups.
   Each group contributes a precomputed "flip cost" based on whether pairs inside the group
   are edges or non-edges. We want the minimum total cost over all partitions.

2) Things to notice and step-by-step:
   - Let flip[S] = sum over unordered pairs (i<j) in S of (+1 if edge(i,j) else -1).
   - If we know flip[S] for every subset S, then:
       dp[mask] = min over nonempty sub ⊆ mask: dp[mask \ sub] + flip[sub]
     which is classic "partition DP" over subsets.

3) Solution outline:
   - Read graph, build flip[mask] for all masks using a lowbit transition.
   - Run subset partition DP by enumerating submasks.
   - Time: O(N*2^N + 3^N), which is fine for N<=16.
*/

inline void solve() {
	int N, M;
	cin >> N >> M;

	V<vi> adj(N, vi(N, 0));
	for (int i = 0; i < M; i++) {
		int u, v;
		cin >> u >> v;
		--u; --v;
		adj[u][v] = adj[v][u] = 1;
	}

	int S = 1 << N;

	// val[i][mask] = sum over j in mask of (+1 if edge(i,j) else -1)
	V<vi> val(N, vi(S, 0));
	for (int i = 0; i < N; i++) {
		for (int mask = 1; mask < S; mask++) {
			int b = __builtin_ctz(mask);
			int rest = mask ^ (1 << b);
			val[i][mask] = val[i][rest] + (adj[i][b] ? 1 : -1);
		}
	}

	// flip[mask] = sum over unordered pairs inside mask of (+1 if edge else -1)
	vi flip(S, 0);
	for (int mask = 1; mask < S; mask++) {
		int b = __builtin_ctz(mask);
		int rest = mask ^ (1 << b);
		flip[mask] = flip[rest] + val[b][rest];
	}

	int base = N * (N - 1) / 2 - M;
	vi dp(S, 0x3f3f3f3f);
	dp[0] = base;

	for (int mask = 1; mask < S; mask++) {
		for (int sub = mask; sub; sub = (sub - 1) & mask) {
			dp[mask] = min(dp[mask], dp[mask ^ sub] + flip[sub]);
		}
	}

	cout << dp[S - 1] << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
