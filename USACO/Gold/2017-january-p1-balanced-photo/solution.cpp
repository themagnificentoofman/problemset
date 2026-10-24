// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t;
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
   We have N cows in a line with distinct heights. A cow is "unbalanced" if the number of taller cows
   on one side is more than twice the number of taller cows on the other side. Count unbalanced cows.

2) Things to notice + step-by-step:
   - For each cow i, we only care about cows taller than i on the left and on the right.
   - If we process cows from tallest to shortest, then when we handle cow i,
     all cows we've already added are exactly the cows taller than i.
   - So we just need a data structure that supports:
       add(position), and count of added positions in a prefix.

3) Solution outline:
   - Sort cows by height decreasing (store original positions).
   - Use a BIT over positions to mark which taller cows have been processed.
   - For each cow at position p:
       L = #taller cows in [0..p-1], R = processed - L
       if max(L,R) > 2*min(L,R) -> answer++
       then add(p).
   - Time: O(N log N).
*/

struct BIT {
	int n = 0;
	vi bit;

	void init(int _n) {
		n = _n;
		bit.assign(n + 1, 0);
	}

	void add(int i, int v) {
		for (i++; i <= n; i += i & -i) bit[i] += v;
	}

	int sumPrefix(int i) {
		if (i < 0) return 0;
		int res = 0;
		for (i++; i > 0; i -= i & -i) res += bit[i];
		return res;
	}
};

inline void solve() {
	int n;
	cin >> n;

	V<P<int,int>> cows;
	cows.reserve(n);
	for (int i = 0; i < n; i++) {
		int h; cin >> h;
		cows.pb({h, i});
	}
	sort(rall(cows)); // by height desc (heights are distinct)

	BIT bit;
	bit.init(n);

	int ans = 0;
	int processed = 0;
	for (auto [h, pos] : cows) {
		int L = bit.sumPrefix(pos - 1);
		int R = processed - L;

		int mn = min(L, R), mx = max(L, R);
		if (mx > 2 * mn) ans++;

		bit.add(pos, 1);
		processed++;
	}

	cout << ans << "\n";
}

int main() {
	setIO("bphoto");
	solve(); // single case
}
