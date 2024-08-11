#ifndef __process_h_
#define __process_h_

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "burst.h"

class Process {
public:
    Process(std::string name, double arrivalTime, bool type);
    void addBurst(double burstTime, bool burstType); //true = cpu burst, false = i/o burst
    const Burst& burst(unsigned int ind) const;
    std::string name() const;
    double arrivalTime() const;
    unsigned int numBursts() const;
    bool type() const;
    std::ostream& output(std::ostream &out_str); //for testing
private:
    std::string _name;
    double _arrivalTime;
    bool _type; //true = cpu-bound, false = i/o-bound
    std::vector<Burst> _bursts;
};

#endif