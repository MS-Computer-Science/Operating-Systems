#include <stdio.h>
#include <stdlib.h>
#include "task.h"      // Ensure this is included to access 'head'
#include "list.h"
#include "schedulers.h"

// Declare 'head' to be used within this file
extern Task *head;

// Function to pick the next task (FCFS)
Task* pick_next_task() {
    if (head == NULL) {
        return NULL;  // No tasks left
    }
    return head;  // FCFS simply picks the first task
}

// // The scheduling function
// void schedule() {
//     Task *task = pick_next_task();
//     while (task != NULL) {
//         printf("Scheduling task %s with burst %d\n", task->name, task->burst);
//         task = head->next;  // Move to the next task
//         head = head->next;  // Update head to the next task
//     }
// }


void schedule() {
    Task *task = pick_next_task();
    int time = 0;  // Initialize cumulative time

    while (task != NULL) {
        printf("%d\n", time);  // Print the current time
        printf("|  %s\n", task->name);  // Print the task name
        time += task->burst;  // Increment time by the task's burst time
        task = head->next;  // Move to the next task
        head = head->next;  // Update head to the next task
    }
    printf("%d\n", time);  // Print the final cumulative time
}