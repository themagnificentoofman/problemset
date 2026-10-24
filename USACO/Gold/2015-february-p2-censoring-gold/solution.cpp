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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better, basically
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
const ll BIG = 4e18; // large enough for ll, not close to INT64_MAX tho
const int MOD = (int)1e9+7; // 998244353;
const int MXN = (int)2e5+5; // modify as needed

/*
1) Problem recap (simple):
   Given a text string S and N “bad” strings, repeatedly delete any bad string that appears
   as a substring while scanning left-to-right (after deletions, new matches can form).
   Output the final censored string.

2) Things to notice + step-by-step:
   - We want to know, after adding each new character to the current result, whether the suffix
     matches any bad string (and if so, how long that match is).
   - Aho-Corasick automaton lets us update the current state in O(1) per character and tells us
     if any pattern ends here.
   - Keep a stack of automaton states aligned with the current result string; when we delete L chars,
     we pop L states and continue from the new top.

3) Solution outline:
   - Build Aho-Corasick over all patterns, storing out[v] = longest pattern length ending at state v
     (including via failure links).
   - Scan S, push chars + states; whenever out[state] = L > 0, pop L.
   - Time: O(|S| + total_pattern_length + alphabet*states) ≈ O(|S| + total_pattern_length).
*/

struct Node {
	AR<int,26> nxt;
	int link = 0;
	int out = 0; // longest matched pattern length ending here
	Node() { nxt.fill(-1); }
};

inline void solve() {
	str s;
	int n;
	cin >> s >> n;

	V<Node> ac;
	ac.reserve(sz(s) + 5);
	ac.eb(); // root

	for (int i = 0; i < n; i++) {
		str t; cin >> t;
		int v = 0;
		for (char ch : t) {
			int c = ch - 'a';
			if (ac[v].nxt[c] == -1) {
				ac[v].nxt[c] = sz(ac);
				ac.eb();
			}
			v = ac[v].nxt[c];
		}
		ac[v].out = max(ac[v].out, sz(t));
	}

	queue<int> q;

	// init root transitions + first layer links
	for (int c = 0; c < 26; c++) {
		int u = ac[0].nxt[c];
		if (u != -1) {
			ac[u].link = 0;
			q.push(u);
		} else {
			ac[0].nxt[c] = 0;
		}
	}

	// BFS for failure links, also build full transitions and propagate out[]
	while (!q.empty()) {
		int v = q.ft; q.pop();

		ac[v].out = max(ac[v].out, ac[ac[v].link].out);

		for (int c = 0; c < 26; c++) {
			int u = ac[v].nxt[c];
			if (u != -1) {
				ac[u].link = ac[ac[v].link].nxt[c];
				q.push(u);
			} else {
				ac[v].nxt[c] = ac[ac[v].link].nxt[c];
			}
		}
	}

	str res;
	res.reserve(sz(s));
	vi st;
	st.reserve(sz(s) + 1);

	int cur = 0;
	st.pb(cur);

	for (char ch : s) {
		int c = ch - 'a';
		cur = ac[cur].nxt[c];

		res.pb(ch);
		st.pb(cur);

		int L = ac[cur].out;
		if (L > 0) {
			for (int k = 0; k < L; k++) {
				res.ppb();
				st.ppb();
			}
			cur = st.bk;
		}
	}

	cout << res << "\n";
}

int main() {
	setIO("censor");
	solve(); // single case
}
