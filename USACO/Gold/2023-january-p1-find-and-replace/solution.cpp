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
     Start with the string "a". Each operation replaces every occurrence of one letter c with a
     nonempty string st (all lowercase). The final string can be huge, but we only need to
     print S[l..r], where r-l+1 <= 2e5.

 (2) Things to notice and step-by-step derivation:
     - Work backwards: after all operations, each letter expands into some (possibly huge) rope.
     - If we process operations from last to first, then c -> st becomes:
       expand[c] = concat(expand[st[0]], expand[st[1]], ...).
     - We only need substring queries, so store each expansion as a binary concat tree with
       subtree sizes clipped at 1e18, and traverse only nodes that intersect [l..r].

 (3) Solution outline:
     - Keep expand[26] as roots of concat trees for each letter (initially each is a leaf).
     - Read operations, apply them in reverse to update expand[c] by concatenating the current
       roots of letters in st (build with a small merge-pairs loop).
     - Finally, iteratively DFS the tree for expand['a'] and output characters in [l..r].
     - Time: O(sum|st| + (r-l+1) * log K) in practice; Memory: O(sum|st|).
 */

struct Node {
	ll sz;
	int l, r;   // children indices, -1 if leaf
	char ch;    // valid only for leaf
};

const ll LIM = (ll)1e18;

inline int newLeaf(V<Node>& pool, char c) {
	pool.pb(Node{1, -1, -1, c});
	return sz(pool) - 1;
}

inline int newMerge(V<Node>& pool, int a, int b) {
	ll s = pool[a].sz + pool[b].sz;
	if (s > LIM) s = LIM;
	pool.pb(Node{s, a, b, 0});
	return sz(pool) - 1;
}

inline void printRange(const V<Node>& pool, int root, ll l, ll r) {
	if (root < 0) return;
	l = max(l, 1LL);
	r = min(r, pool[root].sz);
	if (l > r) return;

	struct Frame { int u; ll l, r; };
	V<Frame> st;
	st.pb({root, l, r});

	while (sz(st)) {
		auto cur = st.bk; st.ppb();
		const Node& u = pool[cur.u];

		if (u.l == -1) {
			cout << u.ch;
			continue;
		}

		ll leftSz = pool[u.l].sz;

		// push right then left so left is processed first
		if (cur.r > leftSz) {
			ll nl = max(1LL, cur.l - leftSz);
			ll nr = cur.r - leftSz;
			if (nl <= nr) st.pb({u.r, nl, nr});
		}
		if (cur.l <= leftSz) {
			ll nl = cur.l;
			ll nr = min(cur.r, leftSz);
			if (nl <= nr) st.pb({u.l, nl, nr});
		}
	}
}

inline void solve() {
	ll l, r;
	int n;
	cin >> l >> r >> n;

	V<P<char,str>> ops(n);
	for (int i = n - 1; i >= 0; i--) cin >> ops[i].fi >> ops[i].se;

	V<Node> pool;
	pool.reserve(26 + 2 * MXN);

	vi expand(26, -1);
	for (char c = 'a'; c <= 'z'; c++) expand[c - 'a'] = newLeaf(pool, c);

	for (int i = 0; i < n; i++) {
		char c = ops[i].fi;
		const str& st = ops[i].se;

		vi parts;
		parts.reserve(sz(st));
		for (char d : st) parts.pb(expand[d - 'a']);

		// merge in pairs to keep trees reasonably balanced
		while (sz(parts) > 1) {
			vi nxt;
			nxt.reserve((sz(parts) + 1) / 2);
			for (int j = 0; j < sz(parts); j += 2) {
				if (j + 1 == sz(parts)) nxt.pb(parts[j]);
				else nxt.pb(newMerge(pool, parts[j], parts[j + 1]));
			}
			parts.swap(nxt);
		}

		expand[c - 'a'] = parts[0];
	}

	printRange(pool, expand[0], l, r);
	cout << '\n';
}

int main() {
	setIO();
	solve(); // single case
}
