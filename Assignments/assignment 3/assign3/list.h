/**
 * list data structure containing the tasks in the system
 */

 #ifndef LIST_H
#define LIST_H

#include "task.h"

extern Task *head; // Declare the head pointer


struct node {
    Task *task;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Task *task);
void delete(struct node **head, Task *task);
void traverse(struct node *head);

void add(char *name, int priority, int burst);
#endif 