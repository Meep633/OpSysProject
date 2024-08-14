#include "algos.h"

struct rrPriority {
    bool operator()(Process& a, Process& b) const { //assume first burst is a CPU burst
        int aTime = a.burst(0).arrivalTime();
        int bTime = b.burst(0).arrivalTime();
        if (aTime == bTime) {
            return a.name() > b.name();
        }
        return aTime > bTime;
    }
};

void RR(const std::vector<Process> &processes, int numProcesses, int t_cs, int t_slice, std::ofstream &simout) {
    //add numCPUPreemptions and numIOPreemptions to statkeeping
    //whenever cpu is in use, check if time since burst started has exceeded t_slice
    //if so, preempt the process by updating the first burst's arrival time and send back to ready queue
}