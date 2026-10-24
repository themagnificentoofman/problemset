// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default)
using db = long double;  // change to double if TL is tight (almost never)

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
const ll BIG = 4e18; // large enough for ll
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple terms):
   You have n bottles with milk amounts. Each day: add A to the A smallest bottles,
   and steal B from the B largest bottles, then re-sort. After D days, output
   sum(milk[i]^2) mod 1e9+7.

2) Things to notice and step-by-step derivation:
   - The middle band of size (A-B) always gets a guaranteed +1 net per day.
   - After sorting, we can track where this band sits: indices [n-A .. n-B-1].
   - The tricky part is how the “right edge” of this band levels with equal neighbors;
     we maintain a plateau of equal values that contains the right edge and jump many days
     at once until the plateau can expand left/right.

3) Solution outline:
   - Sort milk.
   - Maintain band [l..r] and a plateau [ql..qr] (ql<=r<=qr), plus (k,num,p) for days done,
     plateau base value, and remainder for uneven distribution.
   - Jump in batches until k==D, then write back final values and compute sum of squares.
   - Time: O(n log n) for sort + near-linear plateau updates; overall O(n log n).
*/

// ceil(a/b) for a>=0, b>0
static inline ll ceildiv(ll a, ll b) { return (a + b - 1) / b; }

inline void solve() {
	int n;
	ll d, A, B;
	cin >> n >> d >> A >> B;

	vl milk(n);
	for (int i = 0; i < n; i++) cin >> milk[i];
	sor(milk);

	int l = n - (int)A;
	int r = n - (int)B - 1; // inclusive

	// plateau [ql..qr] always contains r, and milk[ql..qr] are equal to current "num" (ignoring +k offsets)
	int ql = r, qr = r;
	while (ql > l && milk[ql - 1] == milk[r]) ql--;
	while (qr + 1 < n && milk[qr + 1] == milk[r]) qr++;

	ll k = 0;        // full days already processed
	ll num = milk[r];
	ll p = 0;        // remainder for uneven distribution inside [ql..qr]

	while (true) {
		ll w = (qr - ql + 1);      // plateau width
		ll in = (r - ql + 1);      // guaranteed +1 per day count feeding into plateau

		// days until plateau base reaches next right value (so we can extend qr)
		ll numx = BIG;
		if (qr + 1 < n) {
			ll need = (milk[qr + 1] - num) * w - p;
			// need > 0 under invariants; keep as-is to match original logic
			numx = ceildiv(need, in);
		}

		// days until we can absorb one more from the left (ql--)
		ll numy = BIG;
		if (ql > l && r != qr) {
			// derived from balancing growth between left side and right-only part
			numy = (w * (num - (milk[ql - 1] + k)) + p) / (qr - r);
		}

		ll t = min(numx, numy);
		if (k + t > d) break;

		ll add = t * in + p;
		num += add / w;
		p = add % w;
		k += t;

		while (ql > l && milk[ql - 1] + k == num) ql--;
		while (qr + 1 < n && milk[qr + 1] == num) qr++;
	}

	// final partial batch for remaining days
	ll remDays = d - k;
	ll w = (qr - ql + 1);
	ll in = (r - ql + 1);

	ll total = remDays * in + p;
	ll addEach = total / w;
	ll rem = total % w;

	num += addEach;
	p = rem;
	k = d;

	// indices [l..ql-1] gained +d for sure
	for (int i = l; i < ql; i++) milk[i] += d;

	// plateau [ql..qr] ends at num with p of them at num+1 (give +1 to rightmost p)
	for (int i = ql; i <= qr; i++) {
		milk[i] = (qr - i < p ? num + 1 : num);
	}

	// compute sum of squares
	ll ans = 0;
	for (int i = 0; i < n; i++) {
		ll v = milk[i] % MOD;
		ans = (ans + (v * v) % MOD) % MOD;
	}
	cout << ans << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
