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
   We have 2N positions on a circle, each cow label appears exactly twice.
   Count how many pairs of cows have crossing chords (their two occurrences interleave).

2) Things to notice + step-by-step:
   - Scan positions from left to right (the circle is already "cut" into a line in the input).
   - When we see a cow the first time, its chord is "open".
   - When we see it the second time at position i with first position p:
       the chords that cross it are exactly the cows that started in (p, i) and are still open now.

3) Solution outline:
   - Maintain a BIT over positions marking which first-occurrences are currently open (1=open, 0=closed).
   - On first occurrence at i: add(i, +1).
   - On second occurrence with first at p: add to answer = sum(p+1..i-1), then add(p, -1).
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
		int res = 0;
		for (i++; i > 0; i -= i & -i) res += bit[i];
		return res;
	}

	int sum(int l, int r) {
		if (l > r) return 0;
		return sumPrefix(r) - (l ? sumPrefix(l - 1) : 0);
	}
};

inline void solve() {
	int n;
	cin >> n;

	vi a(2 * n);
	for (int i = 0; i < 2 * n; i++) {
		cin >> a[i];
		--a[i];
	}

	vi firstPos(n, -1);
	BIT bit;
	bit.init(2 * n);

	ll ans = 0;
	for (int i = 0; i < 2 * n; i++) {
		int x = a[i];
		if (firstPos[x] == -1) {
			firstPos[x] = i;
			bit.add(i, 1); // mark this chord as open
		} else {
			int p = firstPos[x];
			ans += bit.sum(p + 1, i - 1); // open starts strictly inside (p, i)
			bit.add(p, -1); // close it
		}
	}

	cout << ans << "\n";
}

int main() {
	setIO("circlecross");
	solve(); // single case
}
