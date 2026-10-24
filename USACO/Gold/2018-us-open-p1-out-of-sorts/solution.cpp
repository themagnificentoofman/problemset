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
1) Problem recap (simple):
   We run Bessie's “bi-directional bubble sort” loops until the array is sorted. Each outer loop prints "moo".
   Given the input array, output how many times "moo" gets printed.

2) Things to notice + derivation:
   Compare the input array A to its sorted version S.
   For each cut between positions i and i+1, let M_i be how many elements must cross that cut to match S
   (equivalently: among the first i positions, how many belong on the right in the sorted order).
   One full outer iteration can reduce this imbalance by at most 1 for every cut, so we need at least max(M_i)
   iterations, and that many iterations is also sufficient. If all M_i are 0, we still print "moo" once.

3) Solution outline:
   Sort pairs (value, original_index) to represent S stably.
   Scan S from left to right, insert original_index into a Fenwick tree.
   For each i (1..N-1), prefix_sum(i) = how many inserted indices are <= i in the original array.
   Then i - prefix_sum(i) is exactly M_i. Answer is max over i, with minimum 1.
*/

inline void solve() {
	int n; cin >> n;
	vpi a(n+1);
	for (int i = 1; i <= n; i++) {
		int x; cin >> x;
		a[i] = mp(x, i); // store (value, original index); 1-indexing matches BIT nicely
	}
	sor(a); // stable via (value, index) tie-break (ignore i = 0)

	vi bit(n+1, 0);
	auto add = [&](int j, int v) {
		for (; j <= n; j += (j & -j)) bit[j] += v;
	};
	auto pref = [&](int j) {
		int s = 0;
		for (; j > 0; j -= (j & -j)) s += bit[j];
		return s;
	};

	int ans = 1;
	for (int i = 1; i <= n-1; i++) {
		add(a[i].se, 1);          // we've "seen" this element's original position
		ans = max(ans, i - pref(i)); // M_i for the cut after position i
	}
	cout << ans << "\n";
}

int main() {
	setIO("sort");
	solve(); // single case
}