#include "task.h"
#include "pmm.h"
#include "screen.h"
#define MAX_TASK 3
pcb_t tasks[MAX_TASK];
int current_task = 0;
int task_count = 0;

void init_multitasking() {
    for (int i=0; i<MAX_TASK; i++) tasks[i].active = 0;

    tasks[0].id = 0;
    tasks[0].active = 1;
    task_count = 1;
    current_task = 0;
}

unsigned int schedule(unsigned int current_esp) {
    if (task_count <= 1) return current_esp;
    tasks[current_task].esp = current_esp;

    do {
        current_task++;
        if (current_task >= MAX_TASK) current_task = 0;
    } while (tasks[current_task].active == 0);

    return tasks[current_task].esp;
}

void create_task(void (*task_code)()) {
    if (task_count >= MAX_TASK) {
        print("Error: Max of tasks reached.\n");
        return;
    }
    unsigned int* stack = (unsigned int*)((char*)pmm_alloc_frame() + 4096);

    *(--stack) = 0x202;
    *(--stack) = 0x08;
    *(--stack) = (unsigned int)task_code;

    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;
    *(--stack) = 0;

    int id = task_count;
    tasks[id].id = id;
    tasks[id].esp = (unsigned int)stack;
    tasks[id].active = 1;
    
    task_count++;
}
