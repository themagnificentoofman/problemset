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
     We have N cows in a line with breeds b[i]. We choose an interval [l, r] (l < r) and
     make cows l and r the leaders. Each leader's breed must not appear anywhere else in
     the interval (including the other leader). Count the number of valid intervals.

 (2) Things to notice and step-by-step derivation:
     - Fix l as the left leader. Let x be the next position to the right with breed b[l]
       (or N if none). Then r must satisfy l < r < x.
     - Also, r's breed must not appear in (l, r), which means r must be the first
       occurrence (to the right of l) of its breed.
     - While scanning from right to left, maintain nextPos[breed] = closest index >= i
       where that breed appears. For each breed, exactly one position is "active".
     - Put a 1 in a Fenwick tree at each active position. For l = i, the valid r are
       exactly the active positions in (i, x), so we can query a range sum.

 (3) Solution outline:
     - Read breeds and convert to 0-indexed breed ids.
     - Scan i from N-1 down to 0:
       - x = nextPos[b[i]]; answer += count of active positions in (i, x).
       - Update nextPos[b[i]] from x to i in the Fenwick tree.
     - Print the total.
     - Time: O(N log N); Memory: O(N).
 */

struct BIT {
	int n;
	vi bit;
	BIT(int n = 0) : n(n), bit(n + 1, 0) {}

	void add(int i, int v) {
		for (i++; i <= n; i += i & -i) bit[i] += v;
	}

	int sumPref(int i) {
		int s = 0;
		for (i++; i > 0; i -= i & -i) s += bit[i];
		return s;
	}

	int sumRange(int l, int r) {
		if (l > r) return 0;
		return sumPref(r) - (l ? sumPref(l - 1) : 0);
	}
};

inline void solve() {
	int n;
	cin >> n;
	vi b(n);
	for (int i = 0; i < n; i++) {
		cin >> b[i];
		b[i]--;
	}

	BIT fw(n);
	vi nextPos(n, n);

	ll ans = 0;
	for (int i = n - 1; i >= 0; i--) {
		int c = b[i];
		int x = nextPos[c];

		ans += fw.sumRange(i + 1, x - 1);

		if (x < n) fw.add(x, -1);
		nextPos[c] = i;
		fw.add(i, 1);
	}

	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
