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
const int MAXL = 3;
const ll INFLL = BIG;

/*
 (1) Problem recap in simple terms:
     We may flip any character (paying its cost). For each k = 1..floor(N/L), find the minimum total
     cost so the final string contains at least k occurrences of "M" followed by L-1 "O"s.

 (2) Things to notice and step-by-step derivation:
     - Two length-L moos can never overlap (they would force a position to be both 'M' and 'O').
     - So we only need to pick some starting positions, each representing one moo interval of length
       L, with no overlaps.
     - Let mooCost[i] be the cost to make S[i..i+L-1] equal to the moo pattern.
     - We want the whole answer sequence: best[0], best[1], ..., best[floor(N/L)].
       This is a min-plus DP over intervals of fixed length, and the sequences are convex, so we
       can merge them greedily by slopes.

 (3) Solution outline:
     - Precompute mooCost[i] for every start i (0 <= i <= N-L).
     - Use divide-and-conquer on ranges [l,r). For each range, store dp[x][y]:
       the convex cost sequence after deleting x chars on the left and y on the right
       (x,y < L), so the remaining length aligns for moo placements.
     - Merge left/right halves by greedy merging of convex sequences; also handle moos that cross the
       midpoint (at most L-1 possibilities).
     - Output the final dp[0][0] sequence (excluding k=0).
     - Time complexity: O(N log N). Memory complexity: O(N).
 */

struct Info {
	int len = 0;
	AR<AR<vl, MAXL>, MAXL> dp;
};

static int n, lLen;
static str st;
static vi flipCost;
static vl mooCost;

static inline void mergeConvex(vl& out, const vl& a, const vl& b, int shift, ll add) {
	/*
	 out[t] = min(out[t], add) along the greedy merge path of slopes from a and b.
	 a and b are convex sequences encoded as values; their differences are nondecreasing.
	 */
	int needSz = sz(a) + sz(b) - 1 + shift;
	if (sz(out) < needSz) out.rsz(needSz, INFLL);

	int ia = 0, ib = 0;
	for (int t = shift;; t++) {
		out[t] = min(out[t], add);

		if (ia + 1 < sz(a)) {
			if (ib + 1 < sz(b)) {
				ll da = a[ia + 1] - a[ia];
				ll db = b[ib + 1] - b[ib];
				if (da <= db) add += da, ia++;
				else add += db, ib++;
			} else {
				add += a[ia + 1] - a[ia];
				ia++;
			}
		} else if (ib + 1 < sz(b)) {
			add += b[ib + 1] - b[ib];
			ib++;
		} else break;
	}
}

static Info solveRange(int l, int r) {
	Info res;
	res.len = r - l;

	if (lLen == 1 && res.len == 1) {
		res.dp[0][0] = {0, mooCost[l]};
		return res;
	}

	if (res.len < lLen) {
		for (int i = 0; i < lLen; i++) {
			for (int j = 0; j < lLen; j++) {
				if (i + j <= res.len) res.dp[i][j] = {0};
			}
		}
		return res;
	}

	int mid = (l + r) / 2;
	Info left = solveRange(l, mid);
	Info right = solveRange(mid, r);

	for (int i = 0; i < lLen; i++) {
		for (int j = 0; j < lLen; j++) {
			if (i + j > res.len) continue;

			if (i >= left.len) {
				res.dp[i][j] = right.dp[i - left.len][j];
				continue;
			}
			if (j >= right.len) {
				res.dp[i][j] = left.dp[i][j - right.len];
				continue;
			}

			mergeConvex(res.dp[i][j], left.dp[i][0], right.dp[0][j], 0, 0);

			for (int k = 1; k < lLen; k++) {
				if (left.len >= i + k && right.len >= j + (lLen - k)) {
					mergeConvex(res.dp[i][j], left.dp[i][k], right.dp[lLen - k][j], 1,
					            mooCost[mid - k]);
				}
			}
		}
	}
	return res;
}

inline void solve() {
	cin >> lLen >> n;
	cin >> st;
	flipCost.rsz(n);
	for (int i = 0; i < n; i++) cin >> flipCost[i];

	mooCost.clear();
	mooCost.reserve(n);

	auto flipAt = [&](int pos, char want) -> ll {
		return (st[pos] == want) ? 0LL : (ll)flipCost[pos];
	};

	for (int i = 0; i + lLen <= n; i++) {
		ll cur = 0;
		for (int j = 0; j < lLen; j++) {
			char want = (j == 0 ? 'M' : 'O');
			cur += flipAt(i + j, want);
		}
		mooCost.pb(cur);
	}

	vl ans = solveRange(0, n).dp[0][0];
	ans.erase(bg(ans));
	for (ll x : ans) cout << x << "\n";
}

int main() {
	setIO();
	solve(); // single case
}
