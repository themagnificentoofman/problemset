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
     We have a rooted tree, and each node i must be assigned an integer s_i in [l_i, r_i]. The
     imbalance is the maximum |s_i - s_j| where j is an ancestor of i. We want the minimum
     possible imbalance, and if B=1 also output one assignment that achieves it.

 (2) Things to notice and step-by-step derivation:
     - Along any root->v path, let L = max l on the path and R = min r on the path.
     - Any assignment must satisfy imbalance >= max(0, L - R) on that path (two nodes force it).
     - Since root is an ancestor of every node, imbalance also must be large enough so one root
       value can stay within it for every node's interval; this gives
       ceil(max_l_all - min_r_all) / 2 when max_l_all > min_r_all.
     - The minimum answer is the maximum of these two lower bounds, and it is achievable.

 (3) Solution outline:
     - Read the tree; parent indices satisfy p_i < i, so we can process nodes in increasing i.
     - Maintain for each node i:
       - mxLo[i] = max l along the path root->i
       - mnHi[i] = min r along the path root->i
       Update ans with mxLo[i] - mnHi[i].
     - Also compute globalMaxLo = max l over all nodes and globalMinHi = min r over all nodes,
       and update ans with ceil((globalMaxLo - globalMinHi)/2) if that gap is positive.
     - If B=1, pick mid = floor((globalMaxLo + globalMinHi)/2) and set s_i = clamp(mid, l_i, r_i).
       This keeps every root->i path range within ans.
     - Time: O(n) per test case (sum n <= 1e5). Memory: O(n).
 */

inline void solve(int bFlag) {
	int n;
	cin >> n;

	vi p(n + 1, 0);
	for (int i = 2; i <= n; i++) cin >> p[i];

	vl lo(n + 1), hi(n + 1);
	for (int i = 1; i <= n; i++) cin >> lo[i] >> hi[i];

	vl mxLo(n + 1), mnHi(n + 1);
	ll ans = 0;

	ll globalMaxLo = 0;
	ll globalMinHi = BIG;

	for (int i = 1; i <= n; i++) {
		globalMaxLo = max(globalMaxLo, lo[i]);
		globalMinHi = min(globalMinHi, hi[i]);

		if (i == 1) {
			mxLo[i] = lo[i];
			mnHi[i] = hi[i];
		} else {
			mxLo[i] = max(lo[i], mxLo[p[i]]);
			mnHi[i] = min(hi[i], mnHi[p[i]]);
		}

		ans = max(ans, mxLo[i] - mnHi[i]);
	}

	ll globalGap = globalMaxLo - globalMinHi;
	if (globalGap > 0) ans = max(ans, (globalGap + 1) / 2);

	cout << ans << '\n';

	if (bFlag) {
		ll mid = (globalMaxLo + globalMinHi) / 2;
		for (int i = 1; i <= n; i++) {
			ll s = max(lo[i], min(mid, hi[i]));
			cout << s << " \n"[i == n];
		}
	}
}

int main() {
	setIO();
	int t, bFlag; cin >> t >> bFlag;
	while (t--) solve(bFlag);
}
