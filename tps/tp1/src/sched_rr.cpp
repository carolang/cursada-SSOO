#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	int cpus = argn[0];
	int i;
	// Cargo los quantum de las cpus
	for(i=1; i<=cpus; i++){
		quantum.push_back(argn[i]);
		elapsed_ticks.push_back(0);
		current_pid.push_back(IDLE_TASK);
	}
	current = -1;
	total = 0;
}

SchedRR::~SchedRR() {

}

void SchedRR::load(int pid) {
	total++;
	ready.push_back(true);
	processing.push_back(false);
}

void SchedRR::unblock(int pid) {
	ready[pid] = true;
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	elapsed_ticks[cpu]++;
	bool quantum_change=false;
	// Se fija si cambia el quantum en este tick
	if(elapsed_ticks[cpu] == quantum[cpu]){
		elapsed_ticks[cpu] = 0;
		quantum_change = true;
	}
	
	int cur = current_pid[cpu];
	
	// Bloqueo la tarea si hay que bloquearla
	if(m == EXIT || m == BLOCK){
		block(cur);
	}
	
	// Si no hay cambio de quantum puede ser que siga la misma o que pase a IDLE_TASK
	if(!quantum_change){
		if(m == TICK) return current_pid[cpu];
		
		processing[cur] = false;
		current_pid[cpu] = IDLE_TASK;
		return IDLE_TASK;
	}
	else{
		// Hay cambio de quantum
		if(cur != IDLE_TASK) processing[cur] = false;
		
		int sig = next(cpu);	// next() se encarga de cambiar los flags de sig
		return sig;
	}
}

int SchedRR::next(int cpu) {
	int ava = available();
	if(ava == 0){
		current_pid[cpu] = IDLE_TASK;
		return IDLE_TASK;
	}
	
	// Hay al menos un proceso disponible
	current = (current+1)%total;
	while(!ready[current] || processing[current]){
		current = (current+1)%total;
	}
	
	processing[current] = true;
	current_pid[cpu] = current;
	return current;
}

void SchedRR::block(int pid){
	if(pid == IDLE_TASK) return;
	ready[pid] = false;
}

int SchedRR::available(){
	int res=0;
	for(int i=0;i<total;i++){
		if(ready[i] && !processing[i]){
			res++;
		}
	}
	return res;
}
