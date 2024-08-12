#ifndef __burst_h_
#define __burst_h_

class Burst {
public:
    Burst(int burstTime, bool type) {
        this->_burstTime = burstTime;
        this->_type = type;
        this->_remainingTime = burstTime;
        this->_turnaroundTime = 0;
        this->_waitTime = 0;
        this->_arrivalTime = 0;
    }
    int burstTime() { return this->_burstTime; }
    bool type() { return this->_type; }
    int remainingTime() { return this->_remainingTime; }
    void setRemainingTime(int time) { this->_remainingTime = time; }
    int turnaroundTime() { return this->_turnaroundTime; }
    void setTurnaroundTime(int time) { this->_turnaroundTime = time; }
    int waitTime() { return this->_waitTime; }
    void setWaitTime(int time) { this->_waitTime = time; }
    int arrivalTime() { return this->_arrivalTime; }
    void setArrivalTime(int time) { this->_arrivalTime = time; }
private:
    int _burstTime;
    bool _type; //true = cpu, false = i/o
    int _remainingTime;
    int _turnaroundTime;
    int _waitTime;
    int _arrivalTime;
};

#endif