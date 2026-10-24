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
     Given n cows in order with success probabilities p[i], pick a contiguous segment to maximize
     the probability that exactly one cow succeeds. Output floor(1e6 * bestProbability).

 (2) Things to notice and step-by-step derivation:
     - For a segment, P(exactly one) = (∏(1-p[i])) * (Σ p[i]/(1-p[i])).
     - Maintain prod = ∏(1-p[i]) and sum = Σ p[i]/(1-p[i]) for a sliding window [l, r).
     - The best window has sum near 1, so we can two-pointer: expand r while sum < 1, then
       record answer and advance l (removing its contribution).

 (3) Solution outline:
     - Read probabilities as long doubles and store p[i] = pt / 1e6.
     - Use two pointers with (prod, sum) updated in O(1) per move.
     - Track best = max(best, prod * sum), then print floor(best * 1e6).
     - Time: O(n). Memory: O(n).
 */

inline void solve() {
	int n;
	cin >> n;
	vd p(n);
	for (int i = 0; i < n; i++) {
		int pt; cin >> pt;
		p[i] = (db)pt / (db)1000000.0;
	}

	int r = 0;
	db prod = 1, sum = 0;
	db best = 0;

	for (int l = 0; l < n; l++) {
		while (r < n && sum < 1) {
			db q = 1 - p[r];
			prod *= q;
			sum += p[r] / q;
			r++;
		}
		best = max(best, prod * sum);

		if (l < r) {
			db q = 1 - p[l];
			prod /= q;
			sum -= p[l] / q;
		} else {
			r = l + 1;
		}
	}

	ll ans = (ll)(best * (db)1000000.0 + (db)1e-12);
	cout << ans << "\n";
}

int main() {
	setIO("cowdate");
	solve(); // single case
}
