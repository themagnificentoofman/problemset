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
const int MAX_N = 2000;

/*
 (1) Problem recap in simple terms:
     You have n points (i, h[i]). For each i, some j>i are visible from i: scanning
     rightwards, we keep points whose slope from i is non-decreasing. After each
     update h[x] += y, output the total number of visible pairs (i, j).

 (2) Things to notice and step-by-step derivation:
     - For fixed i, visible points form a chain; if a new point becomes visible,
       it can only erase a suffix of the chain.
     - An update at x only affects chains that start at x or to the left of x.
     - With n <= 2000, store each chain as a bitset and keep the total count.

 (3) Solution outline:
     - Build every start i's visible chain by a scan; sum set bits for the answer.
     - On update at x: for every m<x, insert x if it becomes visible, then delete
       any later chain points hidden by x; adjust the running answer.
     - Rebuild the chain starting at x from scratch.
     - Time: O(n^2) preprocessing, O(n^2) per update; Memory: O(n^2 / 64).
 */

// bitset helper: add _Find_prev (libstdc++ already provides _Find_next/_Find_first)
template <size_t NB> struct Bitset : bitset<NB> {
	template <typename... Args> Bitset(Args... args) : bitset<NB>(args...) {}
	static constexpr int arraySize = (NB + 63) / 64;

	union RawCast {
		AR<uint64_t, arraySize> a;
		Bitset b;
	};

	int _Find_prev(size_t i) const {
		if (i == 0) return -1;
		if ((*this)[--i]) return (int)i;
		size_t m = i / 64;
		const auto &a = ((RawCast *)(this))->a;
		uint64_t buf = a[m] & ((1ull << (i & 63)) - 1);
		if (buf) return (int)(m * 64 + 63 - __builtin_clzll(buf));
		while (m--) {
			if (a[m]) return (int)(m * 64 + 63 - __builtin_clzll(a[m]));
		}
		return -1;
	}
};

inline void solve() {
	int n;
	cin >> n;
	vl h(n);
	for (auto &x : h) cin >> x;

	V<Bitset<MAX_N>> segs(n);

	auto betterSlope = [&](int m, int l, int r) -> bool {
		// returns true iff slope(m->l) <= slope(m->r); allow l==m as a base case
		if (l == m) return true;
		return (ll)(l - m) * (h[r] - h[m]) >= (ll)(r - m) * (h[l] - h[m]);
	};

	auto buildChain = [&](Bitset<MAX_N> &b, int st) -> void {
		int lst = st;
		b.reset();
		for (int i = st + 1; i < n; ++i) {
			if (betterSlope(st, lst, i)) {
				lst = i;
				b[i] = 1;
			}
		}
	};

	ll ans = 0;
	for (int i = 0; i < n; ++i) {
		buildChain(segs[i], i);
		ans += (ll)segs[i].count();
	}

	int q;
	cin >> q;
	while (q--) {
		int x, y;
		cin >> x >> y;
		--x;
		h[x] += y;

		for (int m = 0; m < x; ++m) {
			int it = (int)segs[m]._Find_next(x);     // first visible index strictly > x (or n)
			int l = segs[m]._Find_prev((size_t)it);  // visible just before it (or -1)
			if (l == -1) l = m;

			if (!betterSlope(m, l, x)) continue;

			if (!segs[m][x]) {
				segs[m][x] = 1;
				++ans;
			}

			while (it < n) {
				if (betterSlope(m, x, it)) break;
				int nxt = (int)segs[m]._Find_next(it);
				segs[m][it] = 0;
				--ans;
				it = nxt;
			}
		}

		ans -= (ll)segs[x].count();
		buildChain(segs[x], x);
		ans += (ll)segs[x].count();

		cout << ans << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
