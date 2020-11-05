#include "../lib/SRTNP.h"

process* SRTNP_tick (process* running_process){
	if (running_process == NULL || running_process->time_left == 0) {
		if (SRTNP_queue->next != NULL) {
			queue_object* q_obj = SRTNP_queue->next;
			process* p = q_obj->object;

			int min_time = p->time_left;
			int index = 0;		// index of the process with the lowest remaining time

			int zaehler = 1;
			while (q_obj->next != NULL) {
				// inspect next queue_object
				q_obj = q_obj->next;
				p = q_obj->object;

				if (p->time_left < min_time) {
					min_time = p->time_left;
					index = zaehler;
				}
				zaehler++;
			}

			// index-ten Prozess aus der queue entfernen und als running_process waehlen
			queue_object* previous = SRTNP_queue->next;

			if (index == 0) {
				SRTNP_queue->next = SRTNP_queue->next->next;
				running_process = previous->object;
				free(previous);
			}
			else {
				// previous auf die Stelle VOR der index-ten stellen
				for (int i = 0; i < index-1; i++) {
					previous = previous->next;
				}
				queue_object* buf = previous->next;
				previous->next = buf->next;
				running_process = buf->object;

				free(buf);
			}
		}
		else running_process = NULL;
	}
	// nach Prozess mit geringerer Restlaufzeit suchen => mögliche Verdrängung
	else {
		queue_object* q_obj = SRTNP_queue;
		process* p;

		int min_time = running_process->time_left;
		int index = -1;		// index of the process with the lowest remaining time, currently indicating that the running process has the lowest remaining time

		int zaehler = 0;
		while (q_obj->next != NULL) {
			// inspect next queue_object
			q_obj = q_obj->next;
			p = q_obj->object;

			if (p->time_left < min_time) {
				min_time = p->time_left;
				index = zaehler;
			}
			zaehler++;
		}

		// index-ten Prozess aus der queue entfernen und als running_process waehlen
		queue_object* previous = SRTNP_queue->next;

		if (index == 0) {
			queue_add(running_process, SRTNP_queue);

			SRTNP_queue->next = SRTNP_queue->next->next;
			running_process = previous->object;
			free(previous);
		}
		else if (index > 0) {
			queue_add(running_process, SRTNP_queue);

			// previous auf die Stelle VOR der index-ten stellen
			for (int i = 0; i < index-1; i++) {
				previous = previous->next;
			}
			queue_object* buf = previous->next;
			previous->next = buf->next;
			running_process = buf->object;

			free(buf);
		}
	}

	if (running_process != NULL) running_process->time_left--;

    return running_process;
}

int SRTNP_startup(){
	SRTNP_queue = new_queue();
	if (SRTNP_queue == NULL){
		return 1;
	}
    return 0;
}

process* SRTNP_new_arrival(process* arriving_process, process* running_process){
    if (arriving_process != NULL) {
    	// Verdrängung
    	if (running_process != NULL && (arriving_process->time_left < running_process->time_left)) {
    		queue_add(running_process, SRTNP_queue);
    		running_process = arriving_process;
    	}
    	// keine Verdrängung
    	else {
    		queue_add(arriving_process, SRTNP_queue);
    	}
    }
    return running_process;
}

void SRTNP_finish(){
    free_queue(SRTNP_queue);
}
