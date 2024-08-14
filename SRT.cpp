#include "algos.h"

struct srtPriority { //might not need to change anything in the operator
    bool operator()(Process& a, Process& b) const { //assume first burst is a CPU burst
        if (a.getPredictedRemainingTime() != b.getPredictedRemainingTime()) { // a/b is predicted to finish before b/a
            return a.getPredictedRemainingTime() > b.getPredictedRemainingTime();
        }
        if (a.burst(0).numPreemptions() > 0 || b.burst(0).numPreemptions() > 0) { // a/b was preempted
            return a.burst(0).numPreemptions() == 0;
        }
        if ((a.burst(0).first() && !b.burst(0).first()) || (!a.burst(0).first() && b.burst(0).first())) { // a/b arrived for the first time (not both)
            return a.burst(0).first();
        }
        return a.name() > b.name(); // a/b completed i/o burst or arrived at same time -> alphabetical order
    }
};


void printQueue(std::priority_queue<Process, std::vector<Process>, srtPriority> queue, int timeOfSwitchIn, int totalTime) {
    std::cout << "[Q";
    if (queue.empty()) {
        std::cout << " empty";
    }
    while (!queue.empty()) {
        if (!(queue.top().contextSwitching() && totalTime - timeOfSwitchIn == 1)) { //specific thing to check to match given outputs
            std::cout << " ";
            std::cout << queue.top().name();
        }
        queue.pop();
    }
    std::cout << "]" << std::endl;
}

void SRT(const std::vector<Process> &processes, int numProcesses, int t_cs, double alpha, std::ofstream &simout) {

}