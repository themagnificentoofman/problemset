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
const int MXN = (int)5e4+5; // modify as needed
const int MXK = 51;

inline void solve() {
    int n, k;
    cin >> n >> k;
    bool adj[MXK][MXK] = {}; // adj[i][j] -> 1 if breed i can transmit to breed j, 0 if not.
    int cows[MXN]; // breeds of cows 1...n
    for (int i = 1; i <= n; i++) cin >> cows[i];
    for (int i = 1; i <= k; i++) {
        for (int j = 1; j <= k; j++) {
            char c; cin >> c;
            adj[i][j] = (c == '1'); // breed i can trasmit to breed j: edge i -> j exists
        }
        // remember that the cows can transmit to cow N
        adj[i][0] = adj[i][cows[n]];
    }
    // set cow N to a fake breed 0
    cows[n] = 0;
    vi dist[MXK];
    for (int i = 0; i <= k; i++) {
        dist[i].rsz(n + 1);
        for (int j = 0; j <= n; j++) dist[i][j] = -1;
    }
    deque<pi> q; // sender info: first -> breed, second -> location
    q.pb({cows[1], 1});
    dist[cows[1]][1] = 0;
    while (sz(q)) {
        auto [breed, loc] = q.ft;
        q.ppf();
        // move left one cow
        if (loc - 1 >= 1 && dist[breed][loc - 1] == -1) {
			dist[breed][loc - 1] = dist[breed][loc] + 1;
			q.pb({breed, loc - 1});
		}
		// move right one cow
		if (loc + 1 <= n && dist[breed][loc + 1] == -1) {
			dist[breed][loc + 1] = dist[breed][loc] + 1;
			q.pb({breed, loc + 1});
		}
		// transmit to the cow at the current location
		if (adj[breed][cows[loc]] && dist[cows[loc]][loc] == -1) {
			dist[cows[loc]][loc] = dist[breed][loc];
			q.pf({cows[loc], loc});
		}
    }
    cout << dist[0][n];
}

int main() {
    setIO();
    solve(); // single case
}