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
1) Problem recap in simple terms:
   We have a permutation of cows. In one move, we take the first cow and insert it somewhere later.
   Find how many cows at the front must be moved (k), and for each of those k cows, output where it gets inserted
   (as the number of positions it shifts right in the current array state).

2) Things to notice and step-by-step derivation:
   - The array is already "done" from some point to the end: the longest suffix that is strictly increasing is already sorted.
     Everything before it must be moved -> that count is k.
   - While we process the first k cows from left to right, we maintain a multiset of cows that are already in the "sorted part"
     (initially the increasing suffix, then we insert each moved cow).
   - For current cow x, its insertion index among the sorted part is (#elements < x). But in the current array,
     there are still (k-1-i) unmoved cows sitting in front, so the answer adds that offset.

3) Solution outline:
   - Find k by scanning from the back while it stays increasing.
   - Use a Fenwick tree over values 0..n-1 to count how many inserted values are < x.
   - Initialize Fenwick with the suffix values. For i=0..k-1:
       ans = fenwick.sum(x-1) + (k-1-i); print ans; fenwick.add(x, +1).
*/

struct Fenwick {
	int n;
	vi bit;
	Fenwick(int _n = 0) { init(_n); }
	void init(int _n) {
		n = _n;
		bit.assign(n + 1, 0);
	}
	void add(int i, int v) { // i is 0-indexed
		for (i++; i <= n; i += i & -i) bit[i] += v;
	}
	int sumPrefix(int i) { // sum [0..i], i is 0-indexed
		if (i < 0) return 0;
		int r = 0;
		for (i++; i > 0; i -= i & -i) r += bit[i];
		return r;
	}
};

inline void solve() {
	int n; cin >> n;
	vi a(n);
	for (int i = 0; i < n; i++) {
		cin >> a[i];
		--a[i]; // values 0..n-1
	}

	int suf = n - 1;
	while (suf > 0 && a[suf - 1] < a[suf]) suf--; // first index of increasing suffix
	int k = suf;
	cout << k << '\n';
	if (k == 0) return;

	Fenwick fw(n);
	for (int i = k; i < n; i++) fw.add(a[i], 1);

	for (int i = 0; i < k; i++) {
		int x = a[i];
		int smaller = fw.sumPrefix(x - 1);      // how many inserted values < x
		int ans = smaller + (k - 1 - i);        // offset for remaining unmoved cows
		cout << ans << " \n"[i == k - 1];
		fw.add(x, 1);
	}
}

// can you use a PBDS or a segment tree? yeah. am i gonna use it? no

int main() {
	setIO(); // file i/o doesn't work, for NO reason at all
	solve(); // single case
}