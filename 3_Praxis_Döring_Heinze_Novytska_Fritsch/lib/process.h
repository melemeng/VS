#ifndef PROCESS_H
#define PROCESS_H

typedef struct _process{
    int start_time;
    int time_left;
    int priority;
    int id;
    int service_time;
}process;

//process* running_process;
//process** arriving_processes;

#endif //PROCESS_H
