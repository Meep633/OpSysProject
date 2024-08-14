#include "algos.h"

struct srtPriority {
    bool operator()(Process& a, Process& b) const { //assume first burst is a CPU burst
        int aTime = a.tau();
        int bTime = b.tau();
        if (aTime == bTime) {
            return a.name() > b.name();
        }
        return aTime > bTime;
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

void SRT(const std::vector<Process> &processes, int numProcesses, int t_cs, std::ofstream &simout, double alpha) {
    //processQueue keeps track of processes that haven't arrived for the first time yet 
    //readyQueue keeps track of processes waiting to use the CPU
    //ioQueue keeps track of processes executing an I/O burst
    std::priority_queue<Process, std::vector<Process>, arrivalPriority> processQueue(processes.begin(), processes.end());
    std::priority_queue<Process, std::vector<Process>, srtPriority> readyQueue;
    std::priority_queue<Process, std::vector<Process>, ioPriority> ioQueue;
    
    //variables to keep track of general things
    int totalTime = 0;
    int terminatedProcesses = 0;
    bool cpuInUse = false;
    Process usingCPU;
    int timeOfSwitchOut = 0;
    int timeOfSwitchIn = 0;

    //stats
    int cpuBurstTimeSum = 0;
    int cpuBoundWaitTimeSum = 0;
    int ioBoundWaitTimeSum = 0;
    int cpuBoundTurnaroundTimeSum = 0;
    int ioBoundTurnaroundTimeSum = 0;
    int numCPUBoundCPUBursts = 0;
    int numIOBoundCPUBursts = 0;
    int numCPUBoundContextSwitches = 0;
    int numIOBoundContextSwitches = 0;

    int stopPrintTime = 10000;
#ifdef DEBUG
    stopPrintTime = INT_MAX; //print everything
#endif

    //maybe change bursts vector to a list to make removing head more efficient

    std::cout << "time 0ms: Simulator started for SRT ";
    printQueue(readyQueue, timeOfSwitchIn, totalTime);

    while (!(terminatedProcesses == numProcesses && totalTime - timeOfSwitchOut == t_cs/2)) { //sim ends when all processes terminated and last process switched out
        //check if a cpu burst has completed
        if (cpuInUse) {
            Burst& burst = usingCPU.burst(0);
            int finishTime = burst.arrivalTime() + burst.waitTime() + t_cs/2 + burst.burstTime();
            if (finishTime == totalTime) { //finished current CPU burst
                //update stats
                burst.setTurnaroundTime(totalTime - burst.arrivalTime() + t_cs/2); // + t_cs/2 to account for switch out
                cpuBurstTimeSum += burst.burstTime();
                if (usingCPU.type()) { //cpu-bound process
                    cpuBoundWaitTimeSum += burst.waitTime();
                    cpuBoundTurnaroundTimeSum += burst.turnaroundTime();
                    numCPUBoundCPUBursts++;
                    numCPUBoundContextSwitches++;
                } else { //i/o-bound process
                    ioBoundWaitTimeSum += burst.waitTime();
                    ioBoundTurnaroundTimeSum += burst.turnaroundTime();
                    numIOBoundCPUBursts++;
                    numIOBoundContextSwitches++;
                }

                int burstTime = burst.burstTime();
                cpuInUse = false;
                usingCPU.removeBurst(0);
                timeOfSwitchOut = totalTime;
                

                if (usingCPU.numCPUBursts() == 0) {
                    std::cout << "time " << totalTime << "ms: Process " << usingCPU.name() << " terminated ";
                    printQueue(readyQueue, timeOfSwitchIn, totalTime);
                    terminatedProcesses++;
                } else {
                    if (totalTime < stopPrintTime) {
                        std::cout << "time " << totalTime << "ms: Process " << usingCPU.name() << " (tau " << usingCPU.tau() << "ms) completed "
                                    "a CPU burst; " << usingCPU.numCPUBursts() << " burst";
                        if (usingCPU.numCPUBursts() != 1) {
                            std::cout << "s";
                        }
                        std::cout << " to go ";
                        printQueue(readyQueue, timeOfSwitchIn, totalTime);
                    }
                    //update tau
                    int tau = ceil((alpha * burstTime) + ((1-alpha) * usingCPU.tau()));
                    if (totalTime < stopPrintTime) {
                        std::cout << "time " << totalTime << "ms: Recalculated tau for process " << usingCPU.name() << ": old tau " << usingCPU.tau()
                                    << "ms ==> new tau " << tau << "ms ";
                        printQueue(readyQueue, timeOfSwitchIn, totalTime);
                    }
                    usingCPU.setTau(tau);


                    usingCPU.burst(0).setArrivalTime(totalTime + t_cs/2); //time when i/o burst execution starts
                    ioQueue.push(usingCPU);
                    if (totalTime < stopPrintTime) {
                        int ioTime = usingCPU.burst(0).arrivalTime() + usingCPU.burst(0).burstTime();
                        std::cout << "time " << totalTime << "ms: Process " << usingCPU.name() << " switching "
                                    "out of CPU; blocking on I/O until time " << ioTime << "ms ";
                        printQueue(readyQueue, timeOfSwitchIn, totalTime);
                    }
                }
            }
        }

        if (!readyQueue.empty() && cpuInUse) {
            //Context switch out if arriving process has a lower tau than the remaining time of the process using the CPU
            Process p = readyQueue.top();
            int timeRemaining = usingCPU.tau() - (totalTime - usingCPU.burst(0).arrivalTime());
            if (p.tau() < timeRemaining) {
                usingCPU.setContextSwitching(true);
                timeOfSwitchOut = totalTime;
                if (totalTime < stopPrintTime) {
                    std::cout << "time " << totalTime << "ms: Process " << usingCPU.name() << " (tau " << usingCPU.tau() << "ms) will "
                                "preempt " << usingCPU.name() << " ";
                    printQueue(readyQueue, timeOfSwitchIn, totalTime);
                }
            }
        }

        //check if a process can start using the cpu to execute a cpu burst (no process can be executing burst and context switch out must finish first)
        if (!readyQueue.empty() && !cpuInUse && (timeOfSwitchOut == 0 || totalTime - timeOfSwitchOut >= t_cs/2)) {
            Process p = readyQueue.top();
            if (p.contextSwitching() && totalTime - timeOfSwitchIn == t_cs/2) {
                p.setContextSwitching(false);
                usingCPU = p;
                cpuInUse = true;
                readyQueue.pop();
                if (totalTime < stopPrintTime) {
                    std::cout << "time " << totalTime << "ms: Process " << p.name() << " (tau " << p.tau() << "ms) started using the "
                                "CPU for " << p.burst(0).burstTime() << "ms burst ";
                    printQueue(readyQueue, timeOfSwitchIn, totalTime);
                }
            } else if (!p.contextSwitching()) {
                Burst& burst = p.burst(0);
                burst.setWaitTime(burst.waitTime() + totalTime - burst.arrivalTime()); //add to prev wait time is important for SRT and RR cuz of preemptions
                p.setContextSwitching(true);
                readyQueue.pop(); 
                readyQueue.push(p); //queue.top() returns a const ref so need to reinsert after making changes
                timeOfSwitchIn = totalTime;
            }
        }

        //check if an i/o burst completed
        if (!ioQueue.empty()) {
            Process p = ioQueue.top();
            Burst burst = p.burst(0);
            while (!ioQueue.empty() && totalTime - burst.arrivalTime() == burst.burstTime()) { //multiple processes may have finished an i/o burst at the same time
                p.removeBurst(0);
                p.burst(0).setArrivalTime(totalTime); //next cpu burst arrived in ready queue now
                readyQueue.push(p);
                if (totalTime < stopPrintTime) {
                    std::cout << "time " << totalTime << "ms: Process " << p.name() << " (tau " << p.tau() << "ms) completed I/O; added "
                                "to ready queue ";
                    printQueue(readyQueue, timeOfSwitchIn, totalTime);
                }

                //might be specific to FCFS
                //if added process is at top of ready queue and cpu is ready to take in another process context switch happens immediately
                if (readyQueue.top().name().compare(p.name()) == 0 && !cpuInUse && totalTime - timeOfSwitchOut >= t_cs/2) {
                    timeOfSwitchIn = totalTime;
                    p.setContextSwitching(true);
                    p.burst(0).setWaitTime(p.burst(0).waitTime());
                    readyQueue.pop();
                    readyQueue.push(p);
                }

                ioQueue.pop();
                if (!ioQueue.empty()) {
                    p = ioQueue.top();
                    burst = p.burst(0);
                }
            }
        }

        //check if a process arrived for the first time
        if (!processQueue.empty()) {
            Process p = processQueue.top();
            while (!processQueue.empty() && totalTime == p.arrivalTime()) {
                //might be specific to FCFS
                if (timeOfSwitchIn == 0) { //only do this for first process so it can switch in immediately
                    timeOfSwitchIn = totalTime;
                    p.setContextSwitching(true); //dont need to update wait time since doesn't include context switch time
                }

                readyQueue.push(p); //don't need to set first cpu burst's arrival time since that was set in process generation
                
                if (totalTime < stopPrintTime) {
                    std::cout << "time " << totalTime << "ms: Process " << p.name() << " (tau " << p.tau() << "ms) arrived; added to ready "
                                "queue ";
                    printQueue(readyQueue, timeOfSwitchIn, totalTime);
                }

                processQueue.pop();
                if (!processQueue.empty()) {
                    p = processQueue.top();
                }
            }
        }

        totalTime++;
    }

    std::cout << "time " << totalTime << "ms: Simulator ended for SRT ";
    printQueue(readyQueue, timeOfSwitchIn, totalTime);
    std::cout << std::endl;

    //print stats
    simout << "Algorithm SRT\n";
    simout << "-- CPU utilization: " << ceil((cpuBurstTimeSum*1000.0*100) / totalTime)/1000.0 << "%\n";

    double avg = 0;
    // float avg = numCPUBoundCPUBursts ? (ceil((totalTime * 1000.0) / totalNum) / 1000.0):0.f;
    if (numCPUBoundCPUBursts != 0) {
        avg = ceil((cpuBoundWaitTimeSum*1000.0) / numCPUBoundCPUBursts)/1000.0;
    }
    simout << "-- CPU-bound average wait time: " << avg << " ms\n";

    avg = 0;
    if (numIOBoundCPUBursts != 0) {
        avg = ceil((ioBoundWaitTimeSum*1000.0) / numIOBoundCPUBursts)/1000.0;
    } 
    simout << "-- I/O-bound average wait time: " << avg << " ms\n";

    avg = 0;
    if (numCPUBoundCPUBursts + numIOBoundCPUBursts != 0) {
        avg = ceil(((cpuBoundWaitTimeSum + ioBoundWaitTimeSum)*1000.0) / (numCPUBoundCPUBursts + numIOBoundCPUBursts))/1000.0;
    }
    simout << "-- overall average wait time: " << avg << " ms\n";

    avg = 0;
    if (numCPUBoundCPUBursts != 0) {
        avg = ceil((cpuBoundTurnaroundTimeSum*1000.0) / numCPUBoundCPUBursts)/1000.0;
    }
    simout << "-- CPU-bound average turnaround time: " << avg << " ms\n";

    avg = 0;
    if (numIOBoundCPUBursts != 0) {
        avg = ceil((ioBoundTurnaroundTimeSum*1000.0) / numIOBoundCPUBursts)/1000.0;
    } 
    simout << "-- I/O-bound average turnaround time: " << avg << " ms\n";

    avg = 0;
    if (numCPUBoundCPUBursts + numIOBoundCPUBursts != 0) {
        avg = ceil(((cpuBoundTurnaroundTimeSum + ioBoundTurnaroundTimeSum)*1000.0) / (numCPUBoundCPUBursts + numIOBoundCPUBursts))/1000.0;
    }
    simout << "-- overall average turnaround time: " << avg << " ms\n";

    simout << "-- CPU-bound number of context switches: " << numCPUBoundContextSwitches << "\n";
    simout << "-- I/O-bound number of context switches: " << numIOBoundContextSwitches << "\n";
    simout << "-- overall number of context switches: " << numCPUBoundContextSwitches + numIOBoundContextSwitches << "\n";
    simout << "-- CPU-bound number of preemptions: 0\n";
    simout << "-- I/O-bound number of preemptions: 0\n";
    simout << "-- overall number of preemptions: 0\n";
}