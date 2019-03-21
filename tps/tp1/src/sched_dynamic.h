#ifndef __SCHED_DYNAMIC_
#define __SCHED_DYNAMIC_

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedDynamic : public SchedBase {
	public:
		SchedDynamic(std::vector<int> argn);
        	~SchedDynamic();
		virtual void initialize();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int cpus;
		std::vector<int> current_idx;
	
		int next(int cpu);
		int priority(int pid);
			
		std::vector<bool> ready;
		std::vector<bool> processing;
		std::vector<int> deadline;
};

#endif
