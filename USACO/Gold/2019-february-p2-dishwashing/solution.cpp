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
using vi  = V<int>;
using vb  = V<bool>;
using vl  = V<ll>;
using vd  = V<db>;
using vs  = V<str>;
using vpi = V<pi>;
using vpl = V<pl>;
using vpd = V<pd>;

// misc. ops & aliases
#define pf  push_front
#define ppb pop_back
#define ppf pop_front
#define lb  lower_bound
#define ub  upper_bound
template<class T, size_t SZ> using AR = array<T,SZ>;
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b) - bg(a)); }
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b) - bg(a)); }
template<class T> using pq  = priority_queue<T>;
template<class T> using pqg = priority_queue<T,V<T>,greater<T>>;

// input & output
inline namespace FileIO {
	void setIn(string s){ freopen(s.c_str(),"r",stdin); }
	void setOut(string s){ freopen(s.c_str(),"w",stdout); }
	void setIO(string s = ""){
		cin.tie(0)->sync_with_stdio(0);
		cin.exceptions(cin.failbit); // comment out if this ever breaks things
		if (sz(s)) setIn(s+".in"), setOut(s+".out");
	}
}

// constants (add more if needed)
const db PI  = acos((db)-1);
const ll BIG = 4e18;        // large enough for ll
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)2e5+5; // modify as needed

/*
 1) Problem recap in simple terms:
    Plates (with integer labels) arrive one by one. We must place each plate onto some stack so that
    every stack is non-decreasing from top to bottom, and washing always removes plates from the
    leftmost non-empty stack. We want the longest prefix of plates for which this is possible.

 2) Things to notice and step-by-step derivation:
    - Once a plate with label L gets washed (removed), it is impossible to later place any plate
      with label < L: such a plate would have to appear "to the left" of where L was washed.
      We track this with a variable `placed` = largest label already washed.
    - For any label v, there is a unique stack where v can go, determined only by v and the
      previously seen labels. We store this as `base[v]` = the "bottom label" of the stack that v uses.
    - When a new label x arrives and we discover a new stack bottom x, it also becomes the stack
      for all smaller labels directly below it that are still unassigned. So we run j = x, x-1, ...
      while `base[j]` is 0, and set `base[j] = x`. Each j is assigned once overall ⇒ O(n).
    - On stack `base[x]`, labels are stored in strictly decreasing order in a vector. If the top
      element is < x, that plate must be washed before x can sit on this stack, so we pop it and
      update `placed`. We repeat until the stack is empty or its top ≥ x.

 3) Solution outline:
    - Read plates from left to right.
    - If we ever see x < placed, this violates the washing order ⇒ the process fails here.
    - Otherwise, extend the interval of labels that map to the stack with bottom x by filling `base`.
    - On stack `idx = base[x]`, pop while its top (.bk) is < x, updating `placed`.
    - Push x onto this stack.
    - The answer is the first position where we fail (0-based index), or n if we process all plates.
*/

inline void solve() {
	int n;
	cin >> n;

	// base[v] = bottom label of the stack that label v should go onto
	vi base(n + 1, 0);
	// items[b] = stack whose bottom label is b
	V<vi> items(n + 1);

	int placed = 0; // largest label that has already been washed (popped)
	int ans = n;    // default: all plates can be processed

	for (int i = 0; i < n; ++i) {
		int x;
		cin >> x;

		// If x < placed, we'd have to stack a smaller label after washing a larger one -> impossible
		if (x < placed) {
			ans = i; // we successfully placed i plates (0..i-1)
			break;
		}

		// Assign labels that haven't yet been mapped to a stack: they belong to stack with bottom x.
		for (int j = x; j > 0 && !base[j]; --j) {
			base[j] = x;
		}

		int idx = base[x]; // which stack x should go on

		// Before placing x, wash any smaller plates on this stack (they must be removed first).
		while (!items[idx].empty() && items[idx].bk < x) {
			placed = items[idx].bk; // latest washed plate label
			items[idx].ppb();       // pop_back()
		}

		// Now it's safe to place x on this stack.
		items[idx].pb(x);
	}

	cout << ans << '\n';
}

int main() {
	setIO("dishes");
	solve();
}