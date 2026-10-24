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
const int MXN = (int)5e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     There are N cows in a circle. Each cow i has a bucket with capacity a_i and starts full.
     Every minute, each cow pours all milk to the next cow simultaneously. If a cow ends up
     with more than its bucket capacity, the extra milk is lost. For each t = 1..N, output
     the total milk remaining after t minutes.


 (2) Things to notice and step-by-step derivation:
     - After t minutes, the milk sitting at position i comes from i-t and is capped at each
       step, so it equals min(a_i, a_{i-1}, ..., a_{i-t}) (indices mod N).
     - Therefore the total milk after t minutes is the sum of minimums over all cyclic
       windows of length (t+1).
     - As window length grows, each window minimum only decreases at certain “drop points”.
       A monotonic stack can find these drops and add their effect to a whole range of t
       values using a difference array.


 (3) Solution outline:
     - Read a[0..n-1], sum = total initial milk.
     - Rotate so the global minimum capacity is at index n-1 (breaks the circle at a
       convenient point).
     - Scan indices right-to-left with a monotonic stack of increasing capacities.
       When popping a stack index, add its capacity-drop contribution to a contiguous
       range of minutes in a difference array red[].
     - Prefix-sum red[]; for each t=1..n, subtract red[t] from sum and print sum.
     - Time: O(n)  Memory: O(n)
 */

inline void solve() {
	int n;
	cin >> n;

	vi a(n);
	ll sum = 0;
	for (int &x : a) {
		cin >> x;
		sum += x;
	}

	auto it = min_element(all(a));
	rotate(bg(a), next(it), end(a));

	vi st;
	vl red(n + 2);

	for (int i = n - 1; i >= 0; i--) {
		while (sz(st) && a[st.bk] >= a[i]) {
			if (sz(st) > 1) {
				int t1 = st.bk;
				int t2 = st[sz(st) - 2];

				ll delta = (ll)a[t1] - a[t2];
				int start = t2 - t1;
				int len = t1 - i;

				red[start] += delta;
				red[start + len] -= delta;
			}
			st.ppb();
		}
		st.pb(i);
	}

	int i = -1;
	while (sz(st) > 1) {
		int t1 = st.bk;
		int t2 = st[sz(st) - 2];

		ll delta = (ll)a[t1] - a[t2];
		int start = t2 - t1;
		int len = t1 - i;

		red[start] += delta;
		red[start + len] -= delta;
		st.ppb();
	}

	ll cur = 0;
	for (int t = 1; t <= n; t++) {
		cur += red[t];
		sum -= cur;
		cout << sum << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
