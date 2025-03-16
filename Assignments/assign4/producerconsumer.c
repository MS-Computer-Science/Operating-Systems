#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>  // For sleep function
//#include <windows.h>  // For Sleep function on Windows

#define BUFFER_SIZE 7

typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];
int in = 0; // Index for producer to insert
int out = 0; // Index for consumer to remove

// Semaphores for synchronization
sem_t empty; // Semaphore for empty slots
sem_t full;  // Semaphore for full slots
sem_t mutex; // Mutex for mutual exclusion

void init_buffer() {
    sem_init(&empty, 0, BUFFER_SIZE); // Initially all slots are empty
    sem_init(&full, 0, 0);            // Initially no items in the buffer
    sem_init(&mutex, 0, 1);           // Mutex to ensure mutual exclusion

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0; // Initialize the buffer with 0
    }
}
void* producer(void* param) {
    while (1) {
        // Generate a random number
        buffer_item item = rand() % 100; // Random number between 0 and 99

        // Sleep for a random period
        sleep(rand() % 3);

        sem_wait(&empty); // Wait for an empty slot
        sem_wait(&mutex); // Enter critical section

        buffer[in] = item;  // Insert item into the buffer
        printf("Produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE; // Update producer's index

        sem_post(&mutex); // Exit critical section
        sem_post(&full);  // Signal that a new item is available
    }
    return NULL;
}
void* consumer(void* param) {
    while (1) {
        buffer_item item;

        // Sleep for a random period
        sleep(rand() % 3);

        sem_wait(&full);  // Wait for a full slot
        sem_wait(&mutex); // Enter critical section

        item = buffer[out];  // Remove item from the buffer
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE; // Update consumer's index

        sem_post(&mutex); // Exit critical section
        sem_post(&empty); // Signal that an empty slot is available
    }
    return NULL;
}
int main(int argc, char* argv[]) {
    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    pthread_t producers[num_producers], consumers[num_consumers];

    // Initialize the buffer and semaphores
    init_buffer();

    // Create producer and consumer threads
    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // Sleep for a specified amount of time before exiting
    sleep(sleep_time);

    // Exit the program after the specified time
    printf("Terminating program...\n");

    // Terminate threads (optional cleanup, not always necessary)
    for (int i = 0; i < num_producers; i++) {
        pthread_cancel(producers[i]);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_cancel(consumers[i]);
    }

    return 0;
}
