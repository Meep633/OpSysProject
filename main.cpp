#include <iostream>
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
    std::cout << nextExp(lambda) << std::endl;

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
            int ioTime = ceil(nextExp(lambda)) * 8;
            if (i < numCPUProcesses) {
                cpuTime *= 4;
                ioTime /= 8;
            }
            processes[processes.size()-1].addCPUBurst(cpuTime, true);
            processes[processes.size()-1].addCPUBurst(ioTime, false);
        }
        int cpuTime = ceil(nextExp(lambda));
        processes[processes.size()-1].addCPUBurst(cpuTime, true);
    }
#ifdef DEBUG
    for (unsigned int i = 0; i < processes.size(); i++) {
        processes[i].output(std::cout);
        std::cout << "\n=============================================" << std::endl;
    }
#endif


    return EXIT_SUCCESS;
}