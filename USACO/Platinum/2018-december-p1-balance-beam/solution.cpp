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
const ll SCALE = 100000;

/*
 (1) Problem recap in simple terms:
     Bessie starts at each position i on a line. Each turn she can either take f(i) and stop, or flip
     a fair coin to move left/right. If she falls off the ends, she gets 0. For each start i, output
     floor(1e5 * best expected payment).

 (2) Things to notice and step-by-step derivation:
     - Let V[k] be the optimal expected payment from k. Then V[0]=V[N+1]=0 and
       V[k]=max(f[k], (V[k-1]+V[k+1])/2).
     - Where we choose to keep walking, V must satisfy V[k]=(V[k-1]+V[k+1])/2, so V is linear on
       those intervals.
     - The condition V[k] >= (V[k-1]+V[k+1])/2 means V is concave, so V is the smallest concave
       function that stays above all points (k, f[k]) and also hits (0,0) and (N+1,0).

 (3) Solution outline:
     - Compute the upper convex hull of points (k, f[k]) for k=0..N+1 with a monotone stack.
     - Between consecutive hull vertices (L, f[L]) and (R, f[R]), V is the straight-line
       interpolation, so we can fill answers by one pass.
     - Print floor(SCALE * V[i]) using integer arithmetic.
     - O(N) time, O(N) memory
 */

inline void solve() {
	int n;
	cin >> n;

	vl f(n+2, 0);
	for (int i = 1; i <= n; i++) cin >> f[i];
	f[0] = f[n+1] = 0;

	auto belowLine = [&](int i, int j, int k) -> bool {
		// true if point j lies strictly below the line from i to k
		i128 left = (i128)(k - i) * (i128)f[j];
		i128 right = (i128)(j - i) * (i128)f[k] + (i128)(k - j) * (i128)f[i];
		return left < right;
	};

	vi hull;
	hull.pb(0);
	for (int k = 1; k <= n+1; k++) {
		while (sz(hull) >= 2) {
			int i = hull[sz(hull) - 2];
			int j = hull[sz(hull) - 1];
			if (belowLine(i, j, k)) hull.ppb();
			else break;
		}
		hull.pb(k);
	}

	vi L(n+2, 0), R(n+2, 0);
	for (int t = 0; t + 1 < sz(hull); t++) {
		int a = hull[t], b = hull[t+1];
		L[a] = R[a] = a;
		for (int i = a + 1; i < b; i++) {
			L[i] = a;
			R[i] = b;
		}
	}
	L[n+1] = R[n+1] = n+1;

	for (int i = 1; i <= n; i++) {
		if (L[i] == R[i]) {
			cout << SCALE * f[i] << "\n";
		} else {
			int l = L[i], r = R[i];
			i128 num = (i128)(r - i) * (i128)f[l] + (i128)(i - l) * (i128)f[r];
			i128 den = (i128)(r - l);
			i128 ans = (i128)SCALE * num / den;
			cout << (ll)ans << "\n";
		}
	}
}

int main() {
	setIO("balance");
	solve(); // single case
}
