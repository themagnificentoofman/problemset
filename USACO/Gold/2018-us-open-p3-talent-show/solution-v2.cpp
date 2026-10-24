// Source: https://usaco.guide/general/io

#include <bits/stdc++.h>
#define pb push_back
#define mp make_pair
#define fi first
#define se second
#define all(v) v.begin(), v.end()
#define fastio cin.tie(0)->sync_with_stdio(0)
using namespace std;
using ll = long long;
using vi = vector<ll>;
using vvi = vector<vi>;
using ii = pair<ll,ll>;
using vii = vector<ii>;
void setIO(string s) {
  freopen((s + ".in").c_str(), "r", stdin);
  freopen((s + ".out").c_str(), "w", stdout);
}
int dp[2][5000001];
int main() {
    fastio;
    //setIO("talent");
    ll t, a, b;
    cin >> t >> a >> b;
    dp[0][0] = true;
	for (int i = 0; i <= t; i++) {
		if (i - a >= 0) dp[0][i] |= dp[0][i - a];
		if (i - b >= 0) dp[0][i] |= dp[0][i - b];
		dp[1][i / 2] |= dp[0][i];
	}
	for (int i = 0; i <= t; i++) {
		if (i - a >= 0) dp[1][i] |= dp[1][i - a];
		if (i - b >= 0) dp[1][i] |= dp[1][i - b];
	}
	for (int i = t; i >= 0; i--) {
		if (dp[1][i]) {
			cout << i << endl;
			break;
		}
	}
}