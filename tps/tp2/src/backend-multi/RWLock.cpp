#include "RWLock.h"

RWLock :: RWLock() {
	hayAlguien = false;
	hayEscritor = false;
	lectoresDentro = 0;
	
	pthread_cond_init(&noHayEscritores_cv, NULL);
	pthread_cond_init(&noHayNadieDentro_cv, NULL);
	
	pthread_mutex_init(&m, NULL);
	pthread_mutex_init(&barrera, NULL);
}

void RWLock :: rlock() {
	pthread_mutex_lock(&m);
	
	// Veo que ningun writer este esperando
	pthread_mutex_lock(&barrera);
	pthread_mutex_unlock(&barrera);
	
	// Espero que se libere el recurso
	while(hayEscritor){
		pthread_cond_wait(&noHayEscritores_cv, &m);
	}
	
	lectoresDentro++;
	hayAlguien = true;
	pthread_mutex_unlock(&m);
}

void RWLock :: wlock() {
	pthread_mutex_lock(&m);
	pthread_mutex_lock(&barrera);
	
	// Espero que se libere el recurso
	while(hayAlguien){
		pthread_cond_wait(&noHayNadieDentro_cv, &m);
	}
	
	hayEscritor = true;
	hayAlguien = true;
	
	pthread_mutex_unlock(&barrera);
	pthread_mutex_unlock(&m);
}

void RWLock :: runlock() {
	pthread_mutex_lock(&m);
	lectoresDentro--;
	
	// Si soy el ultimo en irme, aviso que el recurso esta liberado
	if(lectoresDentro == 0){
		hayAlguien = false;
		pthread_cond_signal(&noHayNadieDentro_cv);
	}
	pthread_mutex_unlock(&m);
}

void RWLock :: wunlock() {
	pthread_mutex_lock(&m);
	
	hayAlguien = false;
	hayEscritor = false;
	
	// Aviso que el recurso esta liberado
	pthread_cond_signal(&noHayEscritores_cv);
	pthread_cond_signal(&noHayNadieDentro_cv);
	pthread_mutex_unlock(&m);
}
