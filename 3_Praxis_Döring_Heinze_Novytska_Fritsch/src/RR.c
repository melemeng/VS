#include "../lib/RR.h"

process* RR_tick (process* running_process){
	// neuen Prozess suchen, da keiner ausgewählt ist oder der ausgewählte
	// bereits fertig ist
	if (running_process == NULL || running_process->time_left == 0) {
		running_process = queue_poll(RR_queue);
		time_left_quantum = this_quantum;
	}
	// Umschalten, da Zeitscheibe abgelaufen
	else if (time_left_quantum == 0) {
		// laufenden Prozess in Warteschlange stellen
		queue_add(running_process, RR_queue);

		// neuen laufenden Prozess bestimmen
		running_process = queue_poll(RR_queue);

		time_left_quantum = this_quantum;
	}


	if (running_process != NULL) (running_process->time_left)--;
	time_left_quantum--;

    return running_process;
}

int RR_startup(int quantum){
    this_quantum = quantum;
    time_left_quantum = 0;
    RR_queue = new_queue();

    if (RR_queue == NULL) return 1;
    return 0;
}


process* RR_new_arrival(process* arriving_process, process* running_process){
    if (arriving_process != NULL) queue_add(arriving_process, RR_queue);

    return running_process;
}


void RR_finish(){
    free_queue(RR_queue);
}
