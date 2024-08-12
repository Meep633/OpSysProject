#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include "burst.h"

using namespace std;

struct priority {
    bool operator()(Burst& a, Burst& b) const { //assume first burst is a CPU burst
        return true;
    }
};

int main() {
  Burst b(10, true);

  priority_queue<Burst, vector<Burst>, priority> queue;
  queue.push(b);

  Burst bu = queue.top();

  b.setArrivalTime(11);
  std::cout << b.arrivalTime() << " " << bu.arrivalTime() << std::endl;
  
  return EXIT_SUCCESS;
}