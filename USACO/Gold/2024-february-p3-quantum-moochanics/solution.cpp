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
template <class T1, class T2> using P = pair<T1, T2>;
#define mp make_pair
#define fi first
#define se second
using pi = P<int, int>;
using pl = P<ll, ll>;
using pd = P<db, db>;

// vectors
template <class T> using V = vector<T>;
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
template <class T, size_t SZ> using AR = array<T, SZ>; // c-style array but better, basically (can use .at())
template <class T> int lwb(V<T> &a, const T &b) { return int(lb(all(a), b) - bg(a)); } // ind of 1st elem >= b
template <class T> int upb(V<T> &a, const T &b) { return int(ub(all(a), b) - bg(a)); } // ind of 1st elem > b
template <class T> using pq = priority_queue<T>;                              // max-heap priority queue (largest on top)
template <class T> using pqg = priority_queue<T, V<T>, greater<T>>;           // min-heap priority queue (smallest on top)

// input & output
inline namespace FileIO {
	void setIn(string s) { freopen(s.c_str(), "r", stdin); }
	void setOut(string s) { freopen(s.c_str(), "w", stdout); }
	void setIO(string s = "") {
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if breaks things
		if (sz(s)) setIn(s + ".in"), setOut(s + ".out");
	}
}

// constants (add more if needed)
const db PI = acos((db)-1);
const ll BIG = 4e18;                 // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9 + 7;        // 998244353;
const int MXN = (int)2e5 + 5;        // modify as needed

/* (1) Problem recap in simple terms:
     We have an even number N of particles on a line, alternating types starting with a mootrino.
     Mootrinos start moving right, antimootrinos start moving left, with given speeds and positions.
     Bessie observes at times 1, then 2 seconds later, then 3 seconds later, etc., and at each
     observation every surviving particle instantly reverses direction (same speed). When a
     mootrino and antimootrino meet, they annihilate. For each particle, output the observation
     number when Bessie will first see it has disappeared.

  (2) Things to notice and step-by-step derivation:
     - Observation k happens after k seconds since observation k-1, so within interval k every
       particle moves a distance speed * k, then flips direction.
     - At observation number t, the net signed displacement is:
         +ceil(t/2) * speed if the particle is moving right on odd t observations,
         -floor(t/2) * speed if it is moving left on even t observations,
       and antimootrinos have the opposite sign pattern. This makes positions at observations
       depend only on ceil(t/2) or floor(t/2), not on t(t+1)/2 directly.
     - For two adjacent particles i (left) and i+1 (right), they always have opposite types.
       Let dist = p[i+1] - p[i], sumSpd = s[i] + s[i+1], need = ceil(dist / sumSpd).
       The earliest observation when they have met is:
         t = 2*need - 1 if i is a mootrino (i even in 0-indexing),
         t = 2*need     if i is an antimootrino (i odd in 0-indexing).
     - A particle can only annihilate with its current neighbor, so we can process meetings as
       events between adjacent alive indices, removing pairs and creating a new adjacent pair.

  (3) Solution outline:
     - For each adjacent pair (i, i+1), compute its meeting observation t and push (t,i,i+1)
       into a min-heap.
     - Maintain a doubly-linked list of alive indices using arrays prv/nxt.
     - Pop events in increasing t; if either endpoint is already gone, skip it.
       Otherwise, set ans[i]=ans[i+1]=t, remove both from the list, and if their neighbors
       become adjacent, push the new meeting event for that neighbor pair.
     - Print ans in original order.
     - Time: O(n log n) per test case (sum n ≤ 2e5), Memory: O(n). */

struct Event {
	ll t;
	int l, r;
	bool operator>(const Event &o) const { return t > o.t; }
};

// meeting observation for adjacent alive particles l (left) and r (right)
static inline ll meetObs(const vl &p, const vl &spd, int l, int r) {
	ll dist = p[r] - p[l];
	ll sumSpd = spd[l] + spd[r];
	ll need = (dist + sumSpd - 1) / sumSpd; // ceil
	return 2 * need - (l % 2 == 0);
}

inline void solve() {
	int n;
	cin >> n;
	vl p(n), spd(n);
	for (int i = 0; i < n; i++) cin >> p[i];
	for (int i = 0; i < n; i++) cin >> spd[i];

	vl ans(n, 0);
	vi prv(n), nxt(n);
	for (int i = 0; i < n; i++) {
		prv[i] = i - 1;
		nxt[i] = (i + 1 == n ? -1 : i + 1);
	}

	pqg<Event> pq;
	for (int i = 0; i + 1 < n; i++) pq.push({meetObs(p, spd, i, i + 1), i, i + 1});

	while (!pq.empty()) {
		auto e = pq.top();
		pq.pop();
		int l = e.l, r = e.r;
		if (ans[l] || ans[r]) continue;

		ans[l] = ans[r] = e.t;

		int L = prv[l];
		int R = nxt[r];

		if (L != -1) nxt[L] = R;
		if (R != -1) prv[R] = L;

		if (L != -1 && R != -1) {
			pq.push({meetObs(p, spd, L, R), L, R});
		}
	}

	for (int i = 0; i < n; i++) cout << ans[i] << " \n"[i == n - 1];
}

int main() {
	setIO();
	int t = 1;
	cin >> t; // (comment out if not needed)
	while (t--) solve();
}
