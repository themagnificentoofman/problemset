#include "grader.h"
#include <bits/stdc++.h>

constexpr int BLOCK = 1000;

/*
 notebook layout (all indices are in [0, 5499]):
 - NOTE_BACK / NOTE_TOP are the deque pointers
 - deque items are stored as pairs (carIndex, value): (2*t+2, 2*t+3)
 - NOTE_BLOCK_MINS + b stores the carIndex of the minimum for window start s=b*BLOCK
 - NOTE_BLOCK_ID / NOTE_GLOBAL_MIN / NOTE_OUTPUTS are used during the afternoon pass
*/
constexpr int NOTE_BACK = 0;
constexpr int NOTE_TOP = 1;

constexpr int NOTE_BLOCK_MINS = 4050;
constexpr int NOTE_BLOCK_ID = NOTE_BLOCK_MINS - 1;
constexpr int NOTE_GLOBAL_MIN = NOTE_BLOCK_MINS - 2;
constexpr int NOTE_OUTPUTS = NOTE_BLOCK_MINS - 3;

constexpr int INF = 1000000007;

/*
 (1) Problem recap in simple terms:
     Bessie sees N car IDs twice and must output the minimum ID of every consecutive K cars, in order,
     while only 5500 notebook integers can persist between calls.

 (2) Things to notice and step-by-step derivation:
     - We can't store all N+1-K answers in the notebook, but we can store a small set of "anchors".
     - Split window start positions into blocks of size BLOCK=1000; there are at most 1000 blocks.
     - During the morning pass, for each block start s=b*BLOCK, store the carIndex of the minimum in
       window [s, s+K-1] at NOTE_BLOCK_MINS+b.
     - During the afternoon pass, process one block at a time:
       - Elements far from the block boundary are contained in every window of that block, so one
         global minimum is enough for them.
       - Only O(BLOCK) elements near the shifting boundary need a monotonic deque.

 (3) Solution outline:
     - Morning pass: maintain a monotonic deque in the notebook and record the minimum position for
       each block-start window when its right endpoint is reached.
     - Afternoon pass: start from the stored minimum position of the first block-start window, keep
       a deque for boundary elements plus a global minimum for "always-in-window" elements, and shout
       when we reach the right endpoint of the next window.
     - When we hit the stored minimum position for the next block start, flush remaining outputs for
       the current block, reset state, and continue.
     - Time: O(N) per test case; Memory: O(1) persistent (<= 5500 ints)
 */

void helpBessie(int v) {
	auto qIdx = [](int t) { return 2 * t + 2; };
	auto qVal = [](int t) { return 2 * t + 3; };

	int n = getTrainLength();
	int k = getWindowLength();
	int i = getCurrentCarIndex();
	int pass = getCurrentPassIndex();

	if (pass == 0) {
		// morning pass: store min positions for windows starting at multiples of BLOCK
		if (i == 0) {
			set(NOTE_BACK, 0);
			set(NOTE_TOP, -1);
		}

		int back = get(NOTE_BACK);
		int top = get(NOTE_TOP);

		bool boundary = (i % BLOCK == 0) || (i >= k && ((i - k) % BLOCK == 0));
		if (boundary) {
			while (top >= back && get(qVal(top)) >= v) top--;
			top++;
			set(qIdx(top), i);
			set(qVal(top), v);
		} else {
			int curTopVal = get(qVal(top));
			if (v <= curTopVal) {
				top--;
				while (top >= back && get(qVal(top)) >= v) top--;
				top++;
				set(qIdx(top), i);
				set(qVal(top), v);
			}
		}

		int start = i + 1 - k;
		if (i >= k - 1 && start % BLOCK == 0) {
			while (top >= back && get(qIdx(back)) <= i - k) back++;
			int b = start / BLOCK;
			set(NOTE_BLOCK_MINS + b, get(qIdx(back)));
		}

		set(NOTE_BACK, back);
		set(NOTE_TOP, top);
		return;
	}

	// afternoon pass: output all window minima using the stored block anchors
	int firstMinPos = get(NOTE_BLOCK_MINS);
	if (i < firstMinPos) return;

	if (i == firstMinPos) {
		set(NOTE_BACK, 0);
		set(NOTE_TOP, -1);
		set(NOTE_BLOCK_ID, 1);
		set(NOTE_GLOBAL_MIN, INF);
		set(NOTE_OUTPUTS, 0);
	}

	int bc = get(NOTE_BLOCK_ID);
	int back = get(NOTE_BACK);
	int top = get(NOTE_TOP);
	int outputs = get(NOTE_OUTPUTS);
	int globalMin = get(NOTE_GLOBAL_MIN);

	int prevAnchorPos = get(NOTE_BLOCK_MINS + bc - 1);

	if (i - prevAnchorPos <= BLOCK) {
		while (top >= back && get(qVal(top)) >= v) top--;
		top++;
		set(qIdx(top), i);
		set(qVal(top), v);
	} else {
		if (v < globalMin) {
			globalMin = v;
			set(NOTE_GLOBAL_MIN, globalMin);
		}
	}

	// output when we reach the right endpoint of the next window to report
	if (outputs + k - 1 == i) {
		while (top >= back && get(qIdx(back)) < outputs) back++;

		int ans = globalMin;
		if (top >= back) ans = std::min(ans, get(qVal(back)));

		shoutMinimum(ans);
		outputs++;
	}

	// if we hit the stored min position for the next block start, flush and move to the next block
	for (;;) {
		if (BLOCK * bc + k - 1 >= n) break;
		if (get(NOTE_BLOCK_MINS + bc) != i) break;

		while (outputs <= BLOCK * bc) {
			while (top >= back && get(qIdx(back)) < outputs) back++;

			int ans = globalMin;
			if (top >= back) ans = std::min(ans, get(qVal(back)));

			shoutMinimum(ans);
			outputs++;
		}

		bc++;
		globalMin = INF;
		set(NOTE_GLOBAL_MIN, globalMin);

		top = back = 0;
		set(qIdx(top), i);
		set(qVal(top), v);
	}

	set(NOTE_OUTPUTS, outputs);
	set(NOTE_BLOCK_ID, bc);
	set(NOTE_BACK, back);
	set(NOTE_TOP, top);
}
