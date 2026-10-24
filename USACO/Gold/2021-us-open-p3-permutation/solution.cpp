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
     We pick an order of the N points. Start by connecting the first 3 into a triangle. For each
     later point, we connect it to every earlier point that we can without crossing any existing
     segments. We need to count how many orders make it so that each new point adds exactly 3
     new segments, modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - Adding exactly 3 segments means the new point must lie strictly inside one existing
       triangular face (then it connects to that face's 3 vertices).
     - Therefore, the outer face always stays a triangle, and the first 3 points must form a
       triangle that contains all points; otherwise the answer is 0.
     - As time goes on, the outer triangle can only shrink by replacing one vertex with an
       interior point, so we get a sequence of nested triangles.
     - Between two nested triangles, the "between" points can appear in any order, except the
       last one (the new vertex) must be last to trigger the triangle change.
     - The needed counting blocks are A(a,b)=C(a,b)*b! (choose b points from a, then order them).

 (3) Solution outline:
     - Enumerate all triangles (a<b<c) and compute inside[a][b][c] = #points inside/on it.
     - Let W be the maximum-area triangle; if inside[W] != N, print 0.
     - Precompute C(n,k), factorials, and A(a,b)=C(a,b)*b! mod.
     - DP over triangles in decreasing area: add "finish here" contribution, then transition by
       replacing one vertex with an interior point p (forming a smaller triangle).
     - Multiply by 6 for the 3! possible orders of the first three points.
     - O(n^4) time, O(n^3) memory.
 */

struct Tri {
	int a, b, c;
	ll ar;
};

inline void solve() {
	int n;
	cin >> n;
	vi x(n), y(n);
	for (int i = 0; i < n; i++) cin >> x[i] >> y[i];

	auto area2 = [&](int a, int b, int c) -> ll {
		ll xa = x[a], ya = y[a];
		ll xb = x[b], yb = y[b];
		ll xc = x[c], yc = y[c];
		ll v = xa * yb + xb * yc + xc * ya - xa * yc - xc * yb - xb * ya;
		return v < 0 ? -v : v;
	};

	auto inTri = [&](int a, int b, int c, int p) -> bool {
		return area2(a, b, c) == area2(a, b, p) + area2(b, c, p) + area2(c, a, p);
	};

	V<Tri> tris;
	tris.reserve((ll)n * (n - 1) * (n - 2) / 6);

	V<V<vi>> inside(n, V<vi>(n, vi(n, 0)));
	for (int a = 0; a < n; a++) {
		for (int b = a + 1; b < n; b++) {
			for (int c = b + 1; c < n; c++) {
				ll ar = area2(a, b, c);
				tris.pb({a, b, c, ar});
				int cnt = 0;
				for (int p = 0; p < n; p++) if (inTri(a, b, c, p)) cnt++;
				inside[a][b][c] = cnt;
			}
		}
	}

	if (!sz(tris)) {
		cout << 0 << '\n';
		return;
	}

	sort(all(tris), [&](const Tri& L, const Tri& R) { return L.ar > R.ar; });

	Tri W = tris[0];
	if (inside[W.a][W.b][W.c] != n) {
		cout << 0 << '\n';
		return;
	}

	V<vl> C(n + 1, vl(n + 1, 0));
	vl fact(n + 1, 1);
	for (int i = 0; i <= n; i++) {
		C[i][0] = 1;
		for (int j = 1; j <= i; j++) C[i][j] = (C[i - 1][j - 1] + C[i - 1][j]) % MOD;
		if (i) fact[i] = (fact[i - 1] * i) % MOD;
	}

	V<vl> A(n + 1, vl(n + 1, 0));
	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= i; j++) A[i][j] = (C[i][j] * fact[j]) % MOD;
	}

	V<V<vl>> dp(n, V<vl>(n, vl(n, 0)));
	dp[W.a][W.b][W.c] = 1;

	ll ans = 0;

	for (auto &t : tris) {
		int a = t.a, b = t.b, c = t.c;
		ll cur = dp[a][b][c];
		if (!cur) continue;

		int Sa = inside[a][b][c];
		ans = (ans + cur * A[n - 3][Sa - 3]) % MOD;

		for (int p = 0; p < n; p++) {
			if (p == a || p == b || p == c) continue;
			if (!inTri(a, b, c, p)) continue;

			int v[3] = {a, b, c};
			for (int j = 0; j < 3; j++) {
				int w[3] = {v[0], v[1], v[2]};
				w[j] = p;
				sort(w, w + 3);
				int d = w[0], e = w[1], f = w[2];

				int Sd = inside[d][e][f];
				if (Sd >= Sa) continue;

				int pool = n - Sd - 1;
				int take = Sa - Sd - 1;
				if (pool < 0 || take < 0 || take > pool) continue;

				ll ways = A[pool][take];
				dp[d][e][f] = (dp[d][e][f] + cur * ways) % MOD;
			}
		}
	}

	ans = (ans * 6) % MOD;
	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
