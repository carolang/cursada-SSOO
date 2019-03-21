#ifndef __SCHED_FIXED_
#define __SCHED_FIXED_

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std;

class SchedFixed : public SchedBase {
  public:
    SchedFixed(std::vector<int> argn);
    ~SchedFixed();
    virtual void initialize();
    virtual void load(int pid);
    virtual void unblock(int pid);
    virtual int tick(int cpu, const enum Motivo m);

  private:
    int cpus;
    std::vector<int> current_idx;

    int next(int cpu);

    std::vector<bool> ready;
    std::vector<bool> processing;
};

#endif
