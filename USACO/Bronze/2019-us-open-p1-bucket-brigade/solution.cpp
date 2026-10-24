// Source: https://usaco.guide/general/io

#include <bits/stdc++.h>
using namespace std;
int main() {
             int barnx, barny, rockx, rocky, lakex, lakey;
         for(int i = 0; i < 10; i++) {
          string row; cin >> row;
          for(int j = 0; j < 10; j++) {
               if(row[j] == 'B') { barnx = i, barny = j; }
                           if(row[j] == 'R') { rockx = i, rocky = j; }
               if(row[j] == 'L') { lakex = i, lakey = j; }
          }
         }
     // initial distance
     int cows = abs(barnx - lakex) + abs(barny - lakey) - 1;
     if(barnx == lakex && rockx == barnx && ((lakey < rocky && rocky < barny) || (barny < rocky && rocky < lakey))) cows += 2;
  if(barny == lakey && rocky == barny && ((lakex < rockx && rockx < barnx) || (barnx < rockx && rockx < lakex))) cows += 2;
     cout << cows;
}