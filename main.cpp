#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#include "process.h"
#include "random.h"

int main(int argc, char **argv) {
    //Parse arguments
    if (argc != 6) {
        std::cerr << "ERROR: Invalid number of arguments" << std::endl;
        return EXIT_FAILURE;
    }

    int numProcesses;
    int numCPUProcesses;
    long seed;
    double lambda;
    int upperBound;

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

    srand48(seed);

    //Generate processes
    std::vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) { 
        std::string name(1, i/10 + 'A');
        name += std::to_string(i%10);
        
        double arrivalTime = floor(nextExp(lambda));
        while (arrivalTime > upperBound) {
            arrivalTime = floor(nextExp(lambda));
        }
        
        bool type = true;
        if (i >= numCPUProcesses) {
            type = false;
        }
        processes.push_back(Process(name, arrivalTime, type));
        
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

            processes[processes.size()-1].addBurst(cpuTime, true);
            processes[processes.size()-1].addBurst(ioTime, false);
        }
        int cpuTime = ceil(nextExp(lambda));
        while (cpuTime > upperBound) {
            cpuTime = ceil(nextExp(lambda));
        }
        if (i < numCPUProcesses) {
            cpuTime *= 4;
        }
        processes[processes.size()-1].addBurst(cpuTime, true);
    }

    //Part 1 output
    double cpuBoundCPUBurstSum = 0;
    int cpuBoundTotalCPUBursts = 0;
    double ioBoundCPUBurstSum = 0;
    int ioBoundTotalCPUBursts = 0;
    double cpuBoundIOBurstSum = 0;
    int cpuBoundTotalIOBursts = 0;
    double ioBoundIOBurstSum = 0;
    int ioBoundTotalIOBursts = 0;

    std::cout << "<<< PROJECT PART I" << std::endl;
    std::cout << "<<< -- process set (n=" << numProcesses << ") with " << numCPUProcesses << " CPU-bound process";
    if (numCPUProcesses != 1) {
        std::cout << "es";
    }
    std::cout << std::endl; //need lambda to be exactly 6 digits after decimal
    std::cout << "<<< -- seed=" << seed << "; lambda=";
    std::cout << std::fixed << lambda;
    std::cout << std::defaultfloat << "; bound=" << upperBound << std::endl;
    for (unsigned int i = 0; i < processes.size(); i++) {
        if (processes[i].type()) {
            std::cout << "CPU-bound process ";
        } else {
            std::cout << "I/O-bound process ";
        }
        std::cout << processes[i].name() << ": arrival time " << processes[i].arrivalTime() << "ms; " << processes[i].numBursts()/2 + 1 << " CPU burst";
        if (processes[i].numBursts()/2 + 1 > 1) {
            std::cout << "s";
        }
        std::cout << ":" << std::endl;
        for (unsigned int j = 0; j < processes[i].numBursts(); j++) {
            std::cout << "==> ";
            if (j % 2 == 0) {
                std::cout << "CPU burst " << processes[i].burst(j).time << "ms";
                if (j != processes[i].numBursts()-1) {
                    std::cout << " ";
                }
                if ((int)i < numCPUProcesses) {
                    cpuBoundCPUBurstSum += processes[i].burst(j).time;
                    cpuBoundTotalCPUBursts++;
                } else {
                    ioBoundCPUBurstSum += processes[i].burst(j).time;
                    ioBoundTotalCPUBursts++;
                }
            } else {
                std::cout << "I/O burst " << processes[i].burst(j).time << "ms" << std::endl;
                if ((int)i < numCPUProcesses) {
                    cpuBoundIOBurstSum += processes[i].burst(j).time;
                    cpuBoundTotalIOBursts++;
                } else {
                    ioBoundIOBurstSum += processes[i].burst(j).time;
                    ioBoundTotalIOBursts++;
                }
            }
        }
        std::cout << std::endl;
    }

    std::ofstream simout("simout.txt");

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
    
    simout.close();

    return EXIT_SUCCESS;
}