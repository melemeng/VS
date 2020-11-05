#include "../lib/MLF.h"

int quantum_left = 0;

// vorheriges Level des laufenden Prozesses
int running_level;

// Modus der Abarbeitung: 0->RR, 1->FCFS
int mode = 0;


int MLF_startup(){
    MLF_queue = calloc(4, sizeof(queue_object));
    for (int i = 0; i < 4; i++) {
    	MLF_queue[i] = new_queue();
    	if (MLF_queue[i] == NULL) return 1;
    }
    return 0;
}


process* MLF_tick (process* running_process){
    if (running_process == NULL || running_process->time_left == 0 || (mode == 0 && quantum_left == 0)) {
    	// Warteliste wählen, aus der der nächste Prozess kommen soll
    	int level = 0;
    	while (level < 4 && MLF_queue[level]->next == NULL) {
    		level++;
    	}

    	// Aktuell laufender Prozess steigt in höheres Level und ist immer noch mit höchster Priorität
    	if (running_process != NULL && running_process->time_left != 0 && running_level + 1 < level) {
    		running_level++;
    		switch (running_level) {
			case 0: mode = 0; quantum_left = 1; break;
			case 1: mode = 0; quantum_left = 2; break;
			case 2: mode = 0; quantum_left = 4; break;
			case 3: mode = 1; break;
    		}
    	}
    	// Level mit Round Robin
    	else if (level < 4) {
    		if (running_process != NULL && running_process->time_left != 0)
    			queue_add(running_process, MLF_queue[running_level + 1]);

    		running_process = queue_poll(MLF_queue[level]);
    		running_level = level;

    		switch (level) {
    		case 0: mode = 0; quantum_left = 1; break;
    		case 1: mode = 0; quantum_left = 2; break;
    		case 2: mode = 0; quantum_left = 4; break;
    		case 3: mode = 1; break;
    		}
    	}
    	// keine Prozesse zum Wechseln vorhanden
    	else if (running_process != NULL && running_process->time_left != 0) {
    		// aktuellen Prozess in anderem Level weiterlaufen lassen
    		running_level++;
    		switch (running_level) {
			case 0: mode = 0; quantum_left = 1; break;
			case 1: mode = 0; quantum_left = 2; break;
			case 2: mode = 0; quantum_left = 4; break;
			case 3: mode = 1; break;
			}
    	}
    	else running_process = NULL;
    }

    if (running_process != NULL) {
    	running_process->time_left--;
    	if (mode == 0) quantum_left--;
    }
    return running_process;
}

process* MLF_new_arrival(process* arriving_process, process* running_process){
    if (arriving_process != NULL) {
    	queue_add(arriving_process, MLF_queue[0]);
    }
    return running_process;
}


void MLF_finish(){
	for (int i = 0; i < 4; i++) {
		free_queue(MLF_queue[i]);
	}
	free(MLF_queue);
}
