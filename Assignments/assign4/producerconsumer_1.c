#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "buffer.h"

buffer_item buffer[BUFFER_SIZE];
int in = 0, out = 0;
pthread_mutex_t mutex;
sem_t empty, full;

void initialize_buffer() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
}

int insert_item(buffer_item item) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return 0;
}

int remove_item(buffer_item *item) {
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return 0;
}

void *producer(void *param) {
    buffer_item item;
    unsigned int seed = time(NULL) ^ (unsigned int)pthread_self();

    while (1) {
        int sleep_time = (rand_r(&seed) % 3) + 1;
        sleep(sleep_time);
        item = rand_r(&seed);
        if (insert_item(item) != 0) {
            fprintf(stderr, "Producer report error condition\n");
        } else {
            printf("Producer produced %d\n", item);
        }
    }
}

void *consumer(void *param) {
    buffer_item item;
    unsigned int seed = time(NULL) ^ (unsigned int)pthread_self();

    while (1) {
        int sleep_time = (rand_r(&seed) % 3) + 1;
        sleep(sleep_time);
        if (remove_item(&item) != 0) {
            fprintf(stderr, "Consumer report error condition\n");
        } else {
            printf("Consumer consumed %d\n", item);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <sleep time> <producers> <consumers>\n", argv[0]);
        return -1;
    }

    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    initialize_buffer();

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    sleep(sleep_time);

    printf("Main thread exiting...\n");
    return 0;
}
