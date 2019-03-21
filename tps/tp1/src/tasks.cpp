#include "tasks.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

void TaskCPU(int pid, vector<int> params) { // params: n
	uso_CPU(pid, params[0]); // Uso el CPU n milisegundos.
}

void TaskIO(int pid, vector<int> params) { // params: ms_pid, ms_io,
	uso_CPU(pid, params[0]); // Uso el CPU ms_pid milisegundos.
	uso_IO(pid, params[1]); // Uso IO ms_io milisegundos.
}

void TaskAlterno(int pid, vector<int> params) { // params: ms_pid, ms_io, ms_pid, ...
	for(int i = 0; i < (int)params.size(); i++) {
		if (i % 2 == 0) uso_CPU(pid, params[i]);
		else uso_IO(pid, params[i]);
	}
}

void TaskConBloqueo(int pid, vector<int> params) { // params: tiempo, inicioBloq, finBloq
	int tiempo = params[0];
	int inicioBloq = params[1];
	int finBloq = params[2];

	if(inicioBloq > tiempo){
		uso_CPU(pid, tiempo);
	}else{
		uso_CPU(pid,inicioBloq-2);
		uso_IO(pid,finBloq-inicioBloq+1);
		uso_CPU(pid,tiempo-finBloq);
	}
}

/* Tasks programadas por nosotros */

void TaskConsola(int pid, vector<int> params){ // params: n, bmin, bmax
	int n = params[0];
	int bmin = params[1];
	int bmax = params[2];
	int i;
	for(i=0;i<n;i++){
		int d = rand() % (bmax - bmin + 1) + bmin;	// d en el rango [bmin, bmax]
		uso_IO(pid, d);
	}
}

void TaskBatch(int pid, vector<int> params){	// params: total_cpu, cant_bloqueos
	int total_cpu = params[0];
	int cant_bloqueos = params[1];
	
	// Definir los momentos de bloqueo
	bool hay_bloqueo[total_cpu];
	int bloqueos_actuales = 0;
	//cant_bloqueos = min(cant_bloqueos, total_cpu*2/3);	// Si la cantidad de bloqueos es grande el random es un bajon
	while(bloqueos_actuales < cant_bloqueos){
		int t = rand()%total_cpu;
		if(!hay_bloqueo[t]){
			hay_bloqueo[t] = true;
			bloqueos_actuales++;
		}
	}
	
	// Ejecutar el proceso
	int i;
	for(i=0;i<total_cpu;i++){
		if(hay_bloqueo[i]){
			uso_IO(pid, 1);			// Usa su quantum en ejec. llamada bloqueante
		}
		else{
			uso_CPU(pid, 1);		// Usa su quantum en comer CPU
		}
	}
}

void tasks_init(void) {
	/* Todos los tipos de tareas se deben registrar acá para poder ser usadas.
	 * El segundo parámetro indica la cantidad de parámetros que recibe la tarea
	 * como un vector de enteros, o -1 para una cantidad de parámetros variable. */
	register_task(TaskCPU, 1);
	register_task(TaskIO, 2);
	register_task(TaskAlterno, -1);
	register_task(TaskConBloqueo, 3);
	register_task(TaskConsola, 3);
	register_task(TaskBatch, 2);
}
