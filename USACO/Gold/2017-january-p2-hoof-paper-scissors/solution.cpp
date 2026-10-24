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
   Over N games of Hoof/Paper/Scissors, Bessie can change her gesture at most K times.
   Given the opponent’s moves, maximize how many games Bessie wins.

2) Things to notice + step-by-step:
   - Only the current gesture matters; switching is just when today’s gesture != yesterday’s.
   - Let dp[j][g] be the best wins so far if we used j switches and our current gesture is g.
   - For each day, try moving from previous gesture prev to new gesture g:
       switches add (prev != g), and add 1 win if g beats opponent today.

3) Solution outline:
   - Initialize dp[0][g] = 0 for all g (we can start with any gesture for free).
   - For each game, build ndp from dp by trying all prev->g transitions.
   - Answer is max over dp[j][g] for 0 <= j <= K.
   - Time: O(N * K * 9).
*/

inline int toMove(char c) {
	if (c == 'H') return 0;
	if (c == 'P') return 1;
	return 2; // 'S'
}

// H=0, P=1, S=2; winning gesture is (opp+1)%3
inline bool win(int g, int opp) {
	return g == (opp + 1) % 3;
}

inline void solve() {
	int n, k;
	cin >> n >> k;

	vi opp(n);
	for (int i = 0; i < n; i++) {
		char c; cin >> c;
		opp[i] = toMove(c);
	}

	const int NEG = -1e9;
	V<vi> dp(k + 1, vi(3, NEG));
	for (int g = 0; g < 3; g++) dp[0][g] = 0;

	for (int i = 0; i < n; i++) {
		V<vi> ndp(k + 1, vi(3, NEG));
		for (int j = 0; j <= k; j++) {
			for (int prev = 0; prev < 3; prev++) {
				if (dp[j][prev] == NEG) continue;
				for (int g = 0; g < 3; g++) {
					int nj = j + (prev != g);
					if (nj > k) continue;
					ndp[nj][g] = max(ndp[nj][g], dp[j][prev] + win(g, opp[i]));
				}
			}
		}
		dp.swap(ndp);
	}

	int ans = 0;
	for (int j = 0; j <= k; j++) {
		for (int g = 0; g < 3; g++) ans = max(ans, dp[j][g]);
	}
	cout << ans << "\n";
}

int main() {
	setIO("hps");
	solve(); // single case
}
