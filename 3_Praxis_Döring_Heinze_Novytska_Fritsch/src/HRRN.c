#include "../lib/HRRN.h"


int current_time = 0;

process* HRRN_tick (process* running_process){
	if (running_process == NULL || running_process->time_left == 0) {
		if (HRRN_queue->next != NULL) {
			double max_rr;
			int index;
			int zaehler;
			queue_object* buf;
			process* p;

			buf = HRRN_queue->next;
			p = buf->object;
			max_rr = (double) (current_time + p->time_left - p->start_time)/p->service_time;
			index = 0;
			zaehler = 1;

			// Prozess mit höchster rr suchen
			while (buf->next != NULL) {
				buf = buf->next;
				p = buf->object;

				double rr = (current_time + p->time_left - p->start_time)/p->service_time;
				if (rr > max_rr) {
					max_rr = rr;
					index = zaehler;
				}
				zaehler++;
			}

			queue_object* previous = HRRN_queue->next;
			if (index == 0) {
				HRRN_queue->next = previous->next;
				running_process = previous->object;
				free(previous);
			}
			else if (index > 0) {
				// previous auf die Stelle VOR der index-ten stellen
				for (int i = 0; i < index-1; i++) {
					previous = previous->next;
				}
				queue_object* q_obj = previous->next;
				previous->next = q_obj->next;
				running_process = q_obj->object;

				free(q_obj);
			}
		}
		else running_process = NULL;
	}

	if (running_process != NULL)
		running_process->time_left--;
	current_time++;
    return running_process;
}

int HRRN_startup(){
	HRRN_queue = new_queue();
	if (HRRN_queue == NULL) {
		return 1;
	}
    return 0;
}

process* HRRN_new_arrival(process* arriving_process, process* running_process){
	if (arriving_process != NULL) {
		arriving_process->service_time = arriving_process->time_left;
		queue_add(arriving_process, HRRN_queue);
	}
    return running_process;
}

void HRRN_finish(){
    free_queue(HRRN_queue);
}
