// Source: https://usaco.guide/general/io
// Alice's code

#include <bits/stdc++.h>
using namespace std;

bool canSpell(const vector<string> &blocks, const string &word) {
    int len = word.size();
    vector<bool> used(4, false);

    function<bool(int)> backtrack = [&](int idx) {
        if (idx == len) return true;
        for (int i = 0; i < 4; i++) {
            if (!used[i] && blocks[i].find(word[idx]) != string::npos) {
                used[i] = true;
                if (backtrack(idx + 1)) return true;
                used[i] = false;
            }
        }
        return false;
    };

    return backtrack(0);
}

int main() {
    int N;
    cin >> N;

    vector<string> blocks(4);
    for (int i = 0; i < 4; i++) cin >> blocks[i];

    vector<string> words(N);
    for (int i = 0; i < N; i++) cin >> words[i];

    for (auto &word : words) {
        if (canSpell(blocks, word)) cout << "YES\n";
        else cout << "NO\n";
    }

    return 0;
}