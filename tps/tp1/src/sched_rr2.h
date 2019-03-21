#ifndef __SCHED_RR2__
#define __SCHED_RR2__

#include <vector>
#include <queue>
#include "basesched.h"

class SchedRR2 : public SchedBase {
	public:
		SchedRR2(std::vector<int> argn);
        	~SchedRR2();
		virtual void initialize() {};
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int next(int cpu);
		void block(int pid);
		int available(int cpu);
		// Para administrar los quantums
		int cpus;
		std::vector<int> quantum;
		std::vector<int> elapsed_ticks;
		// Para ciclar los procesos
		std::vector<std::vector<int> > task_list;	// Historico
		std::vector<int> curr_idx;
		std::vector<int> assigned_cpu;
		std::vector<bool> ready;
		std::vector<bool> processing;
		int current_pid(int cpu);
		// Para asignar cpus
		int total;
		int total_proc(int cpu);
};

#endif
