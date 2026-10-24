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
const int MXN = (int)3e4+5;
const int MXK = 6;
const int MXST = 203;       // Bell(6)
const int BASE = 7;         // digits 0..6 (K <= 6)
const int MXENC = 117649;   // 7^6

/*
 (1) Problem recap in simple terms:
     We have an N x K grid graph with positive edge costs. We want to open a set of gates so all
     cells become connected with minimum total cost, and count how many minimum-cost plans exist
     modulo 1e9+7.

 (2) Things to notice and step-by-step derivation:
     - With all costs positive, any minimum-cost connected plan is a minimum spanning tree (no
       cycles).
     - K <= 6 is tiny, so we can sweep row by row and only remember how the K frontier cells are
       partitioned into connected components (a set partition / DSU state).
     - When adding an edge:
       - If it connects two different components, it merges them (allowed).
       - If it connects within one component, it makes a cycle (not allowed for a tree).
     - When moving down (vertical step), choosing not to connect to the next row would "drop" the
       old vertex; that is only allowed if its component still has another frontier representative.

 (3) Solution outline:
     - Enumerate all set partitions of {0..K-1} as canonical "restricted growth strings" (<= 203).
     - Precompute transitions for each state:
       - Horizontal edge: keep state (skip) or merge two adjacent labels (take).
       - Vertical edge: keep state (take) or replace one label with a fresh label (skip) if the old
         component still appears elsewhere.
     - Run DP over all edges in scan order, tracking (minCost, ways) per state.
     - Answer is ways for the state where all K frontier nodes are in one component.
     - Time: O(N * K * Bell(K)), Memory: O(N * K + Bell(K)).
 */

static int n, k;
static int hori[MXN][MXK];   // only [i][0..k-2] used
static int vert[MXK][MXN];   // only [c][0..n-2] used

static int numStates;
static int states[MXST][MXK];
static int idOf[MXENC];

static int horNext[MXST][MXK]; // only [st][0..k-2] used
static int verNext[MXST][MXK]; // only [st][0..k-1] used

static ll dpCost[MXST], ndpCost[MXST];
static int dpWays[MXST], ndpWays[MXST];

inline int encState(const int v[]) {
	int code = 0, pw = 1;
	for (int i = 0; i < k; i++) {
		code += v[i] * pw;
		pw *= BASE;
	}
	return code;
}

inline void normalize(int v[]) {
	int remap[BASE];
	for (int i = 0; i < BASE; i++) remap[i] = -1;

	int nxt = 0;
	for (int i = 0; i < k; i++) {
		int x = v[i];
		if (remap[x] == -1) remap[x] = nxt++;
		v[i] = remap[x];
	}
}

inline int getIdNorm(int v[]) {
	normalize(v);
	return idOf[encState(v)];
}

void genStates(int pos, int mx, int cur[]) {
	if (pos == k) {
		for (int i = 0; i < k; i++) states[numStates][i] = cur[i];
		for (int i = k; i < MXK; i++) states[numStates][i] = 0;

		int code = encState(cur);
		idOf[code] = numStates++;
		return;
	}
	for (int x = 0; x <= mx + 1; x++) {
		cur[pos] = x;
		genStates(pos + 1, max(mx, x), cur);
	}
}

inline int doConnect(int st, int j) {
	int v[MXK];
	for (int i = 0; i < k; i++) v[i] = states[st][i];

	if (v[j] == v[j + 1]) return -1; // would create a cycle
	int old = v[j], rep = v[j + 1];
	for (int i = 0; i < k; i++) if (v[i] == old) v[i] = rep;

	return getIdNorm(v);
}

inline int doDisappear(int st, int j) {
	int v[MXK];
	for (int i = 0; i < k; i++) v[i] = states[st][i];

	int lab = v[j];
	int cnt = 0;
	for (int i = 0; i < k; i++) cnt += (v[i] == lab);
	if (cnt == 1) return -1; // would close off a component too early

	v[j] = k; // fresh label (not used elsewhere)
	return getIdNorm(v);
}

inline void addState(int idx, ll cost, int ways) {
	if (idx < 0) return;
	if (cost < ndpCost[idx]) {
		ndpCost[idx] = cost;
		ndpWays[idx] = ways;
	} else if (cost == ndpCost[idx]) {
		int x = ndpWays[idx] + ways;
		if (x >= MOD) x -= MOD;
		ndpWays[idx] = x;
	}
}

inline void runHor(int j, int w) {
	for (int i = 0; i < numStates; i++) ndpCost[i] = BIG, ndpWays[i] = 0;

	for (int st = 0; st < numStates; st++) if (dpCost[st] != BIG) {
		addState(st, dpCost[st], dpWays[st]); // skip
		int nxt = horNext[st][j];
		if (nxt != -1) addState(nxt, dpCost[st] + (ll)w, dpWays[st]); // take
	}

	for (int i = 0; i < numStates; i++) dpCost[i] = ndpCost[i], dpWays[i] = ndpWays[i];
}

inline void runVer(int j, int w) {
	for (int i = 0; i < numStates; i++) ndpCost[i] = BIG, ndpWays[i] = 0;

	for (int st = 0; st < numStates; st++) if (dpCost[st] != BIG) {
		int nxt = verNext[st][j];
		if (nxt != -1) addState(nxt, dpCost[st], dpWays[st]); // skip (disappear)
		addState(st, dpCost[st] + (ll)w, dpWays[st]);         // take (keep)
	}

	for (int i = 0; i < numStates; i++) dpCost[i] = ndpCost[i], dpWays[i] = ndpWays[i];
}

inline void solve() {
	cin >> n >> k;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < k - 1; j++) cin >> hori[i][j];
	}
	for (int c = 0; c < k; c++) {
		for (int i = 0; i < n - 1; i++) cin >> vert[c][i];
	}

	for (int i = 0; i < MXENC; i++) idOf[i] = -1;
	numStates = 0;

	int cur[MXK];
	for (int i = 0; i < MXK; i++) cur[i] = 0;
	cur[0] = 0;
	genStates(1, 0, cur);

	for (int st = 0; st < numStates; st++) {
		for (int j = 0; j < k - 1; j++) horNext[st][j] = doConnect(st, j);
		for (int j = 0; j < k; j++) verNext[st][j] = doDisappear(st, j);
	}

	for (int st = 0; st < numStates; st++) dpCost[st] = BIG, dpWays[st] = 0;

	int start[MXK];
	for (int i = 0; i < k; i++) start[i] = i;
	int startId = getIdNorm(start);
	dpCost[startId] = 0;
	dpWays[startId] = 1;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < k - 1; j++) runHor(j, hori[i][j]);
		if (i < n - 1) {
			for (int j = 0; j < k; j++) runVer(j, vert[j][i]);
		}
	}

	int goal[MXK];
	for (int i = 0; i < k; i++) goal[i] = 0;
	int goalId = getIdNorm(goal);

	cout << dpWays[goalId] << "\n";
}

int main() {
	setIO("escape");
	solve(); // single case
}
