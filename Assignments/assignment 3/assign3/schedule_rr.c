#include <stdio.h>
#include <stdlib.h>
#include "task.h"      // Ensure this is included to access 'head'
#include "list.h"
#include "schedulers.h"

// Declare 'head' to be used within this file
extern Task *head;

// Time quantum for Round Robin
#define TIME_QUANTUM 3

void schedule() {
    int time = 0;  // Initialize cumulative time
    Task *current_task = head;

    while (head != NULL) {
        current_task = head;  // Start with the head of the list

        while (current_task != NULL) {
            if (current_task->burst > TIME_QUANTUM) {
                printf("%d\n", time);  // Print the current time
                printf("|  %s\n", current_task->name);  // Print the task name
                time += TIME_QUANTUM;  // Increment time by the time quantum
                current_task->burst -= TIME_QUANTUM;  // Reduce the remaining burst time
            } else {
                printf("%d\n", time);  // Print the current time
                printf("|  %s\n", current_task->name);  // Print the task name
                time += current_task->burst;  // Increment time by the remaining burst time
                current_task->burst = 0;  // Set the burst time to 0

                // Remove the finished task from the list
                Task **indirect = &head;
                while (*indirect != current_task) {
                    indirect = &(*indirect)->next;
                }
                *indirect = current_task->next;
            }

            current_task = current_task->next;  // Move to the next task in the list
        }
    }
    printf("%d\n", time);  // Print the final cumulative time
}
