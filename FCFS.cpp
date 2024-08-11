#include <iostream>
#include <vector>
#include <algorithm>
#include "process.h"

using namespace std;

void FCFS(vector<Process> &processes) {
    
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        return a.arrivalTime() < b.arrivalTime();
    });
}