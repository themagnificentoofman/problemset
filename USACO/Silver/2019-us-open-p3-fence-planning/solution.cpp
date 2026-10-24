// Source: https://usaco.guide/general/io

#include <bits/stdc++.h>
using namespace std;
int n, m;
struct Cow {
	int x, y;
	bool visited;
	vector<int> adj;
};
vector<Cow> cows;
vector<int> curr_net;

void dfs(int curr) {
	cows[curr].visited = true;
	curr_net.push_back(curr);
	for (int i : cows[curr].adj) {
		if (!cows[i].visited) { dfs(i); }
	}
}

int main() {
	cin >> n >> m;
	cows.resize(n);
	for (Cow &c : cows) {
		cin >> c.x >> c.y;
		c.visited = false;
	}
	for (int i = 0; i < m; i++) {
		int a, b;
		cin >> a >> b;
		cows[a - 1].adj.push_back(b - 1);
		cows[b - 1].adj.push_back(a - 1);
	}
	vector<vector<int>> networks;
	for (int i = 0; i < n; i++) {
		if (!cows[i].visited) {
			curr_net.clear();
			dfs(i);
			networks.push_back(curr_net);
		}
	}
	int min_perimeter = INT32_MAX;
	for (vector<int> net : networks) {
		int min_x = INT32_MAX;
		int max_x = 0;
		int min_y = INT32_MAX;
		int max_y = 0;
		for (int i : net) {
			min_x = min(min_x, cows[i].x);
			max_x = max(max_x, cows[i].x);
			min_y = min(min_y, cows[i].y);
			max_y = max(max_y, cows[i].y);
		}
		min_perimeter = min(min_perimeter, 2 * (max_x - min_x) + 2 * (max_y - min_y));
	}

	cout << min_perimeter << endl;
}
