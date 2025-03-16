#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"

#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3

buffer_item buffer[BUFFER_SIZE];
int in = 0, out = 0;
sem_t empty, full;
pthread_mutex_t mutex;

void *producer(void *param) {
    while (1) {
        sleep(rand() % 3);
        int item = rand() % 100;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("Producer %ld produced %d at %d\n", (long)param, item, in);
        in = (in + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *param) {
    while (1) {
        sleep(rand() % 3);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %ld consumed %d from %d\n", (long)param, item, out);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <sleep_time> <num_producers> <num_consumers>\n", argv[0]);
        return -1;
    }
    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    pthread_t producers[num_producers], consumers[num_consumers];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    for (long i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)i);
    }
    for (long i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void *)i);
    }

    sleep(sleep_time);
    printf("Main thread exiting.\n");
    exit(0);
}