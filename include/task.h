#ifndef TASK_H
#define TASK_H

typedef struct {
    unsigned int esp;
    int id;
    int active;
} pcb_t;

void init_multitasking();
void create_task(void (*task_code)());
unsigned int schedule(unsigned int current_esp);

#endif