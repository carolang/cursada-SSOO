#ifndef RWLock_h
#define RWLock_h
#include <iostream>

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
        //pthread_rwlock_t rwlock;
        
        bool hayAlguien;
        bool hayEscritor;
        int lectoresDentro;
        
        pthread_cond_t noHayEscritores_cv;
        pthread_cond_t noHayNadieDentro_cv;
        pthread_mutex_t m;
        pthread_mutex_t barrera;
};

#endif
