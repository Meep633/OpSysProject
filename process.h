#ifndef __process_h_
#define __process_h_

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "burst.h"

class Process {
public:
    Process();
    Process(std::string name, int arrivalTime, bool type);
    void addBurst(int burstTime, bool burstType); //true = cpu burst, false = i/o burst
    void removeBurst(unsigned int ind);
    Burst& burst(unsigned int ind);
    unsigned int numBursts() const;
    unsigned int numCPUBursts() const;
    std::string name() const;
    int arrivalTime() const;
    bool type() const;
    bool contextSwitching() const;
    void setContextSwitching(bool contextSwitching);
    int tau() const;
    void setTau(int tau);
    std::ostream& output(std::ostream &out_str); //for testing
private:
    std::vector<Burst> _bursts;
    int _numCPUBursts;
    std::string _name;
    int _arrivalTime;
    bool _type; //true = cpu-bound, false = i/o-bound
    bool _contextSwitching;
    int _tau;
};

#endif