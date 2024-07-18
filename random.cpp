#include "random.h"

double nextExp(double lambda) {
    double r = drand48();
    return -log(r) / lambda;
}