// Source: https://usaco.guide/general/io

#include <bits/stdc++.h>
using namespace std;
vector<vector<bool>> connected;
vector<bool> visited;
int n;
int dfs(int c){
	visited[c] = true;
	int reached = 1;
	for(int i = 0; i < (int)(connected.size()); i++){
		if(!visited[i] && connected[c][i]){
			visited[i] = true;
			reached += dfs(i);
		}
	}
	return reached;
}
int main() {
	cin >> n;
	vector<int> x(n), y(n), p(n);
	for(int i = 0; i < n; i++){
		cin >> x[i] >> y[i] >> p[i];
	}
	connected = vector<vector<bool>>(n, vector<bool>(n));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int dist_squared =
			    ((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]));
			connected[i][j] = dist_squared <= p[i] * p[i];
			//to see if a cow can reach another cow
		}
	}
	int maxc = 0;
	for (int i = 0; i < n; i++) {
		visited.assign(n, false);
		maxc = max(maxc, dfs(i));
	}
	cout << maxc << "\n";
}
