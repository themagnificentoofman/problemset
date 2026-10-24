#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    cin >> n;
    vector<string> grid(n);
    for (int i = 0; i < n; i++) {
        cin >> grid[i];
    }

    int flips = 0;
    for (int i = n - 1; i >= 0; i--) {
        for (int j = n - 1; j >= 0; j--) {
            if (grid[i][j] == '1') {
                flips++;
                for (int r = 0; r <= i; r++) {
                    for (int c = 0; c <= j; c++) {
                        grid[r][c] ^= 1;
                    }
                }
            }
        }
    }

    cout << flips;
}

// O(n^4) (~10000 operations, easily passes)
// Greedy strategy: always flip the sub-rectangle containing
// the bottom-rightmost cow