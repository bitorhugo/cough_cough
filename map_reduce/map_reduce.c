//
// Created by Vitor Hugo on 24/05/2022.
//

#include <pthread.h>
#include "map_reduce.h"

semaphore_t *e = NULL;
semaphore_t *f = NULL;

SHARED_DT_HASH* buffers = NULL;

pthread_mutex_t mutexx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t H_fd_mutex = PTHREAD_MUTEX_INITIALIZER;

int *usee = NULL;
int *filll = NULL;

/**
 * consumer will calculate occupation of admission in buffers[id][use]
 * @param args
 * @return NULL
 */
_Noreturn void* hash_consumer(void *args) {
    CONSUMER_HASH_DATA *chd = (CONSUMER_HASH_DATA*) args; // fd_out and id
    uint32_t admission_line;

    for ( ;; ) {
        semaphore_wait(f[chd->id]);
        pthread_mutex_lock(&mutexx);
        admission_line = buffers[chd->id].admissions_line[usee[chd->id]];
        usee[chd->id] = (usee[chd->id] + 1) % MAX_DT_SZ; // update use variable
        pthread_mutex_unlock(&mutexx);
        semaphore_signal(e[chd->id]);

        occupation_v4(admission_line, chd);
    }

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
        pthread_mutex_lock(&mutexx);
        buffers[id].admissions_line[filll[id]] = i; // send admission_line to
        // buf
        filll[id] = (filll[id] + 1) % MAX_DT_SZ; // update fill var
        pthread_mutex_unlock(&mutexx);
        semaphore_signal(f[id]);
    }
    return NULL;
}

void map_reduce (int N_producers, int N_consumers, DATASET *data,
                 int fd_out) {

    THREAD_DATA td_arr [N_producers];
    pthread_t producer_threads[N_producers];
    pthread_t consumer_threads[N_consumers];
    CONSUMER_HASH_DATA chd[N_consumers];

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
        chd[i].data = data;
        chd[i].fd_out = fd_out;
        chd[i].id = i;
        int thread_check = pthread_create(&consumer_threads[i],
                                          NULL,
                                          hash_consumer,
                                          &chd[i]);
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

    usee = calloc(N, sizeof(int));
    filll = calloc(N, sizeof(int));
}

void occupation_v4 (uint32_t admission_line, const CONSUMER_HASH_DATA *chd) {
    int occupation [NUM_ROOMS] = {};

    size_t current_ts = chd->data->lines[admission_line].areas_timestamps[0];

    // cycle through timestamps prior to current
    for (size_t i = admission_line; i > 0; i--) {
        for (size_t j = 0; j < NUM_ROOMS; j++) {
            size_t in = chd->data->lines[i - 1].areas_timestamps[j];
            size_t out = chd->data->lines[i - 1].areas_timestamps[j + 1];
            if (in < current_ts && current_ts < out) {
                occupation[j] ++;
                // a person cannot be in two rooms at the same time
                // once someone is found break the loop
                break;
            }
        }
    }
    H_write_to_fd(chd, current_ts, occupation);
}

void H_write_to_fd (const CONSUMER_HASH_DATA *chd,
                    uint32_t admission_line,
                    int *occupation) {

    char buffer[128];

    const char *rooms[NUM_ROOMS] = {"wait_triage",
                                    "triage",
                                    "wait_doc_app",
                                    "doc_app"};

    for (int i = 0; i < NUM_ROOMS; i++) {
        sprintf(buffer,
                "$%u,%s#%d\n",
                admission_line,
                rooms[i],
                occupation[i]);

        // write to fd
        pthread_mutex_lock(&H_fd_mutex);
        if (writen(chd->fd_out, buffer, strlen(buffer)) != strlen(buffer)) {
            perror("write failed\n");
            exit(1);
        }
        pthread_mutex_unlock(&H_fd_mutex);
    }


}

void free_map_reduce () {
    free (e);
    free (f);
    free (buffers);
    free(usee);
    free(filll);
}