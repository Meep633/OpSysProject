#include "process.h"

Process::Process(std::string name, double arrivalTime, bool type) {
    this->_name = name;
    this->_arrivalTime = arrivalTime;
    this->_type = type;
    this->_bursts = std::vector<Burst>();
}

void Process::addBurst(double burstTime, bool burstType) {
    this->_bursts.push_back(Burst(burstTime, burstType));
}

const Burst& Process::burst(unsigned int ind) const {
    return this->_bursts[ind];
}

std::string Process::name() const {
    return this->_name;
}

double Process::arrivalTime() const {
    return this->_arrivalTime;
}

unsigned int Process::numBursts() const {
    return this->_bursts.size();
}

bool Process::type() const {
    return this->_type;
}

std::ostream& Process::output(std::ostream &out_str) {
    out_str << this->_name << ": ";
    if (this->_type) {
        out_str << "CPU-bound ";
    } else {
        out_str << "I/O-bound ";
    }
    out_str << "arrived at " << this->_arrivalTime << "ms" << std::endl;
    for (unsigned int i = 0; i < this->_bursts.size(); i++) {
        if (i % 4 == 0 && i > 0) {
            out_str << std::endl;
        }
        if (this->_bursts[i].type) {
            out_str << "CPU ";
        } else {
            out_str << "I/O ";
        }
        out_str << std::setw(5) << this->_bursts[i].time << "ms";
        if (i != this->_bursts.size()-1) {
            out_str << ", ";
        }
    }
    return out_str;
}