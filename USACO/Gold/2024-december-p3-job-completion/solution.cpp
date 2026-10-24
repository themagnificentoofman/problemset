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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
Job Completion: You have N jobs; job i takes t_i time, and must be started by time s_i
(start time <= s_i). Starting at time 0, you run chosen jobs one-by-one (no overlap).
Pick and order a subset to maximize how many jobs you complete.

Things to notice and step-by-step derivation:
- “Start by s_i” is equivalent to “finish by (s_i + t_i)”, since finish = start + t_i.
- So each job becomes (duration = t_i, deadline = d_i = s_i + t_i).
- This is the classic “maximize #jobs done before deadlines” problem.

Solution outline:
- Sort jobs by increasing deadline d.
- Sweep in that order; tentatively take each job, tracking total time sum.
- If sum exceeds the current deadline, drop one taken job with the largest duration.
- The heap size at the end is the maximum number of jobs doable.
- Time: O(n log n), Memory: O(n).
*/

inline void solve() {
	int n; cin >> n;
	vpl jobs; jobs.rsz(n);

	for (int i = 0; i < n; i++) {
		ll s, t; cin >> s >> t;
		jobs[i] = mp(s + t, t); // (deadline, duration)
	}

	sor(jobs);

	ll sumTime = 0;
	pq<ll> dur;
	for (auto [d, t] : jobs) {
		dur.push(t);
		sumTime += t;
		if (sumTime > d) {
			sumTime -= dur.top();
			dur.pop();
		}
	}

	cout << sz(dur) << '\n';
}

int main() {
	setIO();
	int t = 1; cin >> t; // (comment out if not needed)
	while(t--) solve();
}
