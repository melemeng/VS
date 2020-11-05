#include "../lib/PRIOP.h"
#include <stdio.h>


process* PRIOP_tick (process* running_process){
	if (running_process == NULL || running_process->time_left == 0) {
		// Suche des Prozesses mit der höchsten Priorität
		queue_object* q_obj = PRIOP_queue->next;

		if (q_obj != NULL) {
			process* p = q_obj->object;

			int max_prio = p->priority;
			int index = 0;		// index of the process with the highest priority

			int zaehler = 1;
			while (q_obj->next != NULL) {
				// inspect next queue_object
				q_obj = q_obj->next;
				p = q_obj->object;

				if (p->priority > max_prio) {
					max_prio = p->priority;
					index = zaehler;
				}
				zaehler++;
			}

			// index-ten Prozess aus der queue entfernen und als running_process waehlen

			queue_object* previous = PRIOP_queue->next;

			if (index == 0) {
				PRIOP_queue->next = PRIOP_queue->next->next;
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

	if (running_process!=NULL) {
		(running_process->time_left)--;
	}

    return running_process;
}

int PRIOP_startup(){
	PRIOP_queue = new_queue();
	if (PRIOP_queue == NULL){
		return 1;
	}
    return 0;
}

process* PRIOP_new_arrival(process* arriving_process, process* running_process){
	if (arriving_process != NULL) {
		// Verdrängung
		if (running_process != NULL && (arriving_process->priority > running_process->priority)) {
			queue_add(running_process, PRIOP_queue);
			running_process = arriving_process;
		}
		// keine Verdrängung
		else
			queue_add(arriving_process, PRIOP_queue);
	}

    return running_process;
}

void PRIOP_finish(){
    free_queue(PRIOP_queue);
}
