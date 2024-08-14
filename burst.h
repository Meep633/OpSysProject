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
        this->_initialArrivalTime = 0;
        this->_latestArrivalTime = 0;
        this->_numPreemptions = 0;
    }
    int burstTime() const { return this->_burstTime; }
    bool type() const { return this->_type; }
    int remainingTime() const { return this->_remainingTime; }
    void setRemainingTime(int time) { this->_remainingTime = time; }
    int turnaroundTime() const { return this->_turnaroundTime; }
    void setTurnaroundTime(int time) { this->_turnaroundTime = time; }
    int waitTime() const { return this->_waitTime; }
    void setWaitTime(int time) { this->_waitTime = time; }
    int initialArrivalTime() const { return this->_initialArrivalTime; }
    void setInitialArrivalTime(int time) { this->_initialArrivalTime = time; }
    int latestArrivalTime() const { return this->_latestArrivalTime; }
    void setLatestArrivalTime(int time) { this->_latestArrivalTime = time; }
    int numPreemptions() const { return this->_numPreemptions; }
    void incrementPreemptions() { this->_numPreemptions++; }
private:
    int _burstTime;
    bool _type; //true = cpu, false = i/o
    int _remainingTime;
    int _turnaroundTime;
    int _waitTime;
    int _initialArrivalTime;
    int _latestArrivalTime; //new arrival time after being preempted
    int _numPreemptions;
};

#endif