#include "prio_q.h"

struct prio_q * prio_q_create()
{
	struct prio_q * list = malloc(sizeof(struct prio_q));
	list->size = 0;
	list->front = NULL;

	return list;
}

void prio_q_enqueue(struct prio_q *q, void *data, int prio)
{
	struct prio_q_elem * newElement = malloc(sizeof(struct prio_q_elem));
	newElement->data = data;
	newElement->priority = prio;

	// search for right spot to insert queue element
	if (q->size == 0) {
		q->front = newElement;
		newElement->next = NULL;
	}
	else {
		// go through list and search for first queue element with a smaller priority than the inserted one

		// in case that newElement has the highest priority
		if (newElement->priority > q->front->priority) {
			newElement->next = q->front;
			q->front = newElement;
		}
		else {
			// list element right in front of the inspected one
			struct prio_q_elem * previous = q->front;

			// i = 1 because the first element is already skipped
			for (int i = 1; i < q->size; i++) {
				// previous->next == currently inspected

				if (previous->next->priority < prio) {
					// spot to insert found
					// insert newElement in front of the currently inspected queue element
					newElement->next = previous->next;
					previous->next = newElement;

					// jump out of the for
					break;
				}
				// in case that newElement has the lowest priority of all
				else if (i == q->size - 1) {
					// add newElement to the end of the list
					previous->next->next = newElement;

					newElement->next = NULL;
				}

				// set new previous
				previous = previous->next;
			}
		}
	}

	// increase size of q
	(q->size)++;
}

void * prio_q_dequeue(struct prio_q *q)
{
	if (q->size == 0) return NULL;
	else {
		// handle dequeuing
		struct prio_q_elem * queue_elem = q->front;
		q->front = queue_elem->next;
		(q->size)--;

		// handle destroying of the element
		void * data = queue_elem->data;
		free(queue_elem);


		return data;
	}
}

void * prio_q_front(struct prio_q *q)
{
	if (q->size == 0) return NULL;
	else return q->front->data;
}

int prio_q_destroy(struct prio_q *q, void ** data)
{
	// number of returned data elements
	int n = 0;
	// buffer for returned data
	void * elemData = NULL;

	for (int i = 0; i < q->size; i++) {
		elemData = prio_q_dequeue(q);

		// copy element data in array 'data' only if it's not NULL
		if (data != NULL) {
			*(data + i) = elemData;
			n++;
		}
	}

	// destroy priority queue list
	free(q);
	return n;
}

void prio_q_print(struct prio_q * q, void (*print_data)(void*))
{
	if (q->size != 0) {
		printf("=> list remaining queues:\n");
		struct prio_q_elem * elem = q->front;
		do{
			print_data(elem->data);
			printf(": priority %d\n", elem->priority);

			elem = elem->next;
		} while (elem != NULL);
		printf("\n");
	}
	else printf("=> no remaining queues!\n\n");
}
