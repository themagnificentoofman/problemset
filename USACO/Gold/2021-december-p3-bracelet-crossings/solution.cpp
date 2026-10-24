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
     We are given m “snapshots”. Each snapshot lists colors encountered in order, where each color
     should appear either 0 times or exactly twice in that snapshot (open then close). Decide if
     all snapshots can come from a consistent set of properly nested, non-crossing bracelets.

 (2) Things to notice and step-by-step derivation:
     - A color must appear in a contiguous range of snapshots (no disappearing then reappearing).
     - Within one snapshot, opens/closes must be properly nested (stack property).
     - Each color’s immediate container (parent color) must be the same every time it appears.
     - The relative order of colors across snapshots must be consistent (global order merging).

 (3) Solution outline:
     - Track lastRow[color] to enforce contiguity of appearances across snapshots.
     - For each snapshot, scan the sequence and use a stack to validate proper nesting.
     - On each first-open, record/verify parent[color] and collect this snapshot’s open-order list.
     - Merge the snapshot order into a global order, checking intersection consistency.
     - Time: O(total_points * n) (n <= 50); Memory: O(n).
 */

inline void solve() {
	int n, m;
	cin >> n >> m;

	vi lastRow(n + 1, -1);
	vi par(n + 1, -1);

	vi globalOrder;
	vb inGlobal(n + 1, false);

	bool good = true;

	for (int row = 0; row < m; row++) {
		int k;
		cin >> k;

		vi st;
		vi rowOrder;
		st.reserve(k);
		rowOrder.reserve(k);

		int cur = 0;

		for (int i = 0; i < k; i++) {
			int c;
			cin >> c;

			if (!good) continue;

			if (lastRow[c] == row) {
				// second time this row => must be closing, and must match stack top
				if (!sz(st) || st.bk != c) {
					good = false;
					continue;
				}
				st.ppb();
				cur = par[cur];
			} else {
				// first time this row => opening
				if (lastRow[c] != -1 && lastRow[c] != row - 1) {
					good = false;
					continue;
				}
				if (lastRow[c] != -1 && par[c] != cur) {
					good = false;
					continue;
				}

				rowOrder.pb(c);
				par[c] = cur;
				lastRow[c] = row;
				st.pb(c);
				cur = c;
			}
		}

		if (good && sz(st)) good = false;
		if (!good) continue;

		vb inRow(n + 1, false);
		for (int c : rowOrder) inRow[c] = true;

		int ptr = 0;
		for (int c : rowOrder) {
			while (ptr < sz(globalOrder) && !inRow[globalOrder[ptr]]) ptr++;

			if (!inGlobal[c]) {
				globalOrder.ins(bg(globalOrder) + ptr, c);
				inGlobal[c] = true;
			} else {
				if (ptr == sz(globalOrder) || globalOrder[ptr] != c) {
					good = false;
					break;
				}
			}
			ptr++;
		}
	}

	cout << (good ? "YES\n" : "NO\n");
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while(t--) solve();
}
