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
template<class T, size_t SZ> using AR = array<T,SZ>; // c-style array but better, basically
template<class T> int lwb(V<T>& a, const T& b) { return int(lb(all(a),b)-bg(a)); } // 1st idx with a[idx] >= b
template<class T> int upb(V<T>& a, const T& b) { return int(ub(all(a),b)-bg(a)); } // 1st idx with a[idx] > b
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
const ll BIG = 4e18;
const int MOD = (int)1e9+7;
const int MXN = (int)2e5+5;

/*
1) Problem recap in simple terms:
   We have n votes (each vote is a candidate ID 1..n). Each query updates one vote.
   After each update, we look at the frequency table (counts per candidate) and compute
   the best possible |id_hi - id_lo| subject to: count(id_hi) + count(id_lo) >= max_count.

2) Things to notice and step-by-step derivation:
   - Let max_count be the current maximum frequency (the “mode” frequency).
   - For each frequency f that exists (>0), we care about:
       minId(f) = smallest candidate ID with count f
       maxId(f) = largest  candidate ID with count f
   - If we pick a low-frequency bucket fL and need fR so that fL + fR >= max_count,
     we want the smallest ID at fL and the largest ID among all buckets with freq >= fR.
   - #distinct positive frequencies is O(sqrt(n)) because 1+2+...+m <= n.

3) Solution outline (with time complexity):
   - Maintain cnt[id], plus freqs[f] = set of IDs with count f, and freq_mp[f] = how many IDs at f.
   - On each query, move two IDs between buckets in O(log n).
   - Build the sorted list of distinct positive frequencies; compute minId/maxId per frequency,
     suffix-max the maxId; then run a two-pointer scan to maximize the answer.
   - Time: O((n+q) log n + q * m), where m = #distinct positive frequencies (m = O(sqrt(n))).
*/

inline void solve() {
	int n, q;
	cin >> n >> q;

	vi a(n);
	for (int i = 0; i < n; i++) cin >> a[i];

	vi cnt(n + 1, 0);
	for (int v : a) cnt[v]++;

	// freq_mp[f] = how many candidates currently have frequency f (kept ordered by f)
	map<int,int> freq_mp;

	// freqs[f] = set of candidate IDs with frequency f
	V<set<int>> freqs(n + 1);

	for (int id = 1; id <= n; id++) {
		freq_mp[cnt[id]]++;
		freqs[cnt[id]].ins(id);
	}

	auto adjust = [&](int id, int delta) -> void {
		// remove from old bucket
		int oldf = cnt[id];
		freq_mp[oldf]--;
		if (freq_mp[oldf] == 0) freq_mp.erase(oldf);
		freqs[oldf].erase(id);

		// add to new bucket
		cnt[id] += delta;
		int newf = cnt[id];
		freq_mp[newf]++;
		freqs[newf].ins(id);
	};

	while (q--) {
		int idx, x;
		cin >> idx >> x;
		--idx;

		adjust(a[idx], -1);
		a[idx] = x;
		adjust(a[idx], +1);

		vi fs;
		fs.reserve(sz(freq_mp));
		for (auto &kv : freq_mp) {
			if (kv.fi > 0) fs.pb(kv.fi);
		}

		int m = sz(fs);
		int mode = fs.bk;

		vi mn(m), mx(m);
		for (int i = 0; i < m; i++) {
			int f = fs[i];
			mn[i] = *freqs[f].begin();
			mx[i] = *freqs[f].rbegin();
		}
		for (int i = m - 2; i >= 0; i--) {
			mx[i] = max(mx[i], mx[i + 1]);
		}

		int ans = 0;
		for (int l = 0, r = m - 1; l < m; l++) {
			while (r - 1 >= 0 && fs[l] + fs[r - 1] >= mode) r--;
			ans = max(ans, abs(mx[r] - mn[l]));
		}

		cout << ans << "\n";
	}
}

int main() {
	setIO();
	solve(); // single case
}
