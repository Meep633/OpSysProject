#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include "burst.h"

using namespace std;

int main() {
  vector<Burst> bursts;
  Burst b1(10, true);
  bursts.push_back(b1);
  b1.time = 20;
  cout << b1.time << " " << bursts[0].time << endl;
  return EXIT_SUCCESS;
}