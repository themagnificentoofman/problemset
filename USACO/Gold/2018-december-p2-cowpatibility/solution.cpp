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

static const int FL = 5;

struct ArrHash {
	// small, fast hash for array<int,5>
	size_t operator()(const AR<int,FL>& a) const {
		uint64_t h = 0x9e3779b97f4a7c15ULL;
		for (int i = 0; i < FL; i++) {
			uint64_t x = (uint64_t)(a[i]) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
			h ^= x;
		}
		return (size_t)h;
	}
};

/*
1) Problem recap in simple terms:
   Each cow has 5 favorite ice cream flavors. Two cows are "compatible" if they share at least one flavor.
   Count how many pairs of cows are NOT compatible (share zero flavors).

2) Things to notice and step-by-step derivation:
   Total pairs = n*(n-1)/2.
   If we can count pairs that share >=1 flavor (compatible), answer = total - compatible.
   Use inclusion-exclusion on subsets of flavors:
     for each non-empty subset S, let cnt[S] = #cows containing S.
     pairs sharing all flavors in S is C(cnt[S], 2).
     compatible = sum_{|S| odd} C(cnt[S],2) - sum_{|S| even} C(cnt[S],2).
   Since each cow has only 5 flavors, we can enumerate all 2^5-1 = 31 non-empty subsets per cow.

3) Solution outline:
   For each cow, sort its 5 flavors, then iterate all subset masks 1..31 (using std::bitset<5> or regular bitmask for picks).
   Build a fixed-size key (array<int,5>, unused slots = 0) and add to a hash map grouped by subset size.
   Run inclusion-exclusion over subset sizes 1..5 to get compatible pairs, then print total-compatible.
*/

inline void solve() {
	int n;
	cin >> n;
	ll total = 1LL * n * (n - 1) / 2;

	V<unordered_map<AR<int,FL>, int, ArrHash>> cnt(FL + 1); // cnt[s][key] for subsets of size s

	for (int i = 0; i < n; i++) {
		AR<int,FL> a{};
		for (int j = 0; j < FL; j++) cin >> a[j];
		sort(all(a));

		for (int mask = 1; mask < (1 << FL); mask++) {
			// bitset<FL> bs(mask);
			// int s = (int)bs.count();
            int s = __builtin_popcount(mask);

			AR<int,FL> key{}; // selected flavors in front, rest 0
			int p = 0;
			// for (int j = 0; j < FL; j++) if (bs[j]) key[p++] = a[j];
            for (int j = 0; j < FL; j++) if (mask & (1 << j)) key[p++] = a[j];

			cnt[s][key]++;
		}
	}

	ll compatible = 0;
	for (int s = 1; s <= FL; s++) {
		for (auto &kv : cnt[s]) {
			ll v = kv.se;
			ll pairs = v * (v - 1) / 2;
			if (s & 1) compatible += pairs;
			else compatible -= pairs;
		}
	}

	cout << (total - compatible) << '\n';
}

int main() {
	setIO("cowpatibility");
	solve(); // single case
}
