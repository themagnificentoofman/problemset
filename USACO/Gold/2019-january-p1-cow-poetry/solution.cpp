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
 Problem recap (simple):
 - We have words, each with (syllables, rhyme class). Each line must have exactly K syllables.
 - A poem scheme gives letters A..Z; all lines with the same letter must end in the same rhyme class.
 - Count how many different poems match the scheme (mod 1e9+7).

 Things to notice + derivation:
 - Let dp[x] = #ways to build a sequence of words with exactly x syllables (no rhyme restriction yet).
   Then dp[x] = sum_{s=1..x} dp[x-s] * (number of words with s syllables).
 - For a rhyme class c, the number of valid lines of length K that end in class c is:
   total[c] = sum_{word in class c with syllables s} dp[K - s].
   (Pick the last word from class c, and fill the first K-s syllables any way.)
 - If a scheme letter appears f times, we can choose ANY rhyme class for that letter, contributing:
   sum_c (total[c])^f. Multiply these over all distinct letters.

 Solution outline:
 - Build countSyll[s] and store each word's syllable count inside its rhyme class.
 - Compute dp[0..K].
 - Compute total[c] for each class.
 - Count frequencies of scheme letters, and multiply (sum_c total[c]^freq) for each freq>0.
 */

static inline ll modpow(ll a, ll e) {
	a %= MOD;
	ll r = 1;
	while (e > 0) {
		if (e & 1) r = (r * a) % MOD;
		a = (a * a) % MOD;
		e >>= 1;
	}
	return r;
}

inline void solve() {
	int n, m, k;
	cin >> n >> m >> k;

	vl countSyll(k + 1, 0);     // countSyll[s] = #words with s syllables
	V<vi> cls(n + 1);           // cls[c] = list of syllable counts of words in rhyme class c

	for (int i = 0; i < n; i++) {
		int s, c;
		cin >> s >> c;
		if (s <= k) countSyll[s]++; // syllables are <= k by constraints, but guard anyway
		cls[c].pb(s);
	}

	AR<int, 26> freq{};
	for (int i = 0; i < m; i++) {
		char ch; cin >> ch;
		freq[ch - 'A']++;
	}

	vl dp(k + 1, 0);
	dp[0] = 1;
	for (int i = 1; i <= k; i++) {
		ll ways = 0;
		for (int s = 1; s <= i; s++) {
			if (!countSyll[s]) continue;
			ways = (ways + dp[i - s] * countSyll[s]) % MOD;
		}
		dp[i] = ways;
	}

	vl total(n + 1, 0);
	for (int c = 1; c <= n; c++) {
		ll cur = 0;
		for (int s : cls[c]) {
			if (s <= k) cur = (cur + dp[k - s]) % MOD;
		}
		total[c] = cur;
	}

	ll ans = 1;
	for (int L = 0; L < 26; L++) if (freq[L] > 0) {
		ll sum = 0;
		for (int c = 1; c <= n; c++) {
			if (!total[c]) continue;
			sum = (sum + modpow(total[c], freq[L])) % MOD;
		}
		ans = (ans * sum) % MOD;
	}

	cout << ans << "\n";
}

int main() {
	setIO("poetry");
	solve(); // single case
}