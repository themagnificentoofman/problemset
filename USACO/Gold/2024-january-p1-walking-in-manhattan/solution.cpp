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
const int MXL = 20; // 2^20 > 2e5
const int INF = 2000000007;

/*
 (1) Problem recap in simple terms:
     We have N infinite roads in the plane, each either horizontal (y = c) or vertical (x = c).
     Each cow starts at a point (x, y) that lies on at least one road and has walked for d seconds.
     Cows only move north or east at 1 unit per second. If on a single road, they keep moving along
     that road’s direction. If at an intersection of a horizontal and a vertical road, they move
     north if the number of seconds walked so far is even, otherwise east. For each cow, output the
     final (x, y) position after d seconds.

 (2) Things to notice and step-by-step derivation:
     - Once a cow is on a horizontal road (but not at an intersection), it must move east until it
       hits the next vertical road. Similarly, on a vertical road it must move north until the next
       horizontal road.
     - The choice at intersections depends only on the parity of time. Moving between two roads with
       the same coordinate parity (both even or both odd) takes an even distance, so the time parity
       at the next intersection is unchanged. This means we can "compress" consecutive roads of the
       same parity into a single jump to the next road with different parity.
     - After compression, the cow alternates between going to the next opposite-parity horizontal and
       the next opposite-parity vertical. We can binary-lift these parity-change jumps.

 (3) Solution outline:
     - Split roads into sorted lists horiz (all y) and vert (all x), and append an INF sentinel to
       each list.
     - Build next-parity-change pointers for each list, then build binary lifting tables hn/vn.
     - For each query (x, y, d), first walk along the starting single road (if needed) to the next
       intersection, stopping early if d runs out.
     - From the current intersection, greedily take the largest binary-lift jump whose total
       (delta-y + delta-x) fits in remaining d, updating time parity by the distance consumed.
     - Use the remaining d to walk inside the current compressed "block", choosing axis order by
       the current time parity.
     - Overall time complexity: O((n + q) log n); overall memory complexity: O(n log n).
 */

inline void solve() {
	int n, q;
	cin >> n >> q;

	vi horiz, vert;
	horiz.reserve(n + 1);
	vert.reserve(n + 1);

	for (int i = 0; i < n; i++) {
		char dir;
		int c;
		cin >> dir >> c;
		if (dir == 'H') horiz.pb(c);
		else vert.pb(c);
	}

	sor(horiz);
	sor(vert);

	horiz.pb(INF);
	vert.pb(INF);

	int hCnt = sz(horiz) - 1; // sentinel index
	int vCnt = sz(vert) - 1;  // sentinel index

	V<vi> hn(hCnt + 1, vi(MXL)), vn(vCnt + 1, vi(MXL));
	for (int k = 0; k < MXL; k++) {
		hn[hCnt][k] = hCnt;
		vn[vCnt][k] = vCnt;
	}

	for (int i = hCnt - 1; i >= 0; i--) {
		if (((horiz[i] ^ horiz[i + 1]) & 1) != 0) hn[i][0] = i + 1;
		else hn[i][0] = hn[i + 1][0];
		for (int k = 1; k < MXL; k++) hn[i][k] = hn[hn[i][k - 1]][k - 1];
	}
	for (int i = vCnt - 1; i >= 0; i--) {
		if (((vert[i] ^ vert[i + 1]) & 1) != 0) vn[i][0] = i + 1;
		else vn[i][0] = vn[i + 1][0];
		for (int k = 1; k < MXL; k++) vn[i][k] = vn[vn[i][k - 1]][k - 1];
	}

	auto walkCow = [&](int x, int y, int d) -> pi {
		int ih = lwb(horiz, y);
		int iv = lwb(vert, x);

		ll t = 0;

		if (y < horiz[ih]) {
			if ((ll)y + d <= horiz[ih]) return mp(x, y + d);
			int dy = horiz[ih] - y;
			d -= dy;
			t += dy;
			y = horiz[ih];
		}
		if (x < vert[iv]) {
			if ((ll)x + d <= vert[iv]) return mp(x + d, y);
			int dx = vert[iv] - x;
			d -= dx;
			t += dx;
			x = vert[iv];
		}

		for (int k = MXL - 1; k >= 0; k--) {
			int nh = hn[ih][k], nv = vn[iv][k];
			int dy = horiz[nh] - horiz[ih];
			int dx = vert[nv] - vert[iv];
			if (dy > d || dx > d) continue;
			if (dy + dx > d) continue;

			d -= dy + dx;
			t += dy + dx;
			ih = nh;
			iv = nv;
		}

		if (!d) return mp(vert[iv], horiz[ih]);

		if (t & 1) {
			int nv = vn[iv][0];
			int dx = vert[nv] - vert[iv];
			if (dx >= d) return mp(vert[iv] + d, horiz[ih]);
			d -= dx;
			return mp(vert[nv], horiz[ih] + d);
		} else {
			int nh = hn[ih][0];
			int dy = horiz[nh] - horiz[ih];
			if (dy >= d) return mp(vert[iv], horiz[ih] + d);
			d -= dy;
			return mp(vert[iv] + d, horiz[nh]);
		}
	};

	for (int qq = 0; qq < q; qq++) {
		int x, y, d;
		cin >> x >> y >> d;
		auto res = walkCow(x, y, d);
		cout << res.fi << ' ' << res.se << '\n';
	}
}

int main() {
	setIO();
	solve(); // single case
}
