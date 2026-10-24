// Source: https://usaco.guide/general/io

#include <bits/stdc++.h>
using namespace std;

void setIO(const string& name) {
	cin.tie(0)->sync_with_stdio(0);
	if (!name.empty()) {
		freopen((name + ".in").c_str(), "r", stdin);
		freopen((name + ".out").c_str(), "w", stdout);
	}

}

int main() {
	setIO("revegetate");
	int numpastures, numcows;
	cin >> numpastures >> numcows;
	
}
