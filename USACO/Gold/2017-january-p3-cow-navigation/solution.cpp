// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t;
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
const ll BIG = (ll)4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap (simple):
   Two cows start at (N-1,0). One faces up, the other faces right.
   We issue the same commands (L/R/F) to both; walls/hedges block movement.
   Find the minimum number of commands so both end at (0,N-1).

2) Things to notice + step-by-step:
   - A state is fully described by (x1,y1,x2,y2,dir), where dir is cow1's facing.
   - Cow2's facing is always (dir+1)%4 (90° rotated), so we don't store it separately.
   - From any state there are exactly 3 moves (L, R, F), each with cost 1.
   - This is an unweighted shortest path => BFS over the state space.

3) Solution outline:
   - BFS from the start state, storing dist for each 5D state (or -1 if unvisited).
   - Transition by applying L/R/F, with “if at goal, stay put” for that cow.
   - First time we reach both cows at the goal is the minimum.
   - Time: O(N^4).
*/

struct State {
	int x1, y1, x2, y2, d;
};

inline void solve() {
	int n;
	cin >> n;

	static char g[21][21];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) cin >> g[i][j];
	}

	static int dist[21][21][21][21][4];
	for (int a = 0; a < n; a++)
		for (int b = 0; b < n; b++)
			for (int c = 0; c < n; c++)
				for (int d = 0; d < n; d++)
					for (int e = 0; e < 4; e++)
						dist[a][b][c][d][e] = -1;

	const int dx[4] = {-1, 0, 1, 0}; // U R D L
	const int dy[4] = {0, 1, 0, -1};

	auto ok = [&](int x, int y) -> bool {
		return 0 <= x && x < n && 0 <= y && y < n && g[x][y] != 'H';
	};
	auto atGoal = [&](int x, int y) -> bool {
		return x == 0 && y == n - 1;
	};

	queue<State> q;
	State st{n - 1, 0, n - 1, 0, 0}; // cow1 faces up, cow2 faces right
	dist[st.x1][st.y1][st.x2][st.y2][st.d] = 0;
	q.push(st);

	while (!q.empty()) {
		State cur = q.ft; q.pop();
		int cd = dist[cur.x1][cur.y1][cur.x2][cur.y2][cur.d];

		if (atGoal(cur.x1, cur.y1) && atGoal(cur.x2, cur.y2)) {
			cout << cd << "\n";
			return;
		}

		// turn right
		{
			State nxt = cur;
			nxt.d = (cur.d + 1) % 4;
			if (dist[nxt.x1][nxt.y1][nxt.x2][nxt.y2][nxt.d] == -1) {
				dist[nxt.x1][nxt.y1][nxt.x2][nxt.y2][nxt.d] = cd + 1;
				q.push(nxt);
			}
		}

		// turn left
		{
			State nxt = cur;
			nxt.d = (cur.d + 3) % 4;
			if (dist[nxt.x1][nxt.y1][nxt.x2][nxt.y2][nxt.d] == -1) {
				dist[nxt.x1][nxt.y1][nxt.x2][nxt.y2][nxt.d] = cd + 1;
				q.push(nxt);
			}
		}

		// forward
		{
			State nxt = cur;

			// cow1 forward in dir
			if (!atGoal(nxt.x1, nxt.y1)) {
				int nx = nxt.x1 + dx[nxt.d], ny = nxt.y1 + dy[nxt.d];
				if (ok(nx, ny)) nxt.x1 = nx, nxt.y1 = ny;
			}

			// cow2 forward in (dir+1)%4
			int d2 = (nxt.d + 1) % 4;
			if (!atGoal(nxt.x2, nxt.y2)) {
				int nx = nxt.x2 + dx[d2], ny = nxt.y2 + dy[d2];
				if (ok(nx, ny)) nxt.x2 = nx, nxt.y2 = ny;
			}

			if (dist[nxt.x1][nxt.y1][nxt.x2][nxt.y2][nxt.d] == -1) {
				dist[nxt.x1][nxt.y1][nxt.x2][nxt.y2][nxt.d] = cd + 1;
				q.push(nxt);
			}
		}
	}
}

int main() {
	setIO("cownav");
	solve(); // single case
}
