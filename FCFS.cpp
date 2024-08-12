#include "algos.h"

struct fcfsPriority {
    bool operator()(Process& a, Process& b) const {
        int aTime, bTime = 0; //assume that a and b must have a CPU burst
        for (unsigned int i = 0; i < a.numBursts(); i++) {
            if (a.burst(i).type()) {
                aTime = a.burst(i).arrivalTime();
                break;
            }
        }
        for (unsigned int i = 0; i < b.numBursts(); i++) {
            if (b.burst(i).type()) {
                bTime = b.burst(i).arrivalTime();
                break;
            }
        }
        if (aTime == bTime) {
            return a.name() > b.name();
        }
        return aTime > bTime;
    }
};

void printQueue(std::priority_queue<Process, std::vector<Process>, fcfsPriority> queue) {
    std::cout << "[Q";
    if (queue.empty()) {
        std::cout << " empty";
    }
    while (!queue.empty()) {
        std::cout << " ";
        std::cout << queue.top().name();
        queue.pop();
    }
    std::cout << "]" << std::endl;
}

void FCFS(const std::vector<Process> &processes, int numProcesses, int t_cs, std::ofstream &simout) {
    //processQueue keeps track of processes that haven't arrived for the first time yet 
    //readyQueue keeps track of processes waiting to use the CPU
    //ioQueue keeps track of processes executing an I/O burst
    std::priority_queue<Process, std::vector<Process>, fcfsPriority> processQueue(processes.begin(), processes.end());
    std::priority_queue<Process, std::vector<Process>, fcfsPriority> readyQueue;
    std::priority_queue<Process, std::vector<Process>, fcfsPriority> ioQueue;
    
    //variables to keep track of general things
    int totalTime = 0;
    int terminatedProcesses = 0;
    bool cpuInUse = false;
    Process usingCPU;
    int timeOfSwitchOut = 0;
    int timeOfSwitchIn = 0;

    //stats
    int cpuBurstTimeSum = 0;
    int cpuBoundAvgWaitTime = 0;
    int ioBoundAvgWaitTime = 0;
    int cpuBoundAvgTurnaroundTime = 0;
    int ioBoundAvgTurnaroundTime = 0;
    int numContextSwitches = 0;

    int stopPrintTime = 10000;
#ifdef DEBUG
    stopPrintTime = INT_MAX;
#endif

    //maybe change bursts vector to a list to make removing head more efficient

    std::cout << "time 0ms: Simulator started for FCFS ";
    printQueue(readyQueue);
    while (terminatedProcesses != numProcesses && totalTime - timeOfSwitchOut == t_cs/2) { //sim ends when all processes terminated and last process switched out
        //check if a cpu burst has completed
        if (cpuInUse) {
            Burst burst = usingCPU.burst(0);
            if (burst.arrivalTime() + burst.burstTime() + t_cs/2 == totalTime) { //finished current CPU burst
                if (totalTime < stopPrintTime) {
                    std::cout << "time " << totalTime << "ms: Process " << usingCPU.name() << " completed "
                                "a CPU burst; " << usingCPU.numCPUBursts() << " bursts to go ";
                    printQueue(readyQueue);
                }

                //update stats here
                //add to turnaround time sum and number of context switches
                burst.setTurnaroundTime(totalTime - burst.arrivalTime() + t_cs/2); // + t_cs/2 to account for switch out
                
                cpuInUse = false;
                usingCPU.removeBurst(0);
                timeOfSwitchOut = totalTime;

                if (usingCPU.numCPUBursts() == 0) {
                    std::cout << "Process " << usingCPU.name() << " terminated ";
                    printQueue(readyQueue);
                    terminatedProcesses++;
                } else {
                    if (totalTime < stopPrintTime) {
                        std::cout << "time " << totalTime << "ms: Process " << usingCPU.name() << " switching "
                                    "out of CPU; blocking on I/O until time " << 1 << "ms ";
                        printQueue(readyQueue);
                    }
                    ioQueue.push(usingCPU);
                }
            }
        }

        //check if a process can start using the cpu to execute a cpu burst
        if (!readyQueue.empty() && !cpuInUse) {
            Process p = readyQueue.top();
            if (p.contextSwitching() && totalTime - timeOfSwitchIn == t_cs/2) {
                usingCPU = p;
                cpuInUse = true;
                p.setContextSwitching(false);
                readyQueue.pop();
                if (totalTime < stopPrintTime) {
                    std::cout << "time " << totalTime << "ms: Process " << p.name() << " started using the "
                                "CPU for " << p.burst(0).burstTime() << "ms burst";
                    printQueue(readyQueue);
                }
            } else {
                //update stats here
                Burst burst = p.burst(0);
                burst.setWaitTime(burst.waitTime() + totalTime - burst.arrivalTime());
                p.setContextSwitching(true);
                timeOfSwitchIn = totalTime;
            }
            //check if a process started using a cpu burst
            //make sure to set cpuInUse to true and usingCPU to the top process
            //things to check: cpuInUse, contextSwitchIn, totalTime - timeOfSwitchIn, contextSwitchOut, totalTime - timeOfSwitchOut
            //need to update the burst's wait time here (DO NOT INCLUDE SWITCH IN TIME)
        }

        //update burst's arrival time when it goes to ready queue

        if (readyQueue.empty()) {
            Process p = processQueue.top();
            readyQueue.push(p);
            processQueue.pop();

            totalTime = p.burst(0).arrivalTime();
            std::cout << "time " << totalTime << "ms: Process " << p.name() << " arrived; added to ready queue ";
            printQueue(readyQueue);
            std::cout << std::endl;
        }

        totalTime++;
    }

    //print stats

    //1. pop the first process out of the queue and do simulation
    //2. after process completes burst, remove the CPU burst, print some stuff about I/O burst, remove I/O burst,
    //   and then add process back to queue with new arrival time for next CPU burst
}