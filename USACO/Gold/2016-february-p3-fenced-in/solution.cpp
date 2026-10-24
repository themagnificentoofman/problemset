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
We have a rectangle cut by vertical and horizontal fences into (n+1)(m+1) regions. We want the minimum total fence length to remove so all regions become connected.

2) Key idea + step-by-step:
Think of each region as a node in a grid graph. Removing one fence segment connects two adjacent regions with “cost = segment length”.
All horizontal-adjacent edges in the same row share the same weight (that row’s height gap), and all vertical-adjacent edges in the same column share the same weight (that column’s width gap).
After sorting the width gaps (dx) and height gaps (dy), we can simulate Kruskal without building all edges by counting how many components remain in the other dimension.

3) Solution outline:
- Build dx gaps from sorted x-coordinates (include 0 and A), and dy gaps from sorted y-coordinates (include 0 and B).
- Sort dx and dy, then run the counting-Kruskal sweep to accumulate the MST cost.
Time: O((n+m) log(n+m)).
*/

inline void solve() {
	int A, B, n, m;
	cin >> A >> B >> n >> m;

	vi xs(n + 2), ys(m + 2);
	xs[0] = 0; xs[n + 1] = A;
	ys[0] = 0; ys[m + 1] = B;

	for (int i = 1; i <= n; i++) cin >> xs[i];
	for (int i = 1; i <= m; i++) cin >> ys[i];

	sor(xs);
	sor(ys);

	vi dx(n + 1), dy(m + 1);
	for (int i = 1; i <= n + 1; i++) dx[i - 1] = xs[i] - xs[i - 1];
	for (int j = 1; j <= m + 1; j++) dy[j - 1] = ys[j] - ys[j - 1];

	sor(dx);
	sor(dy);

	int N = n + 1; // #columns of regions
	int M = m + 1; // #rows of regions

	if (N == 1 && M == 1) {
		cout << 0 << "\n";
		return;
	}

	ll ans = 1LL * dx[0] * (M - 1) + 1LL * dy[0] * (N - 1);

	int vii = 1, hii = 1;
	while (vii < N && hii < M) {
		if (dx[vii] < dy[hii]) {
			ans += 1LL * dx[vii] * (M - hii);
			vii++;
		} else {
			ans += 1LL * dy[hii] * (N - vii);
			hii++;
		}
	}

	cout << ans << "\n";
}

int main() {
	setIO("fencedin");
	solve(); // single case
}
