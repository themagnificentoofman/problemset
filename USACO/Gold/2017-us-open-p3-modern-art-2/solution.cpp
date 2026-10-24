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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   Given the final 1D painting (colors 0..N, where 0 is blank), find the minimum number of rounds
   needed if each real color can be painted in one interval total, or output -1 if impossible.

2) Key idea / derivation:
   For each color c, its painted interval must cover from first(c) to last(c).
   When scanning left to right, active intervals must be perfectly nested (stack order).
   If we ever see a color that is not the top of the stack, that means two intervals partially
   intersect -> impossible. The answer is the maximum stack depth (excluding color 0).

3) Solution outline:
   - Compute first/last occurrence for each color.
   - Scan positions i=0..N-1 with a stack initialized to [0].
     * If i == first(color[i]) push it; update best depth.
     * If color[i] != stack.top() => -1.
     * If i == last(color[i]) pop it.
   - Output bestDepth - 1 (exclude background 0). Time: O(N), Space: O(N).
*/

inline void solve() {
	int n; cin >> n;
	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	const int INF = (int)1e9;
	vi first(n+1, INF), last(n+1, -1);

	// record first/last position where each color appears
	for (int i = 0; i < n; i++) {
		first[a[i]] = min(first[a[i]], i);
		last[a[i]] = max(last[a[i]], i);
	}

	// treat 0 as the background interval that is always "active"
	first[0] = -1;
	last[0] = n;

	stack<int> st;
	st.push(0);

	int best = 1; // includes background
	for (int i = 0; i < n; i++) {
		int c = a[i];

		// entering a color's overall interval
		if (i == first[c]) {
			st.push(c);
			best = max(best, sz(st));
		}

		// must match the current top interval (no partial intersections)
		if (st.empty() || st.top() != c) {
			cout << -1 << "\n";
			return;
		}

		// leaving a color's overall interval
		if (i == last[c]) st.pop();
	}

	cout << best - 1 << "\n"; // exclude background
}

int main() {
	setIO("art2");
	solve(); // single case
}
