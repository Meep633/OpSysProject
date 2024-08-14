#include "process.h"

Process::Process() {
    this->_bursts = std::vector<Burst>();
    this->_numCPUBursts = 0;
    this->_name = "";
    this->_arrivalTime = 0;
    this->_type = true;
    this->_contextSwitching = false;
    this->_tau = 1000;
}
Process::Process(std::string name, int arrivalTime, bool type, double lambda) {
    this->_bursts = std::vector<Burst>();
    this->_numCPUBursts = 0;
    this->_name = name;
    this->_arrivalTime = arrivalTime;
    this->_type = type;
    this->_contextSwitching = false;
    this->_tau = 1/lambda;
}

void Process::addBurst(int burstTime, bool burstType) {
    this->_bursts.push_back(Burst(burstTime, burstType));
    if (burstType) { this->_numCPUBursts++; }
}
void Process::removeBurst(unsigned int ind) {
    std::vector<Burst>::iterator itr = this->_bursts.begin();
    for (unsigned int i = 0; i < ind; i++) { itr++; }
    if ((*itr).type()) { this->_numCPUBursts--; }
    this->_bursts.erase(itr);
}
Burst& Process::burst(unsigned int ind) {
    return this->_bursts[ind];
}
unsigned int Process::numBursts() const {
    return this->_bursts.size();
}
unsigned int Process::numCPUBursts() const {
    return this->_numCPUBursts;
}

std::string Process::name() const {
    return this->_name;
}
int Process::arrivalTime() const {
    return this->_arrivalTime;
}
bool Process::type() const {
    return this->_type;
}

bool Process::contextSwitching() const {
    return this->_contextSwitching;
}
void Process::setContextSwitching(bool contextSwitching) {
    this->_contextSwitching = contextSwitching;
}

int Process::tau() const {
    return this->_tau;
}
void Process::setTau(int tau) {
    this->_tau = tau;
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
        if (this->_bursts[i].type()) {
            out_str << "CPU ";
        } else {
            out_str << "I/O ";
        }
        out_str << std::setw(5) << this->_bursts[i].burstTime() << "ms";
        if (i != this->_bursts.size()-1) {
            out_str << ", ";
        }
    }
    return out_str;
}