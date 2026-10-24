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
		cin.exceptions(cin.failbit);
		if (sz(s)) setIn(s+".in"), setOut(s+".out");
	}
}

/*
 (1) Problem recap in simple terms.
    We must compute (mod M) the sum of all step counts K that can occur for some permutation of length N.
    A permutation’s step count equals the lcm of its cycle lengths.

 (2) Things to notice and step-by-step derivation.
    • If K = ∏ p^{e}, the *minimum* n that realizes period K is n_min(K) = ∑ p^{e}.
      (Use disjoint cycles of lengths p^{e}; never share factors and never split a coprime product.)
    • Therefore, K is achievable for length N iff ∑ p^{e} ≤ N.
    • We just need the sum of all such K: sum over choosing, for each prime p ≤ N, either no power,
      or exactly one power p^1, p^2, ... as long as the total sum of chosen powers ≤ N.
    • This is a knapsack-by-primes where each item group (prime p) offers options {0, p^1, p^2, ...}.
      Value contributed by choosing p^k is p^k (since we are summing K), and total K is the product
      over primes of chosen p^k; the DP naturally multiplies by p^k when we pick that option.

 (3) Solution outline.
    • Sieve primes up to N.
    • DP by prime groups with two 1D arrays (prev, cur) of size N+1:
        prev[j] = sum of K over all selections using first i primes with total “power-sum” exactly j.
      Initialize prev[j]=1 for all j (choose nothing ⇒ K=1).
      For each prime p:
        cur = prev (taking the “choose 0 of p” option),
        then for each power pp = p, p^2, ..., and for j from pp..N:
            cur[j] += (pp % M) * prev[j-pp]  (mod M).
      Answer = sum_{j=0..N} prev[j] at the end, but editorial sets res[0][j]=1 and returns res[..][N];
      equivalently, keep arrays as “exactly j” and output prev[N].
    • Complexity: O(N * #primes + Σ N/p^k) = O(N log N) for N ≤ 1e4.
*/

// ---------- helpers ----------
static inline ll addmod(ll a, ll b, ll MOD) { a += b; if (a >= MOD) a -= MOD; return a; }
static inline ll mulmod(ll a, ll b, ll MOD) { return (ll)((i128)a * (i128)b % MOD); }

// sieve primes up to n
static vi sieve(int n) {
	vi primes; vb comp(n+1);
	for (int i = 2; i <= n; i++) {
		if (!comp[i]) {
			primes.pb(i);
			if (1LL*i*i <= n) for (ll j = 1LL*i*i; j <= n; j += i) comp[j] = true;
		}
	}
	return primes;
}

inline void solve() {
	int N; ll MOD; 
	cin >> N >> MOD;

	vi primes = sieve(N);

	// prev[j] = sum of K using processed primes with total sum exactly j.
	vl prev(N+1, 1 % MOD), cur(N+1, 0); // base: choose nothing => K=1 for every capacity j

	for (int p : primes) {
		// start from "choose 0 of p"
		for (int j = 0; j <= N; ++j) cur[j] = prev[j];

		// try one power of p (p^1, p^2, ...) — at most one per prime
		for (ll pp = p; pp <= N; pp *= p) {
			for (int j = (int)pp; j <= N; ++j) {
				// add ways where we spend pp capacity on this prime and multiply K by pp
				cur[j] = addmod(cur[j], mulmod((ll)(pp % MOD), prev[j - (int)pp], MOD), MOD);
			}
			if (pp > (ll)N / p) break; // avoid overflow
		}

		swap(prev, cur);
	}

	// result is total sum for capacity exactly N
	cout << prev[N] % MOD << '\n';
}

int main() {
	setIO(); 
	solve(); // single case
}