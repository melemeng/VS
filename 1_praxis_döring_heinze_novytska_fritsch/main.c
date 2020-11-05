#include "prio_q.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum data_type{DATA_CHAR, DATA_INT, DATA_STRING};

struct arb_data{
	enum data_type type;
	void * value;
};

struct arb_data* create_int(int a)
{
	struct arb_data * d = malloc(sizeof(struct arb_data));
	d->type = DATA_INT;
	d->value = malloc(sizeof(int));
	*((int*)d->value) = a;
	return d;
}

struct arb_data* create_char(char a)
{
	struct arb_data * d = malloc(sizeof(struct arb_data));
	d->type = DATA_CHAR;
	d->value = malloc(sizeof(char));
	*((char*)d->value) = a;
	return d;
}

struct arb_data* create_string(const char * a)
{
	struct arb_data * d = malloc(sizeof(struct arb_data));
	d->type = DATA_STRING;
	int size = strlen(a)+1;
	d->value = malloc(size);
	memcpy(d->value, a, size);
	return d;
}

void free_data(struct arb_data* d)
{
	free(d->value);
	free(d);
}

void print_data(const struct arb_data * d)
{
	switch(d->type)
	{
	case DATA_CHAR:
		printf("'%c'", *((char*)d->value));
	break;
	case DATA_INT:
		printf("%d", *((int*)d->value));
	break;
	case DATA_STRING:
		printf("\"%s\"", (char*)d->value);
	break;
	default:
		printf("<UNKNOWN>");
	}
}

void print_data_void(void * d)
{
	print_data((struct arb_data*)d);
}

int main()
{
	struct prio_q * q = prio_q_create();

	printf("-> enqueuing: 13(prio=1), 'a'(prio=5), 8(prio=3), \"sysprog\"(prio=2)\n");
	prio_q_enqueue(q, create_int(13), 1);
	prio_q_enqueue(q, create_char('a'), 5);
	prio_q_enqueue(q, create_int(8), 3);
	prio_q_enqueue(q, create_string("sysprog"), 2);

	prio_q_print(q, &print_data_void);

	printf("-> enqueuing: \"last\"(prio=0)\n");
	prio_q_enqueue(q, create_string("last"), 0);
	prio_q_print(q, &print_data_void);

	for(int i = 0; i < 6; i++){
		printf("-> dequeuing: ");
		struct arb_data * d = prio_q_dequeue(q);
		if(d != NULL) {
			print_data(d);
			free_data(d);
		}
		else{
			printf("NULL");
		}
		printf("\n");
		prio_q_print(q, &print_data_void);
	}
	printf("-> enqueuing: 42(prio=1)\n");
	prio_q_enqueue(q, create_int(42), 1);
	prio_q_print(q, &print_data_void);
	printf("-> front: ");
	print_data_void(prio_q_front(q));
	printf("\n");

	// free q and all of the data
	struct arb_data * data_array[128];
	int size = prio_q_destroy(q, (void**)data_array);
	printf("=> list of unprocessed queues:\n");
	for(int i = 0; i < size; i++)
	{
		print_data(data_array[i]);
		printf("\n");
		free_data(data_array[i]);
	}

	return 0;
}
