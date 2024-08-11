#include <vector>
#include <algorithm>
#include "process.h"

using namespace std;

void SJF(vector<Process> &processes) {
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.burst(0).time < b.burst(0).time;
    });

}