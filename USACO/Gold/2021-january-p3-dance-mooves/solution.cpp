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
 (1) problem recap in simple terms
     - N cows labeled by their starting positions 1..N.
     - We have K swaps (A[i], B[i]) in order; this sequence repeats cyclically.
     - We perform swaps for a total of M minutes → M total swaps.
     - For each original cow u (starting at position u), output how many DISTINCT positions
       it ever stands on during these M swaps (include its start).

 (2) things to notice and step-by-step derivation
     - One sequence of K swaps induces a permutation p on {1..N}:
         p[u] = the position where cow u ends up after the K swaps.
       Repeating sequences means repeatedly applying p → cows move along cycles of p.
     - During a single sequence (K swaps), each cow u visits positions at times t ∈ [0..K].
       Let S[u] be the increasing-by-time list of pairs (pos, t) that u visits in the first sequence.
       Include (start_pos=u, t=0) so the starting position counts.
     - Over all M swaps, write D = floor(M / K), R = M % K.
       The visited set for cow u equals:
         union of the D full-block visit lists along the cycle: S[u], S[p(u)], ..., S[p^{D-1}(u)]
         (take entries with time ≤ K),
         plus the partial list S[p^D(u)] but only entries with time ≤ R.
     - If D ≥ cycle_length, the full-block union already covers the entire cycle’s positions,
       so the remainder cannot add **new positions** → set R = 0.

 (3) solution outline
     1) Simulate the first block:
        • Maintain fromPos[pos] = which original cow is currently at pos.
        • For each swap at time t (1..K), append (other_pos, t) to the moving cow’s S[•],
          and swap fromPos at the two positions.
        • Also push the starting (pos, 0) for each cow.
     2) Build permutation p: p[originCow] = final position after one block (read from fromPos).
     3) Decompose p into cycles over labels 1..N (labels = **original cow IDs**; we mapped them
        to final positions via p). For each cycle:
        • Let L = cycle length, D = min(floor(M/K), L), R = (D == L ? 0 : M % K).
        • Maintain a sliding window of D consecutive cycle nodes (full blocks),
          and temporarily add a "remainder" node for the partial block.
        • Use cnt[pos] and uniq = # positions with cnt[pos] > 0. Adding/removing S[node] up to a
          time cutoff (K or R) updates cnt and uniq.
        • Slide the window around the cycle and record uniq for each start node.

 (4) worked example (small, illustrative)
     Example: N=6, K=2, M=5. Swaps: (1,2), (2,3).
     First-block simulation (time t=1..2):
       - Start (t=0):
           S[1]: (1,0)   S[2]: (2,0)   S[3]: (3,0)   S[4]: (4,0)   S[5]: (5,0)   S[6]: (6,0)
       - t=1 swap(1,2):
           cow1 moves to pos2 → S[1] += (2,1)
           cow2 moves to pos1 → S[2] += (1,1)
           state: [2,1,3,4,5,6]
       - t=2 swap(2,3):
           cow1 (at pos2) → pos3 → S[1] += (3,2)
           cow3 (at pos3) → pos2 → S[3] += (2,2)
           final state: [2,3,1,4,5,6]
     Build p via final positions:
         pos1 has cow2  → p[2] = 1
         pos2 has cow3  → p[3] = 2
         pos3 has cow1  → p[1] = 3
         pos4 has cow4  → p[4] = 4
         pos5 has cow5  → p[5] = 5
         pos6 has cow6  → p[6] = 6
       Cycles: (1 → 3 → 2 → 1) of length L=3, and fixed points (4), (5), (6).
     D = floor(M/K) = floor(5/2) = 2, R = 1 (< K).
     Consider the 3-cycle [1,3,2] (traverse by p):
       - Seed window with D=2 full blocks: S[1] and S[3], cutoff K=2:
           S[1]: {1 (0), 2 (1), 3 (2)}, S[3]: {3 (0), 2 (2)} → union {1,2,3} → uniq=3
       - For node 1 (i=0):
           remainder index = p^D(1) = cyc[(0+2)%3] = 2
           Add S[2] up to R=1: {2 (0), 1 (1)} → union still {1,2,3} → ans[1]=3
           Remove remainder; slide window: remove S[1], add full S[2] (K=2)
       - For node 3 (i=1):
           remainder index = cyc[(1+2)%3] = 1
           Add S[1] up to R=1: {1 (0), 2 (1)} → union {1,2,3} → ans[3]=3
           Remove remainder; slide: remove S[3], add full S[1]
       - For node 2 (i=2):
           remainder index = cyc[(2+2)%3] = 3
           Add S[3] up to R=1: {3 (0)} → union {1,2,3} → ans[2]=3
     Fixed points 4,5,6 never move within a block and remain fixed by p → each visits one position.

     Complexity note:
       Sum_u |S[u]| = N (starts) + 2K (two entries per minute), so the sliding window’s
       add/remove is linear in N + K overall.
*/

// add S[node] up to time <= cutoff
static inline void add_with_cutoff(
	const V<vpi>& S, int node, int cutoff,
	vi& cnt, int& uniq
) {
	for (const auto& it : S[node]) {
		if (it.se > cutoff) break;        // times are increasing
		if (cnt[it.fi]++ == 0) ++uniq;    // position enters the union
	}
}

// remove S[node] up to time <= cutoff
static inline void remove_with_cutoff(
	const V<vpi>& S, int node, int cutoff,
	vi& cnt, int& uniq
) {
	for (const auto& it : S[node]) {
		if (it.se > cutoff) break;
		if (cnt[it.fi]-- == 1) --uniq;    // position leaves the union
	}
}

// extract the cycle in permutation p containing 'start'
static inline vi get_cycle(int start, const vi& p, vb& vis) {
	vi cyc;
	int u = start;
	while (!vis[u]) {
		vis[u] = true;
		cyc.pb(u);
		u = p[u];
	}
	return cyc;
}

inline void solve() {
	// input
	int N, K; ll M;
	cin >> N >> K >> M;
	vi A(K+1), B(K+1);
	for (int i = 1; i <= K; ++i) cin >> A[i] >> B[i];

	// Step 1: simulate one block to build S[u] and then p
	V<vpi> S(N+1);
	for (int u = 1; u <= N; ++u) S[u].pb({u, 0}); // starting position counts

	vi fromPos(N+1);
	iota(fromPos.begin(), fromPos.end(), 0);      // fromPos[pos] = pos at t=0

	for (int t = 1; t <= K; ++t) {
		int a = A[t], b = B[t];
		// the cow at a moves to b; the cow at b moves to a (log both at time t)
		S[fromPos[a]].pb({b, t});
		S[fromPos[b]].pb({a, t});
		swap(fromPos[a], fromPos[b]);
	}

	// p[originCow] = final position after one full block
	vi p(N+1);
	for (int pos = 1; pos <= N; ++pos) p[fromPos[pos]] = pos;

	// Step 2/3: decompose cycles of p and process each with a sliding window
	vi cnt(N+1, 0);
	int uniq = 0;
	vb vis(N+1, false);
	vi ans(N+1, 0);

	for (int u = 1; u <= N; ++u) if (!vis[u]) {
		vi cyc = get_cycle(u, p, vis);
		int L = sz(cyc);

		ll Dll = M / K;
		int D = (int)min<ll>(Dll, L);
		int R = (D == L ? 0 : (int)(M % K)); // whole cycle covered → remainder adds no new positions

		// seed window with D full nodes (cutoff = K)
		if (D > 0) {
			for (int j = 0; j < D; ++j) add_with_cutoff(S, cyc[j], K, cnt, uniq);
		}

		// slide start index around the cycle; at each start, add the remainder node temporarily
		for (int i = 0; i < L; ++i) {
			int rem_idx = (i + D) % L; // node providing the partial block

			add_with_cutoff(S, cyc[rem_idx], R, cnt, uniq); // add partial
			ans[cyc[i]] = uniq;                              // record
			remove_with_cutoff(S, cyc[rem_idx], R, cnt, uniq); // remove partial

			if (D > 0) {
				remove_with_cutoff(S, cyc[i], K, cnt, uniq);        // drop left
				add_with_cutoff(S, cyc[rem_idx], K, cnt, uniq);     // add right
			}
		}

		// cleanup: remove seeded D nodes for this cycle
		if (D > 0) {
			for (int j = 0; j < D; ++j) remove_with_cutoff(S, cyc[j], K, cnt, uniq);
		}
	}

	// output answers
	for (int i = 1; i <= N; ++i) cout << ans[i] << '\n';
}

int main() {
	setIO();
	solve(); // single case
}