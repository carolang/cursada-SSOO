#ifndef __SCHED_RR__
#define __SCHED_RR__

#include <vector>
#include <queue>
#include "basesched.h"

class SchedRR : public SchedBase {
	public:
		SchedRR(std::vector<int> argn);
        ~SchedRR();
		virtual void initialize() {};
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int next(int cpu);
		void block(int pid);
		int available();
		// Para administrar los quantums
		std::vector<int> quantum;
		std::vector<int> elapsed_ticks;
		std::vector<int> current_pid;
		// Para ciclar los procesos
		int current, total;
		std::vector<bool> ready;
		std::vector<bool> processing;
		
};

#endif
