//
// Created by Vitor Hugo on 24/05/2022.
//

#include <pthread.h>
#include "map_reduce.h"

semaphore_t *e;
semaphore_t *f;

SHARED_DT_HASH* buffers;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int use = 0;
int fill = 0;

/**
 * consumer will calculate occupation of admission in buffers[id][use]
 * @param args
 * @return NULL
 */
void* hash_consumer(void *args) {



    return NULL;
}

/**
 * producers will compute hash and write admission to buffer with id == hash
 * @param args
 * @return
 */
void* hash_producer (void *args) {

    THREAD_DATA *td = (THREAD_DATA*) args;

    for (size_t i = 0; i < td->data->num_lines; i += td->jump_count) {
        uint32_t admission = td->data->lines[i].areas_timestamps[0];
        int id = hash (admission, td->n_consumers); // compute hash
        semaphore_wait(e[id]);
        pthread_mutex_lock(&mutex);
        buffers[id].admissions[fill] = admission; // send admission to buffer
        fill = (fill + 1) % MAX_DT_SZ; // update fill var
        pthread_mutex_unlock(&mutex);
        semaphore_signal(f[id]);
    }
    return NULL;
}

void map_reduce (int N_producers, int N_consumers, DATASET *data,
                 int fd_out) {

    THREAD_DATA td_arr [N_producers];
    pthread_t producer_threads[N_producers];
    pthread_t consumer_threads[N_consumers];

    // set buffers
    buffers = calloc(N_consumers, sizeof(SHARED_DT_HASH));

    // create N empty and N full semaphores
    launch_semaphores(N_consumers);

    // launch producer threads
    for (int i = 0; i < N_producers; i++) {
        td_arr[i].starting_point = i;
        td_arr[i].data = data;
        td_arr[i].jump_count = N_producers;
        td_arr[i].fd_out = fd_out;
        td_arr[i].n_consumers = N_consumers;

        int thread_check = pthread_create(&producer_threads[i],
                                          NULL,
                                          hash_producer,
                                          (void*)&td_arr[i]);
        if (thread_check != 0) {
            perror("thread: ");
            exit(EXIT_FAILURE);
        }
    }

    // launch consumer threads
    for (int i = 0; i < N_consumers; i++) {
        int thread_check = pthread_create(&consumer_threads[i],
                                          NULL,
                                          hash_consumer,
                                          NULL);
        if (thread_check != 0) {
            perror("thread: ");
            exit(EXIT_FAILURE);
        }
    }

    // join producers
    for (int i = 0; i < N_producers; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    // join consumers
    for (int i = 0; i < N_consumers; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    // deallocate mem for semaphores and buffers
    free_map_reduce();
}

int hash (const uint32_t admission, int N) {
    return (int)admission % N;
}

void launch_semaphores (int N) {
    e = calloc(N, sizeof(semaphore_t));
    f = calloc(N, sizeof(semaphore_t));

    for (int i = 0; i < N; i++) {
        kern_return_t ret_empty = semaphore_create(mach_task_self(),
                                                   &e[i],
                                                   SYNC_POLICY_FIFO,
                                                   MAX_DT_SZ);
        kern_return_t ret_full = semaphore_create(mach_task_self(),
                                                   &f[i],
                                                   SYNC_POLICY_FIFO,
                                                   0);
        // check for errors
        if (ret_empty || ret_full != KERN_SUCCESS) {
            perror("semaphore: ");
            exit(EXIT_FAILURE);
        }
    }
}

void free_map_reduce () {
    free (e);
    free (f);
    free (buffers);
}