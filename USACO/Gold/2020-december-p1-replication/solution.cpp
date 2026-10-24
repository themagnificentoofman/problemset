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
 (1) Problem recap (simple):
     - Grid with rocks '#' (impassable) and robots starting at 'S'.
     - Each minute robots move 1 step (4-neighbors). Every d hours they replicate.
     - After t hours, the "cluster" of robots from any moving center has radius floor(t / d) in Manhattan distance.
     - We want the number of cells that can ever be occupied by some robot at some time without going through rocks.

 (2) Things to notice & step-by-step derivation:
     - Rocks constrain where a growing cluster center may travel. If a center stands on cell (i,j) at time t
       with radius r = floor(t / d), it must still have distance_to_rock(i,j) > r, otherwise some robot in the cluster
       would collide with a rock.
     - So we first compute dist_to_rock for every cell by a multi-source BFS from all rocks.
     - Then we run a BFS of "centers" starting from all 'S'. The state carries time (dist of moves).
       At each step, radius = floor(time / d). A cell (i,j) is valid for a center only if dist_to_rock(i,j) > radius.
       This gives all center positions that are safe for some time t.
     - Finally, from every safe center cell we "spread" its radius r across the grid (cannot pass rocks), marking all cells
       within Manhattan distance ≤ r as covered (reachable by the replicated swarm at some time).

 (3) Solution outline:
     - BFS #1 (from '#'): rock_dist[i][j] = minimum steps to reach a rock (or 0 if cell is a rock).
     - BFS #2 (from all 'S' as centers over time): visit a cell the first time we can stand there safely with radius floor(t/d).
       We also guard the moment we cross a multiple of d by checking safety both just before and just after radius growth.
       Record vis[i][j] = the maximum safe radius we achieved while standing on (i,j).
     - BFS #3 (coverage): multi-source from all (i,j) with vis[i][j] ≥ 0, starting with "fuel" = vis[i][j] and spreading
       outward (fuel decreases by 1 each step, stop at rocks). Mark everything we can reach as covered.
     - Answer = count of covered cells.
*/

struct State {
    int i, j, dist; // i,j = cell; dist = time steps (hours) elapsed or remaining (depending on BFS)
};

inline void solve() {
    int n, d;
	cin >> n >> d;

	V<V<char>> grid(n, V<char>(n));
	for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) cin >> grid[i][j];

	/*
	 * BFS #1: multi-source from all rocks to compute rock_dist
	 * rock_dist[i][j] = Manhattan distance to nearest '#'
	 * This lets us quickly check the safety constraint: rock_dist[i][j] > radius.
	 */
	V<vi> rock_dist(n, vi(n, -1));
	queue<State> q;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (grid[i][j] == '#') q.push({i, j, 0});

	while (sz(q)) {
		State s = q.ft;
		q.pop();

		// Skip if position out of bounds.
		if (s.i < 0 || s.i >= n || s.j < 0 || s.j >= n) continue;
		// Skip if already visited.
		if (rock_dist[s.i][s.j] != -1) continue;
		rock_dist[s.i][s.j] = s.dist;

		// Standard 4-neighbor expansion.
		q.push({s.i + 1, s.j, s.dist + 1});
        q.push({s.i - 1, s.j, s.dist + 1});
        q.push({s.i, s.j + 1, s.dist + 1});
        q.push({s.i, s.j - 1, s.dist + 1});
	}

	/*
	 * BFS #2: centers move like a single agent per cluster.
	 * Queue initialized with all 'S' positions (time = 0).
	 * We keep vis[i][j] = the radius r at which we last confirmed it's safe to stand on (i,j).
	 * Safety check: rock_dist[i][j] > r where r = floor(time / d).
	 */
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (grid[i][j] == 'S') q.push({i, j, 0});

	/*
	 * A cell is "visited" if it gets occupied by a center robot.
	 * vis[i][j] stores the safe radius value when we visit (i,j).
	 * -1 means never visited by a center.
	 */
	V<vi> vis(n, vi(n, -1));

	while (sz(q)) {
		State s = q.ft;
		q.pop();

		// Radius just BEFORE this minute finishes (protects crossing multiples of d).
		// When time increases, radius might bump at exact multiples of d. We guard both sides.
		int size = (s.dist - 1) / d;

		// Bounds and "already visited center" checks.
		if (s.i < 0 || s.i >= n || s.j < 0 || s.j >= n) continue;
		if (vis[s.i][s.j] != -1) continue;

		// If even the "previous" radius would collide, we cannot be here when crossing the boundary.
		if (rock_dist[s.i][s.j] <= size) continue;

		vis[s.i][s.j] = size; // record something valid first...

		// Now check with the actual current radius r = floor(time / d).
		size = (s.dist) / d;
		if (rock_dist[s.i][s.j] <= size) continue; // unsafe after the radius potentially grows now
		vis[s.i][s.j] = size; // finally commit with the current radius

		// Expand centers to neighbors for next minute.
        q.push({s.i + 1, s.j, s.dist + 1});
        q.push({s.i - 1, s.j, s.dist + 1});
        q.push({s.i, s.j + 1, s.dist + 1});
        q.push({s.i, s.j - 1, s.dist + 1});
	}

	/*
	 * BFS #3: coverage spread.
	 * From each safe center cell (i,j) with radius vis[i][j], flood outward up to that many steps,
	 * but do not pass through rocks. This marks all cells the replicated swarm can occupy sometime.
	 * We store in cover[i][j] the best remaining radius "fuel" seen so far (so we don't redo worse states).
	 */
	V<vi> cover(n, vi(n, -1));
	while (sz(q)) q.pop();

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (vis[i][j] >= 0) q.push({i, j, vis[i][j]});

	while (sz(q)) {
		State s = q.ft;
		q.pop();

		// Out of bounds or blocked by rock -> cannot occupy / pass.
		if (s.i < 0 || s.i >= n || s.j < 0 || s.j >= n) continue;
		if (grid[s.i][s.j] == '#') continue;

		// If we have already covered this cell with >= remaining radius, skip.
		if (s.dist <= cover[s.i][s.j]) continue;

		cover[s.i][s.j] = s.dist;

		// Keep expanding while we still have remaining radius to spend.
		if (s.dist > 0) {
			q.push({s.i + 1, s.j, s.dist - 1});
			q.push({s.i - 1, s.j, s.dist - 1});
			q.push({s.i, s.j + 1, s.dist - 1});
			q.push({s.i, s.j - 1, s.dist - 1});
		}
	}

	// Count all cells that ended up covered by some swarm at some time.
	int ans = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) ans += (cover[i][j] >= 0);

	cout << ans;
}

int main() {
    setIO();
    solve(); // single case
}