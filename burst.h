#ifndef __burst_h_
#define __burst_h_

class Burst {
public:
    double time;
    bool type; //true = cpu, false = i/o
    Burst(double time, bool type) {
        this->time = time;
        this->type = type;
    }
};

#endif