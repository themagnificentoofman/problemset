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
const int MXN = (int)3e5+5; // modify as needed

/*
 (1) Problem recap in simple terms:
     We have N pastures in a line, each 'H' or 'G'. We partition into contiguous districts of
     size at most K. A district is "bad" if it is Guernsey-majority or tied. Output the
     minimum possible number of bad districts.

 (2) Things to notice and step-by-step derivation:
     - Map H -> +1, G -> -1, and let pref[i] be the prefix sum up to i.
     - District (j+1..i) is bad iff pref[i] - pref[j] <= 0.
     - DP: dp[i] = minimum bad districts covering the first i pastures.
     - Transition: dp[i] = min_{i-K <= j < i} dp[j] + (pref[i] <= pref[j]).
     - Let m be the minimum dp[j] in the window; then dp[i] is either m or m+1, and we
       only need to know if some pref[j] < pref[i] among those with dp[j] = m.

 (3) Solution outline:
     - Compute pref[0..N].
     - Maintain active j in the last K positions:
       - dpVals: multiset of dp[j] to query m = min dp
       - byDp[v]: multiset of pref[j] for each dp value v, so we can query min pref for m
     - For each i: if minPref(m) < pref[i], set dp[i] = m; else dp[i] = m+1.
     - Insert (dp[i], pref[i]) and remove (dp[i-K], pref[i-K]) when i >= K.
     - Overall time and memory complexity: O(N log N) time, O(N) memory.
 */

inline void solve() {
	int n, k;
	cin >> n >> k;

	str s;
	cin >> s;

	vi pref(n + 1, 0), dp(n + 1, 0);
	for (int i = 0; i < n; i++) {
		pref[i + 1] = pref[i] + (s[i] == 'H' ? 1 : -1);
	}

	multiset<int> dpVals;                // active dp[j] values
	V<multiset<int>> byDp(n + 2);        // byDp[v] stores pref[j] for active j with dp[j] = v

	dpVals.ins(0);
	byDp[0].ins(pref[0]);

	for (int i = 1; i <= n; i++) {
		int m = *dpVals.begin();
		int minPref = *byDp[m].begin();
		dp[i] = (minPref < pref[i] ? m : m + 1);

		dpVals.ins(dp[i]);
		byDp[dp[i]].ins(pref[i]);

		if (i >= k) {
			dpVals.erase(dpVals.find(dp[i - k]));
			byDp[dp[i - k]].erase(byDp[dp[i - k]].find(pref[i - k]));
		}
	}

	cout << dp[n] << "\n";
}

int main() {
	setIO("redistricting");
	solve(); // single case
}
