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
const int SIG = 2;

/*
 (1) Problem recap in simple terms:
     We are given N binary strings (not necessarily distinct). We may append extra bits to each string
     (keeping it as a prefix) so that all resulting strings are distinct. Find the minimum possible
     total number of appended bits.

 (2) Things to notice and step-by-step derivation:
     - For any trie node (prefix), its subtree contains "slots" for distinct completions at various
       extra lengths (0,1,2,...).
     - If we know how many slots exist at each extra length in the left and right child, then for the
       parent we add them and shift lengths by +1.
     - We only ever need the shortest N slots globally; longer ones can be discarded.
     - If cnt strings end at this prefix, we greedily assign them to the shortest available slots in
       this subtree, adding (extra length) to the answer.

 (3) Solution outline:
     - Build a trie of all input strings; each node stores how many strings end there.
     - DP per node stores a vector slots[d] = number of available slots at extra length d, plus a
       running cost for assignments already made in the subtree.
     - Postorder: dp(node) = merge(dp(left), dp(right)) (shift lengths), then take(cntEnd) shortest
       slots to satisfy duplicates ending here.
     - Answer is the cost at the root.
     - Time complexity: O(M log N). Memory complexity: O(M log N), where M is total trie nodes.
 */

static int n;

static vi mergeSlots(const vi& a, const vi& b) {
	vi res = a;
	if (sz(res) < sz(b)) res.rsz(sz(b));
	for (int i = 0; i < sz(b); i++) res[i] += b[i];

	// shift lengths by +1 when moving up one trie edge
	res.ins(bg(res), 0);

	ll sum = 0;
	for (int x : res) sum += x;

	for (;;) {
		if (!sz(res)) break;
		if (sum - res.bk < n) break;
		sum -= res.bk;
		res.ppb();
	}
	return res;
}

struct Dp {
	ll cost = 0;
	vi slots; // slots[d] = how many available strings at extra length d

	void take(int x) {
		for (int d = 0; d < sz(slots) && x > 0; d++) {
			int cnt = min(x, slots[d]);
			cost += 1LL * cnt * d;
			x -= cnt;
			slots[d] -= cnt;
		}
		assert(x == 0);
	}
};

static Dp emptyDp() {
	// slots for a completely empty subtree (full binary tree), trimmed to only what we need for N
	vi slots = {1, 0};
	ll sum = 1;

	for (int depth = 2; sum < n; depth++) {
		int x = 1 << (depth - 2);
		slots.pb(x);
		sum += x;
	}
	return {0, slots};
}

static Dp mergeDp(const Dp& a, const Dp& b) {
	Dp res;
	res.cost = a.cost + b.cost;
	res.slots = mergeSlots(a.slots, b.slots);
	return res;
}

struct Node {
	int endCnt = 0;
	AR<int, SIG> ch{0, 0};
	Dp dp;
};

inline void solve() {
	cin >> n;

	V<Node> nodes;
	nodes.rsz(2); // 0 = special "empty dp", 1 = root
	int nxt = 1;

	for (int i = 0; i < n; i++) {
		str st;
		cin >> st;
		int cur = 1;
		for (char c : st) {
			int b = c - '0';
			if (!nodes[cur].ch[b]) {
				nodes[cur].ch[b] = ++nxt;
				nodes.eb();
			}
			cur = nodes[cur].ch[b];
		}
		nodes[cur].endCnt++;
	}

	nodes[0].dp = emptyDp();

	auto pull = [&](int u) -> void {
		int l = nodes[u].ch[0], r = nodes[u].ch[1];
		if (!l && !r) nodes[u].dp = nodes[0].dp;
		else nodes[u].dp = mergeDp(nodes[l].dp, nodes[r].dp);
		nodes[u].dp.take(nodes[u].endCnt);
	};

	for (int u = nxt; u >= 1; u--) pull(u);

	cout << nodes[1].dp.cost << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
