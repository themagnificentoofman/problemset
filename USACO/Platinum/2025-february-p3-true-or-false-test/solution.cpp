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
const ll NEG_BIG = -BIG;

/*
 (1) Problem recap in simple terms:
     Bessie chooses which questions to answer. She gets +a if correct, -b if wrong, 0 if blank.
     Afterward, Elsie may change up to k answered questions so Bessie becomes wrong on them.
     For each k, find the maximum score Bessie can guarantee, while answering at least k questions.

 (2) Things to notice and step-by-step derivation:
     - If a question is flipped, its contribution becomes -b instead of +a, so the score drops by (a+b).
     - For any answered set S, the worst case flips the k questions in S with the largest (a+b).
     - Let i be how many answered questions are NOT flipped. Then:
       - Those i should be the globally smallest (a+b) questions (so they avoid being in the top-k).
       - The flipped k questions can be chosen from the remaining questions; their contributions are -b,
         so we want the k smallest b among the remaining.
     - So for fixed (i,k): score = sum_{first i by (a+b)} a - sum_{k smallest b among remaining} b.

 (3) Solution outline:
     - Sort questions by b to assign ranks; this lets us query "k smallest b" with a segment tree.
     - Sort questions by (a+b). Build prefix sums of a over this order.
     - Build a persistent segment tree over b-ranks for prefixes in (a+b) order; a range diff gives
       counts/sums of b in any suffix.
     - For each k, maximize over i in [0..n-k] using divide-and-conquer optimization on the argmax i.
     - Overall time and memory complexity: O(n log^2 n + q) time, O(n log n) memory.
 */

inline void solve() {
	int n, q;
	cin >> n >> q;

	struct Q {
		ll a, b;
		int id, bRank;
	};
	V<Q> qs(n + 1);
	for (int i = 1; i <= n; i++) {
		cin >> qs[i].a >> qs[i].b;
		qs[i].id = i;
		qs[i].bRank = 0;
	}

	vi ks(q);
	for (int i = 0; i < q; i++) cin >> ks[i];

	// rank questions by b so we can query "k smallest b" via segment tree ranks
	V<Q> byB = qs;
	sort(1 + all(byB), [&](const Q& x, const Q& y) {
		if (x.b != y.b) return x.b < y.b;
		return x.id < y.id;
	});

	vi bRank(n + 1, 0);
	vl sortedB(n + 1, 0);
	for (int i = 1; i <= n; i++) {
		bRank[byB[i].id] = i;
		sortedB[i] = byB[i].b;
	}
	for (int i = 1; i <= n; i++) qs[i].bRank = bRank[qs[i].id];

	// sort by (a+b): these are the ones that are safest from being in the "worst flipped k"
	sort(1 + all(qs), [&](const Q& x, const Q& y) {
		ll sx = x.a + x.b, sy = y.a + y.b;
		if (sx != sy) return sx < sy;
		return x.id < y.id;
	});

	struct SegNode {
		int l = 0, r = 0;
		ll sum = 0;
		int cnt = 0;
	};

	struct PersistentSeg {
		int n;
		const vl* sortedB;
		V<SegNode> seg;

		PersistentSeg(int n_, const vl* sortedB_) : n(n_), sortedB(sortedB_) {
			seg.reserve(n * 20 + 5);
			seg.pb(SegNode()); // node 0 = empty
		}

		int upd(int node, int pos, ll val, int l, int r) {
			int nn = sz(seg);
			seg.pb(seg[node]);
			if (l == r) {
				seg[nn].cnt++;
				seg[nn].sum += val;
				return nn;
			}
			int mid = (l + r) / 2;
			if (pos <= mid) seg[nn].l = upd(seg[nn].l, pos, val, l, mid);
			else seg[nn].r = upd(seg[nn].r, pos, val, mid + 1, r);

			seg[nn].cnt = seg[seg[nn].l].cnt + seg[seg[nn].r].cnt;
			seg[nn].sum = seg[seg[nn].l].sum + seg[seg[nn].r].sum;
			return nn;
		}

		ll kMinSum(int lNode, int rNode, int k, int l, int r) const {
			if (k == 0) return 0;
			if (l == r) return (*sortedB)[l] * (ll)k;

			int mid = (l + r) / 2;
			int lCnt = seg[seg[rNode].l].cnt - seg[seg[lNode].l].cnt;
			if (lCnt >= k) return kMinSum(seg[lNode].l, seg[rNode].l, k, l, mid);

			ll leftSum = seg[seg[rNode].l].sum - seg[seg[lNode].l].sum;
			return leftSum + kMinSum(seg[lNode].r, seg[rNode].r, k - lCnt, mid + 1, r);
		}
	};

	PersistentSeg pst(n, &sortedB);

	vi roots(n + 1, 0);
	vl pref(n + 1, 0), ans(n + 1, NEG_BIG);

	for (int i = 1; i <= n; i++) {
		roots[i] = pst.upd(roots[i - 1], qs[i].bRank, qs[i].b, 1, n);
		pref[i] = pref[i - 1] + qs[i].a;
	}

	auto dc = [&](auto self, int kLo, int kHi, int cutLo, int cutHi) -> void {
		if (kLo > kHi) return;
		int k = (kLo + kHi) / 2;

		int hi = min(cutHi, n - k);
		int bestCut = cutLo;
		ll best = NEG_BIG;

		for (int i = cutLo; i <= hi; i++) {
			ll cost = pst.kMinSum(roots[i], roots[n], k, 1, n);
			ll score = pref[i] - cost;
			if (score >= best) {
				best = score;
				bestCut = i;
			}
		}

		ans[k] = best;
		self(self, kLo, k - 1, bestCut, cutHi);
		self(self, k + 1, kHi, cutLo, bestCut);
	};

	dc(dc, 0, n, 0, n);

	for (int i = 0; i < q; i++) {
		cout << ans[ks[i]] << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
