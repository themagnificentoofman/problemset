#include <bits/stdc++.h>
using namespace std;
int main() {
int x1a, y1a, x2a, y2a;
int x1b, y1b, x2b, y2b;
int x1t, y1t, x2t, y2t;
cin >> x1a >> y1a >> x2a >> y2a;
cin >> x1b >> y1b >> x2b >> y2b;
cin >> x1t >> y1t >> x2t >> y2t;
int areaA = (x2a - x1a) * (y2a - y1a);
int areaB = (x2b - x1b) * (y2b - y1b);
int oax1 = max(x1a, x1t);
int oax2 = min(x2a, x2t);
int oay1 = max(y1a, y1t);
int oay2 = min(y2a, y2t);
int aoverlapArea = 0;
if (oax2 > oax1 && oay2 > oay1) {
aoverlapArea = (oax2 - oax1) * (oay2 - oay1);
}
int obx1 = max(x1b, x1t);
int obx2 = min(x2b, x2t);
int oby1 = max(y1b, y1t);
int oby2 = min(y2b, y2t);
int boverlapArea = 0;
if (obx2 > obx1 && oby2 > oby1) {
boverlapArea = (obx2 - obx1) * (oby2 - oby1);
}
int ans = areaA + areaB - aoverlapArea - boverlapArea;
cout << ans;
return 0;
}