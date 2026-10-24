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
const int MXN = 755; // modify as needed
const int INF = (int)1e9;

// edge[i][j] is the input a_{i,j} (positive = add cost, negative = remove cost).
static int edge[MXN][MXN];

// pref[i][j] = sum of remove-costs max(-edge[u][v],0) over i<=u<v<=j.
static int pref[MXN][MXN];

// dp[i][j] = min cost to make vertices [i..j] have DFS order i,i+1,...,j when starting dfs(i).
static int dp[MXN][MXN];

/*
 (1) Problem recap in simple terms:
     We can add missing edges (paying a_{i,j}) or remove existing edges (paying -a_{i,j}). Find the
     minimum total cost so that there exists some ordering of adjacency lists where DFS(1) visits
     vertices in the exact order 1,2,...,N.

 (2) Things to notice and step-by-step derivation:
     - With DFS order fixed to 1..N, every subtree is a contiguous interval of vertices.
     - For an interval [i..j], after visiting i, DFS must fully finish a first child interval
       [i+1..k] before it can ever reach k+1.
     - So we need edge (i,k+1) to exist (pay max(edge[i][k+1],0) to add it).
     - Also, no vertex in [i+1..k] may have an edge to any vertex in [k+1..j], otherwise DFS could
       jump across and break the order. Those crossing existing edges must be removed.
     - We can precompute the total "remove crossing edges" cost using a 2D prefix sum on the upper
       triangle.

 (3) Solution outline:
     - Build pref[i][j] = total removal cost of edges inside interval [i..j].
     - Interval DP: dp[i][j] = min over split k ( dp[i][k] + dp[k+1][j]
       + add cost for (i,k+1) + remove cost for all edges between [i+1..k] and [k+1..j] ).
     - Answer is dp[0][N-1].
     - Time complexity: O(N^3). Memory complexity: O(N^2).
 */

static inline int cutCost(int l, int m, int r) {
	// remove-cost of all edges between left [l..m-1] and right [m..r]
	if (l >= m || m > r) return 0;
	return pref[l][r] - pref[l][m - 1] - pref[m][r];
}

inline void solve() {
	int n;
	cin >> n;

	for (int i = 0; i < n; i++) {
		dp[i][i] = 0;
		pref[i][i] = 0;
	}

	for (int i = 1; i < n; i++) {
		for (int j = 0; j < i; j++) {
			cin >> edge[j][i];
			edge[i][j] = edge[j][i];
		}
	}

	for (int i = n - 1; i >= 0; i--) {
		for (int j = i + 1; j < n; j++) {
			int rm = max(-edge[i][j], 0);
			pref[i][j] = pref[i + 1][j] + pref[i][j - 1] - pref[i + 1][j - 1] + rm;

			int best = INF;
			for (int k = i; k < j; k++) {
				int add = max(edge[i][k + 1], 0);
				int cross = cutCost(i + 1, k + 1, j);
				best = min(best, dp[i][k] + dp[k + 1][j] + add + cross);
			}
			dp[i][j] = best;
		}
	}

	cout << dp[0][n - 1] << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
