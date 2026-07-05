//
// Created by Peter on 2026/7/5.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_TIMES   10

int g_buffer[BUFFER_SIZE];
int g_count = 0;
int g_in = 0;
int g_out = 0;

pthread_mutex_t g_mutex;
pthread_cond_t g_notFull;
pthread_cond_t g_notEmpty;

void *producer(void *arg) {
    int i;
    for (i = 0; i < NUM_TIMES; i++) {
        pthread_mutex_lock(&g_mutex);

        while(g_count == BUFFER_SIZE) {
            pthread_cond_wait(&g_notFull, &g_mutex);
        }

        g_buffer[g_in] = i;
        printf("producer: for numer : %d\n", i);
        g_in = (g_in + 1) % BUFFER_SIZE;
        g_count++;

        pthread_cond_signal(&g_notEmpty);
        pthread_mutex_unlock(&g_mutex);

        sleep(1);
    }

    return NULL;
}

void *consumer(void *arg) {
    int item;
    for (int i = 0; i < NUM_TIMES; i++) {
        pthread_mutex_lock(&g_mutex);

        while (g_count == 0) {
            pthread_cond_wait(&g_notEmpty, &g_mutex);
        }

        item = g_buffer[g_out];
        printf("consumer: get number: %d\n", item);
        g_out = (g_out + 1) % BUFFER_SIZE;
        g_count--;

        pthread_cond_signal(&g_notFull);
        pthread_mutex_unlock(&g_mutex);

        sleep(2);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t producerId, consumerId;

    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_notFull, NULL);
    pthread_cond_init(&g_notEmpty, NULL);

    pthread_create(&producerId, NULL, producer, NULL);
    pthread_create(&consumerId, NULL, consumer, NULL);

    pthread_join(producerId, NULL);
    pthread_join(consumerId, NULL);

    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_notFull);
    pthread_cond_destroy(&g_notEmpty);

    return 0;
}