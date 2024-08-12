#ifndef __algos_h_
#define __algos_h_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include "process.h"
#include "burst.h"

void FCFS(const std::vector<Process> &processes, int numProcesses, int t_cs, std::ofstream &simout);
void SJF();
void SRT();
void RR();

#endif