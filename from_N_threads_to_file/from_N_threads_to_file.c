//
// Created by Vitor Hugo on 06/05/2022.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "from_N_threads_to_file.h"
/*
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sig_handler () {
    printf("Admission Count: %zu\n", admission_count);
    // loop the alarm every second
    alarm(1);
}

void *func (void* arg) {
    THREAD_DATA *td = (THREAD_DATA*)arg;

    printf("THREAD: %zu\n", td->starting_point);

    for (size_t i = 0; i < td->data->num_lines; i += td->jump_count) {
        occupation_v2(td, i);
    }

    return NULL;
}

void from_N_threads_to_file (int N_threads, DATASET data, int fd_out) {

    // listen for alarm signal
    signal(SIGALRM, sig_handler);

    THREAD_DATA td_arr [N_threads];
    pthread_t threads[N_threads];

    // launch N threads
    for (size_t i = 0; i < N_threads; i++) {
        td_arr[i].starting_point = i;
        td_arr[i].data = &data;
        td_arr[i].jump_count = N_threads;
        td_arr[i].fd_out = fd_out;
        int thread_check = pthread_create(&threads[i],
                                          NULL,
                                          func,
                                          (void*)&td_arr[i]);
        if (thread_check != 0) {
            perror("thread: ");
            exit(EXIT_FAILURE);
        }
    }

    // set an alarm for 1s
    alarm(1);

    // wait for all threads to finish
    for (size_t i = 0; i < N_threads; i++) {
        pthread_join(threads[i], NULL);
    }

}

void occupation_v2 (const THREAD_DATA *td, int line) {
    if (td == NULL) {
        perror("td: ");
        exit(EXIT_FAILURE);
    }

    int occupation [NUM_ROOMS] = {};

    size_t current_ts = td->data->lines[line].areas_timestamps[0];



    // cycle through timestamps prior to current
    for (size_t i = line; i > 0; i--) {

        for (size_t j = 0; j < NUM_ROOMS; j++) {
            size_t in = td->data->lines[i - 1].areas_timestamps[j];
            size_t out = td->data->lines[i - 1].areas_timestamps[j + 1];
            if (in < current_ts && current_ts < out) {
                occupation[j] ++;
                // a person cannot be in two rooms at the same time
                // once someone is found break the loop
                break;
            }
        }
    }

    write_to_fd_v2 (td->fd_out, td->starting_point, current_ts, occupation);

    // TODO: performance hit by putting threads on wait until mutex is unlocked
    pthread_mutex_lock(&mutex);
    admission_count ++;
    pthread_mutex_unlock(&mutex);
}

void write_to_fd_v2 (int fd_out, int thread_id,
                     size_t current_ts, int *occupation) {
    char buffer[256];

    const char *rooms[NUM_ROOMS] =
            {"wait_triage", "triage", "wait_doc_app", "doc_app"};

    for (int i = 0; i < NUM_ROOMS; i++) {
        sprintf(buffer, "%d$%zu,%s#%d\n",
                thread_id,
                current_ts,
                rooms[i],
                occupation[i]);
        // append buffer
        if (writen(fd_out, buffer, strlen(buffer)) != strlen(buffer)) {
            perror("write failed\n");
            exit(1);
        }
    }

}*/