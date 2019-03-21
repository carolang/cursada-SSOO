#include <vector>
#include <queue>
#include "sched_rr2.h"
#include "basesched.h"
#include <iostream>

using namespace std;

SchedRR2::SchedRR2(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus cpu_quantum por parámetro
	cpus = argn[0];
	// Cargo los quantum de las cpus
	for(int i=1; i<=cpus; i++){
		quantum.push_back(argn[i]);
		elapsed_ticks.push_back(0);
		task_list.push_back(vector<int>());
		curr_idx.push_back(-1);
	}
	total = 0;
}

SchedRR2::~SchedRR2() {
}

void SchedRR2::load(int pid) {
	int mini = 0;
	int pMini = pid + 1;			// Un numero mas alto que cualquier cola de procesos de cualquier CPU
	for(int i=0; i<cpus; i++){
		int pI = total_proc(i);
		if(pI < pMini){
			pMini = pI;
			mini = i;
		}
	}
	
	total++;
	assigned_cpu.push_back(mini);
	ready.push_back(true);
	processing.push_back(false);
	
	task_list[mini].push_back(pid);
}

void SchedRR2::unblock(int pid) {
	ready[pid] = true;
}

int SchedRR2::tick(int cpu, const enum Motivo m) {
	elapsed_ticks[cpu]++;
	bool quantum_change=false;
	// Se fija si cambia el quantum en este tick
	if(elapsed_ticks[cpu] == quantum[cpu]){
		elapsed_ticks[cpu] = 0;
		quantum_change = true;
	}
	
	int cur = current_pid(cpu);
	
	// Bloqueo la tarea si hay que bloquearla
	if(m == EXIT || m == BLOCK){
		block(cur);
	}
	// Si ademas termino, la descargo
	if(m == EXIT){
		assigned_cpu[cur] = NULL_CPU;
	}
	
	// Si no hay cambio de quantum puede ser que siga la misma o que pase a IDLE_TASK
	if(!quantum_change){
		if(m == TICK) return current_pid(cpu);
		
		// Si no es un tick
		processing[cur] = false;
		return IDLE_TASK;
	}
	else{
		// Hay cambio de quantum
		if(cur != IDLE_TASK) processing[cur] = false;
		
		int sig = next(cpu);	// next() se encarga de cambiar los flags de sig
		return sig;
	}
}

int SchedRR2::next(int cpu) {
	int ava = available(cpu);
	if(ava == 0) return IDLE_TASK;
	
	// Hay al menos un proceso disponible
	int tot = task_list[cpu].size();
	
	curr_idx[cpu] = (curr_idx[cpu] + 1) % tot;
	int curr_task = task_list[cpu][curr_idx[cpu]];
	
	while(!ready[curr_task] || processing[curr_task]){
		curr_idx[cpu] = (curr_idx[cpu] + 1) % tot;
		curr_task = task_list[cpu][curr_idx[cpu]];
	}
	
	processing[curr_task] = true;
	return curr_task;
}

void SchedRR2::block(int pid){
	if(pid == IDLE_TASK) return;
	ready[pid] = false;
}

int SchedRR2::available(int cpu) {
	int res=0;
	for(int i=0;i<total;i++){
		if(ready[i] && !processing[i] && assigned_cpu[i] == cpu){
			res++;
		}
	}
	return res;
}

int SchedRR2::total_proc(int cpu) {
	int res = 0;
	for(int i=0; i<total; i++){
		if(assigned_cpu[i] == cpu) res++;
	}
	return res;
}

int SchedRR2::current_pid(int cpu){
	if(curr_idx[cpu] == -1) return IDLE_TASK;	// No comenzo la ejecucion
	
	int cur = task_list[cpu][curr_idx[cpu]];
	if(processing[cur]) return cur;
	else return IDLE_TASK;
}
