#include <vector>
#include <algorithm>
#include "process.h"

using namespace std;

// Compare type for std::priority_queue
struct FCFS {
    bool operator()(const Process& a, const Process& b) const {
        return a.arrivalTime() > b.arrivalTime();
    }
};

// // Sorts the processes in order of First Come First Served
// bool FCFS(const Process &a, const Process &b) {
//     return a.arrivalTime() < b.arrivalTime();
// };
