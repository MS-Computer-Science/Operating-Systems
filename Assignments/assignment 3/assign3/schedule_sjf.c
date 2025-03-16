#include <stdio.h>
#include <stdlib.h>
#include "task.h"      // Ensure this is included to access 'head'
#include "list.h"
#include "schedulers.h"

// Declare 'head' to be used within this file
extern Task *head;

// Function to pick the next task based on the shortest job first
Task* pick_next_task() {
    Task *shortest_task = NULL;
    Task *current_task = head;

    while (current_task != NULL) {
        if (shortest_task == NULL || current_task->burst < shortest_task->burst) {
            shortest_task = current_task;
        }
        current_task = current_task->next;
    }

    return shortest_task;
}


void schedule() {
    Task *task = pick_next_task();
    int time = 0;  // Initialize cumulative time

    while (task != NULL) {
        printf("%d\n", time);  // Print the current time
        printf("|  %s\n", task->name);  // Print the task name
        time += task->burst;  // Increment time by the task's burst time

        // Remove the picked task from the list
        Task **indirect = &head;
        while (*indirect != task) {
            indirect = &(*indirect)->next;
        }
        *indirect = task->next;

        task = pick_next_task();  // Pick the next task
    }
    printf("%d\n", time);  // Print the final cumulative time
}
