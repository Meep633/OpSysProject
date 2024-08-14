#ifndef __algos_h_
#define __algos_h_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <climits>
#include <vector>
#include <queue>
#include "process.h"
#include "burst.h"

struct arrivalPriority {
    bool operator()(Process &a, Process &b) const {
        if (a.arrivalTime() == b.arrivalTime()) {
            return a.name() > b.name();
        }
        return a.arrivalTime() > b.arrivalTime();
    }
};

struct ioPriority {
    bool operator()(Process &a, Process &b) const { //assume first burst is an I/O burst
        int aTime = a.burst(0).arrivalTime() + a.burst(0).burstTime(); //arrivalTime + burstTime = time when I/O burst finishes
        int bTime = b.burst(0).arrivalTime() + b.burst(0).burstTime();
        if (aTime == bTime) {
            return a.name() > b.name();
        }
        return aTime > bTime;
    }
};

void FCFS(const std::vector<Process> &processes, int numProcesses, int t_cs, std::ofstream &simout);
void SJF(const std::vector<Process> &processes, int numProcesses, int t_cs, std::ofstream &simout, double alpha);
void SRT();
void RR(const std::vector<Process> &processes, int numProcesses, int t_cs, int t_slice, std::ofstream &simout);

#endif