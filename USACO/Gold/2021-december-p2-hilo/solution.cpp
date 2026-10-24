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
     Given a permutation p[1..n]. For each x in [0..n], Bessie's number is x+0.5 and we run
     Elsie's "HILO" process. Output, for each x, how many times we see a HI immediately
     followed later by a LO (counted over the guesses that are not skipped).

 (2) Things to notice and step-by-step derivation:
     - Work on half-integer boundaries (0.5, 1.5, ..., n+0.5). Each guess v splits an interval.
     - Process guesses in reverse: splitting forward becomes merging backward.
     - When we merge at value v, we may "close" a previously opened split for the same left
       boundary a. That contributes +1 to all x in [last[a], v).
     - We only need range add +1 and final point queries, so a difference array works.

 (3) Solution outline:
     - Maintain a doubly linked list of values 0..n+1 (sentinels included).
     - last[a] stores the last value that merged for left boundary a.
     - Process v = p[n], p[n-1], ..., p[1]:
       - Let a = L[v], b = R[v]. If last[a] exists, do diff[last[a]]++, diff[v]--.
       - Remove v from the list (link a<->b) and set last[a] = v.
     - Prefix-sum diff[0..n] and print.
     - Time: O(n); Memory: O(n).
 */

inline void solve() {
	int n;
	cin >> n;
	vi perm(n);
	for (int i = 0; i < n; i++) cin >> perm[i];

	vi diff(n + 2, 0);
	vi last(n + 1, -1);

	vi L(n + 2), R(n + 2);
	for (int i = 0; i <= n + 1; i++) {
		L[i] = i - 1;
		R[i] = i + 1;
	}

	auto join = [&](int v) {
		int a = L[v], b = R[v];
		if (last[a] != -1) {
			diff[last[a]]++;
			diff[v]--;
		}
		R[a] = b;
		L[b] = a;
		last[a] = v;
	};

	for (int i = n - 1; i >= 0; i--) join(perm[i]);

	int cur = 0;
	for (int x = 0; x <= n; x++) {
		cur += diff[x];
		cout << cur << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
