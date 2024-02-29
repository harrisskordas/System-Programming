#include <stdio.h>   // from www.mario-konrad.ch
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "prod_cons.h"

struct pool_t {
    int * data;
    int start;
    int end;
    int count;
} queue;

static pthread_mutex_t mtx;
static pthread_cond_t cond_nonempty;
static pthread_cond_t cond_nonfull;
static int POOL_SIZE;

void pool_initialize(pool_t * pool, int pool_size) {
    if (pool == NULL) {
        pool = &queue;
    }

    pool->start = 0;
    pool->end = -1;
    pool->count = 0;
    pool->data = (int*) malloc(sizeof(int)*pool_size);
    
    POOL_SIZE = pool_size;

    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);
}

void pool_destroy(pool_t * pool) {
    if (pool == NULL) {
        pool = &queue;
    }

    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_mutex_destroy(&mtx);
    
    free(pool->data);
}

void pool_place(pool_t * pool, int data) {
    if (pool == NULL) {
        pool = &queue;
    }

    pthread_mutex_lock(&mtx);

    while (pool->count >= POOL_SIZE) {
        pthread_cond_wait(&cond_nonfull, &mtx);
    }
    pool->end = (pool->end + 1) % POOL_SIZE;
    pool->data[pool->end] = data;
    pool->count++;
    pthread_mutex_unlock(&mtx);

    pthread_cond_signal(&cond_nonempty);
}

int pool_obtain(pool_t * pool) {
    if (pool == NULL) {
        pool = &queue;
    }

    int data = 0;

    pthread_mutex_lock(&mtx);
    while (pool->count <= 0) {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }
    data = pool->data[pool->start];
    pool->start = (pool->start + 1) % POOL_SIZE;
    pool->count--;
    pthread_mutex_unlock(&mtx);

    pthread_cond_signal(&cond_nonfull);

    return data;
}
