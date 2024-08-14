#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include "process.h"
#include "random.h"
#include "algos.h"

void p1(const std::vector<Process> &processes, int numProcesses, int numCPUProcesses, std::ofstream &simout);

int main(int argc, char **argv) {
    //Parse arguments
    if (argc != 9) {
        std::cerr << "ERROR: Invalid number of arguments" << std::endl;
        return EXIT_FAILURE;
    }

    int numProcesses;
    int numCPUProcesses;
    long seed;
    double lambda;
    int upperBound;
    int t_cs;
    double alpha;
    int t_slice;

    try {
        numProcesses = std::stoi(*(argv+1), NULL);
        if (numProcesses < 1 || numProcesses > 260) {
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given number of processes (" << *(argv+1) << ") is not a valid int in " 
                        "the range 1 <= n <= 260" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        numCPUProcesses = std::stoi(*(argv+2), NULL);
        if (numCPUProcesses < 0 || numCPUProcesses > numProcesses) {
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given number of CPU-bound processes (" << *(argv+2) << ") is not a valid int in " 
                        "the range 0 <= n_cpu <= " << numProcesses << std::endl;
        return EXIT_FAILURE;
    }
    try {
        seed = std::stol(*(argv+3), NULL);
    } catch (...) {
        std::cerr << "ERROR: Given seed (" << *(argv+3) << ") is not a valid long int" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        lambda = std::stod(*(argv+4), NULL);
        if (lambda <= 0) {
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given lambda (" << *(argv+4) << ") is not a valid double > 0" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        upperBound = std::stoi(*(argv+5), NULL);
        if (upperBound < 8) { //if upper bound < 8 then I/O burst times for I/O-bound processes can't be under ceiling
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given upper bound (" << *(argv+5) << ") is not a valid int > 8" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        t_cs = std::stoi(*(argv+6), NULL);
        if (t_cs <= 0 || t_cs % 2 != 0) {
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given context switch time (" << *(argv+6) << ") is not a valid even positive integer" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        alpha = std::stod(*(argv+7), NULL);
        if (alpha < 0 || alpha > 1) {
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given alpha (" << *(argv+7) << ") is not a valid double in the range [0,1]" << std::endl;
        return EXIT_FAILURE;
    }
    try {
        t_slice = std::stoi(*(argv+8), NULL);
        if (t_slice <= 0) {
            throw 0;
        }
    } catch (...) {
        std::cerr << "ERROR: Given time slice (" << *(argv+8) << ") is not a valid positive integer" << std::endl;
        return EXIT_FAILURE;
    }

    srand48(seed);

    //Generate processes
    // std::priority_queue<Process, vector<Process>, FCFS> processes;
    std::vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) { 
        std::string name(1, i/10 + 'A');
        name += std::to_string(i%10);
        
        int arrivalTime = floor(nextExp(lambda));
        while (arrivalTime > upperBound) {
            arrivalTime = floor(nextExp(lambda));
        }
        
        bool type = true;
        if (i >= numCPUProcesses) {
            type = false;
        }

        Process newProcess = Process(name, arrivalTime, type, lambda);
        
        int numCPUBursts = ceil(drand48() * 32);
        for (int j = 1; j < numCPUBursts; j++) {
            int cpuTime = ceil(nextExp(lambda));
            while (cpuTime > upperBound) {
                cpuTime = ceil(nextExp(lambda));
            }
            int ioTime = ceil(nextExp(lambda));
            while (ioTime > upperBound) {
                ioTime = ceil(nextExp(lambda));
            }

            if (i < numCPUProcesses) {
                cpuTime *= 4;
            } else {
                ioTime *= 8;
            }

            newProcess.addBurst(cpuTime, true);
            newProcess.addBurst(ioTime, false);
        }
        int cpuTime = ceil(nextExp(lambda));
        while (cpuTime > upperBound) {
            cpuTime = ceil(nextExp(lambda));
        }
        if (i < numCPUProcesses) {
            cpuTime *= 4;
        }
        newProcess.addBurst(cpuTime, true);
        newProcess.burst(0).setInitialArrivalTime(arrivalTime); //initialize initial arrival time for burst

        processes.push_back(newProcess);
    }

    std::ofstream simout("simout.txt");

    /*
    std::cout << "<<< PROJECT PART I" << std::endl;
    std::cout << "<<< -- process set (n=" << numProcesses << ") with " << numCPUProcesses << " CPU-bound process";
    if (numCPUProcesses != 1) {
        std::cout << "es";
    }
    std::cout << std::endl;
    std::cout << "<<< -- seed=" << seed << "; lambda=";
    std::cout << std::fixed << lambda;
    std::cout << std::defaultfloat << "; bound=" << upperBound << std::endl;
    p1(processes, numProcesses, numCPUProcesses, simout);
    
    simout << "\n";
    */
    
    std::cout << "\n<<< PROJECT PART II" << std::endl;
    std::cout << "<<< -- t_cs=" << t_cs << "ms; alpha=";
    std::cout << std::fixed << std::setprecision(2) << alpha;
    std::cout << "; t_slice=" << t_slice << "ms" << std::endl;
    // FCFS(processes, numProcesses, t_cs, simout);
    // SJF(processes, numProcesses, t_cs, alpha, simout);
    // SRT(processes, numProcesses, t_cs, alpha, simout);
    RR(processes, numProcesses, t_cs, t_slice, simout);

    simout.close();

    return EXIT_SUCCESS;
}

void p1(const std::vector<Process> &processes, int numProcesses, int numCPUProcesses, std::ofstream &simout) {
    //Part 1 output
    double cpuBoundCPUBurstSum = 0;
    int cpuBoundTotalCPUBursts = 0;
    double ioBoundCPUBurstSum = 0;
    int ioBoundTotalCPUBursts = 0;
    double cpuBoundIOBurstSum = 0;
    int cpuBoundTotalIOBursts = 0;
    double ioBoundIOBurstSum = 0;
    int ioBoundTotalIOBursts = 0;

    for (unsigned int i = 0; i < processes.size(); i++) {
        Process p = processes[i];

        if (p.type()) {
            std::cout << "CPU-bound process ";
        } else {
            std::cout << "I/O-bound process ";
        }
        std::cout << p.name() << ": arrival time " << p.arrivalTime() << "ms; " << p.numCPUBursts() << " CPU burst";
        if (p.numBursts()/2 + 1 > 1) {
            std::cout << "s";
        }
        std::cout << ":" << std::endl;
        for (unsigned int j = 0; j < p.numBursts(); j++) {
            std::cout << "==> ";
            if (p.burst(j).type()) {
                std::cout << "CPU burst " << p.burst(j).burstTime() << "ms";
                if (j != p.numBursts()-1) {
                    std::cout << " ";
                }
                if ((int)i < numCPUProcesses) {
                    cpuBoundCPUBurstSum += p.burst(j).burstTime();
                    cpuBoundTotalCPUBursts++;
                } else {
                    ioBoundCPUBurstSum += p.burst(j).burstTime();
                    ioBoundTotalCPUBursts++;
                }
            } else {
                std::cout << "I/O burst " << p.burst(j).burstTime() << "ms" << std::endl;
                if ((int)i < numCPUProcesses) {
                    cpuBoundIOBurstSum += p.burst(j).burstTime();
                    cpuBoundTotalIOBursts++;
                } else {
                    ioBoundIOBurstSum += p.burst(j).burstTime();
                    ioBoundTotalIOBursts++;
                }
            }
        }
        std::cout << std::endl;
    }

    simout << "-- number of processes: " << numProcesses << "\n";
    simout << "-- number of CPU-bound processes: " << numCPUProcesses << "\n";
    simout << "-- number of I/O-bound processes: " << numProcesses - numCPUProcesses << "\n";
    
    simout << std::fixed;
    simout.precision(3);

    double avg = 0;
    if (cpuBoundTotalCPUBursts > 0) {
        avg = ceil(cpuBoundCPUBurstSum*1000 / cpuBoundTotalCPUBursts) / 1000;
    }
    simout << "-- CPU-bound average CPU burst time: " << avg << " ms\n";

    avg = 0;
    if (ioBoundTotalCPUBursts > 0) {
        avg = ceil(ioBoundCPUBurstSum*1000 / ioBoundTotalCPUBursts) / 1000;
    }
    simout << "-- I/O-bound average CPU burst time: " << avg << " ms\n";
    
    avg = 0;
    if (cpuBoundTotalCPUBursts + ioBoundTotalCPUBursts > 0) {
        avg = ceil((cpuBoundCPUBurstSum + ioBoundCPUBurstSum)*1000 / (cpuBoundTotalCPUBursts + ioBoundTotalCPUBursts)) / 1000;
    }
    simout << "-- overall average CPU burst time: " << avg << " ms\n";
    
    avg = 0;
    if (cpuBoundTotalIOBursts > 0) {
        avg = ceil(cpuBoundIOBurstSum*1000 / cpuBoundTotalIOBursts) / 1000;
    }
    simout << "-- CPU-bound average I/O burst time: " << avg << " ms\n";
    
    avg = 0;
    if (ioBoundTotalIOBursts > 0) {
        avg = ceil(ioBoundIOBurstSum*1000 / ioBoundTotalIOBursts) / 1000;
    }
    simout << "-- I/O-bound average I/O burst time: " << avg << " ms\n";
    
    avg = 0;
    if (cpuBoundTotalIOBursts + ioBoundTotalIOBursts > 0) {
        avg = ceil((cpuBoundIOBurstSum + ioBoundIOBurstSum)*1000 / (cpuBoundTotalIOBursts + ioBoundTotalIOBursts)) / 1000;
    }
    simout << "-- overall average I/O burst time: " << avg << " ms\n";
}