#include <vector>
#include <algorithm>
#include "process.h"

using namespace std;

// Sorts the processes in order of Shortest Job First
void SJF(vector<Process> &processes) {
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
        int a_cpu = 0;
        int b_cpu = 0;
        for (int i = 0; i < a.numBursts(); i++) {
            if (a.burst(i).type) {
                a_cpu += a.burst(i).time;
                break;
            }
        }
        for (int i = 0; i < b.numBursts(); i++) {
            if (b.burst(i).type) {
                b_cpu += b.burst(i).time;
                break;
            }
        }
        return a_cpu < b_cpu;
    });

}