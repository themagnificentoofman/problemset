// Source: https://usaco.guide/general/io
// #pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")

#include <bits/stdc++.h>
using namespace std;

// basic type aliases
using str = string;
using ll = long long;
using i128 = __int128_t; // sometimes will use lll, but can easily get mixed up w/ ll (use i128 by default)
using db = long double;  // change to double if TL is tight (almost never)

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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better
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
   Elsie plays N cards (from 1..2N) in a fixed order. Bessie has the other N cards.
   In the first N/2 rounds, higher card wins. In the last N/2 rounds, lower card wins.
   Maximize Bessie's total wins.

2) Things to notice + step-by-step:
   - We can treat the two halves separately by choosing which Bessie cards to reserve:
     * first half wants large cards,
     * second half wants small cards.
   - Let B be Bessie's cards sorted.
     * Use the largest N/2 cards for the first half and greedily match ">".
     * Use the smallest N/2 cards for the second half and greedily match "<".
   - After sorting each half of Elsie's cards, both matchings become simple two-pointer sweeps.

3) Solution outline:
   - Build Bessie's card list from the complement of Elsie's.
   - Split B into low and high halves.
   - Count wins in first half with two pointers on sorted Elsie_first and B_high (want B > E).
   - Count wins in second half with two pointers from the end on sorted Elsie_second and B_low (want B < E).
   - Time: O(N log N).
*/

inline void solve() {
	int n;
	cin >> n;
	int h = n / 2;

	vi e1(h), e2(h);
	vb has(2 * n + 1, false);

	for (int i = 0; i < n; i++) {
		int x; cin >> x;
		has[x] = true;
		if (i < h) e1[i] = x;
		else e2[i - h] = x;
	}

	vi b;
	b.reserve(n);
	for (int x = 1; x <= 2 * n; x++) {
		if (!has[x]) b.pb(x);
	}
	sor(b);

	vi blow(bg(b), bg(b) + h);
	vi bhigh(bg(b) + h, end(b));

	sor(e1);
	sor(e2);

	int ans = 0;

	// first half: want bhigh > e1
	{
		int i = 0, j = 0, win = 0;
		while (i < h && j < h) {
			if (bhigh[j] > e1[i]) {
				win++;
				i++; j++;
			} else {
				j++; // this B card can't beat current Elsie, so it loses somewhere
			}
		}
		ans += win;
	}

	// second half: want blow < e2
	{
		int i = h - 1, j = h - 1, win = 0;
		while (i >= 0 && j >= 0) {
			if (blow[j] < e2[i]) {
				win++;
				i--; j--;
			} else {
				j--; // this B card is too big to win against any remaining Elsie
			}
		}
		ans += win;
	}

	cout << ans << "\n";
}

int main() {
	setIO("cardgame");
	solve(); // single case
}
