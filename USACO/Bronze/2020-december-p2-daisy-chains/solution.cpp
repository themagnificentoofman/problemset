// Source: https://usaco.guide/general/io

#include <bits/stdc++.h>
using namespace std;

const int MXP = 1001;

int main() {
	cin.tie(0)->sync_with_stdio(0);
    int n; cin >> n;
    vector<int> petals(n);
    for(int &x : petals) cin >> x;
    int ans = 0;
    for(int i = 0; i < n; ++i) {
        vector<bool> present(MXP);
        int sum = 0;
        for(int j = i; j < n; ++j) {
            sum += petals[j];
            present[petals[j]] = true;
            if(sum % (j - i + 1) == 0 && present[sum / (j - i + 1)]) {
                ans++;
            }
        }
    }
    cout << ans;
}
