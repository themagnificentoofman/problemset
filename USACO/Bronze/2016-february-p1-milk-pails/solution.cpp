// Source: https://usaco.guide/general/io

// William's solution (O(M/X))

#include <iostream>
using namespace std;

int main() {
    int X, Y, M;
    cin >> X >> Y >> M;
    int maxMilk = 0;

    for (int i = 0; i * X <= M; i++) {
        int rem = M - i * X;
        int j = rem / Y;
        int total = i * X + j * Y;
        maxMilk = max(maxMilk, total);
    }

    cout << maxMilk << '\n';
    return 0;
}