#include "sched_fixed.h"
#include <iostream>

using namespace std;

SchedFixed::SchedFixed(vector<int> argn) {
	cpus = argn[0];
	for(int i=0;i<cpus;i++)
		current_idx.push_back(IDLE_TASK);
}

SchedFixed::~SchedFixed() {
	
}

void SchedFixed::initialize() {
	
}

void SchedFixed::load(int pid) {
	while(ready.size() <= pid){
		ready.push_back(false);
		processing.push_back(false);
	}
	ready[pid] = true;
}

void SchedFixed::unblock(int pid) {
	ready[pid] = true;
}

int SchedFixed::tick(int cpu, const enum Motivo m) {
	int pid = current_pid(cpu);
	int sig;
	
	switch(m){
		case EXIT:
		case BLOCK:
			if(pid != IDLE_TASK){
				ready[pid] = false;
				processing[pid] = false;
			}
			
			sig = next(cpu);
			if(sig != IDLE_TASK){
				processing[sig] = true;
				return sig;
			}
			return IDLE_TASK;
			
		case TICK:
			if(pid != IDLE_TASK) processing[pid] = false;	// Para que aparezca en la lista de prioridades
			
			sig = next(cpu);
			if(sig != IDLE_TASK){
				processing[sig] = true;
				return sig;
			}
			return IDLE_TASK;
	}
	cerr << "Algo aca fallo\n";
    return -2;
}

int SchedFixed::next(int cpu){
	int pid = current_pid(cpu);
	int p; 
	
	if(pid == IDLE_TASK || !ready[pid]){
		pid = IDLE_TASK;
		p = -1;
	}
	else p = period(pid);
	
	// Busco entre los otros procesos a ver si hay otro con mas prioridad
	int tot = ready.size();
	for(int i=0; i<tot; i++){
		if(!ready[i] || processing[i]) continue;
		
		int pnew = period(i);
		if(p == -1 || p > pnew){		// Considero que un proceso tiene mas prioridad cuando el numero es menor (Intel, gracias por tanto, perdon por tan poco)
			p = pnew;
			pid = i;
		}
	}
	return pid;
}
