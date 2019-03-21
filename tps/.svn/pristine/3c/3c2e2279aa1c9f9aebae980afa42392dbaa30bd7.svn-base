#include "sched_dynamic.h"
#include <iostream>

using namespace std;

SchedDynamic::SchedDynamic(vector<int> argn) {
	cpus = argn[0];
	for(int i=0;i<cpus;i++)
		current_idx.push_back(IDLE_TASK);
}

SchedDynamic::~SchedDynamic() {
}

void SchedDynamic::initialize() {
}

void SchedDynamic::load(int pid) {
	while((int)(ready.size()) <= pid){
		ready.push_back(false);
		processing.push_back(false);
		deadline.push_back(-1);
	}
	ready[pid] = true;
	deadline[pid] = current_time() + period(pid);		// La deadline es el tiempo hasta el proximo request
}

void SchedDynamic::unblock(int pid) {
	ready[pid] = true;
}

int SchedDynamic::tick(int cpu, const enum Motivo m) {
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

int SchedDynamic::next(int cpu){
	int pid = current_pid(cpu);
	int p; 
	
	if(pid == IDLE_TASK || !ready[pid]){
		pid = IDLE_TASK;
		p = -1;
	}
	else p = priority(pid);
	
	// Busco entre los otros procesos a ver si hay otro con mas prioridad
	int tot = ready.size();
	for(int i=0; i<tot; i++){
		if(!ready[i] || processing[i]) continue;
		
		int pnew = priority(i);
		if(p == -1 || p > pnew){		// Considero que un proceso tiene mas prioridad cuando el numero es menor (Intel, gracias por tanto, perdon por tan poco)
			p = pnew;
			pid = i;
		}
	}
	return pid;
}

int SchedDynamic::priority(int pid){
	return deadline[pid];	// El numero es mas chico (prioritario) cuando la deadline es mas proxima
}
