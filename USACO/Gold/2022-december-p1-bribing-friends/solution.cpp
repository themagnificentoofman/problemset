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
     You have some dollars and some cones. Bribing friend i gives p points and costs c dollars,
     but each $1 can be paid using either 1 dollar or x cones. Maximize total points.

 (2) Things to notice and step-by-step derivation:
     - Using cones is best on friends with smaller x, so sort friends by x increasing.
     - Track remaining resources with one number j in [0..money+cones]:
       * if j <= money: you have j dollars and 0 cones
       * if j >  money: you have money dollars and (j-money) cones
     - When j > money and cones aren't enough for this friend, leftover cones < x are useless
       later (since future x is >= current x), so we can drop them and keep only dollars.

 (3) Solution outline:
     - Sort friends by x.
     - 1D DP over j: dp[j] = best points with compressed remainder j.
     - Transition per friend: skip, or bribe using (all cones), (some cones + dollars),
       or (all dollars) depending on j.
     - Time: O(n * (money+cones)); Memory: O(money+cones).
 */

struct Friend {
	int x, p, c;
    bool operator<(const Friend& o) {
        if (x != o.x) return x < o.x;
        return p > o.p;
    }
};

inline void solve() {
	int n, money, cones;
	cin >> n >> money >> cones;

	V<Friend> friends(n);
	for (int i = 0; i < n; i++) cin >> friends[i].p >> friends[i].c >> friends[i].x;
	sor(friends);

	int cap = money + cones;
	vi dp(cap + 1, -1), nxt(cap + 1, -1);
	dp[cap] = 0;

	for (int i = 0; i < n; i++) {
		fill(all(nxt), -1);
		auto [x, p, c] = friends[i];
		ll need = 1LL * c * x;

		for (int j = 0; j <= cap; j++) {
			if (dp[j] < 0) continue;

			// skip
			nxt[j] = max(nxt[j], dp[j]);

			// take
			if (j > money) {
				int conesHave = j - money;

				// pay fully with cones
				if ((ll)conesHave >= need) {
					int j2 = j - (int)need;
					nxt[j2] = max(nxt[j2], dp[j] + p);
				} else {
					// use all cones, pay the rest with dollars, drop leftover cones (< x)
					int cover = conesHave / x;
					int costLeft = c - cover;
					if (money >= costLeft) {
						int j2 = money - costLeft;
						nxt[j2] = max(nxt[j2], dp[j] + p);
					}
				}
			} else {
				// pay fully with dollars
				if (j >= c) nxt[j - c] = max(nxt[j - c], dp[j] + p);
			}
		}
		dp.swap(nxt);
	}

	int ans = 0;
	for (int j = 0; j <= cap; j++) ans = max(ans, dp[j]);
	cout << ans << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
