//
// Created by Vitor Hugo on 09/05/2022.
//
#include <__wctype.h>
#include <queue/queue.h>
#include <pthread.h>
#include "P_threads_N_threads.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int fill = 0;
int use = 0;

void sig_handler () {
    printf("Admission Count: %zu\n", admission_count);
    // loop the alarm every second
    alarm(1);
}

void* producer(void* args) {
    THREAD_DATA *td = (THREAD_DATA*)args;

    printf("THREAD: %zu\n", td->starting_point);

    for (size_t i = 0; i < td->data->num_lines; i += td->jump_count) {
        occupation_v3(td, i);
    }

    return NULL;
}

_Noreturn void* consumer(void *args) {

    size_t fd_out = (size_t)args;

    char buf[128];
    for (int i = 0 ;; i++) {
        semaphore_wait(full);
        pthread_mutex_lock(&mutex);
        strcpy(buf, dt.buffer[use]);
        use = (use + 1) % MAX;
        if (writen(fd_out, buf, strlen(buf)) != strlen(buf)) {
            perror("write failed\n");
            exit(1);
        }
        pthread_mutex_unlock(&mutex);
        semaphore_signal(empty);
    }
}

void P_threads_N_threads(int P_threads, int N_threads, DATASET data,
                         int fd_out) {

    // create semaphores for producers and consumers
    kern_return_t ret_empty = semaphore_create(mach_task_self(),
                           &empty,
                           SYNC_POLICY_FIFO,
                           MAX);

    kern_return_t ret_full = semaphore_create(mach_task_self(),
                                               &full,
                                               SYNC_POLICY_FIFO,
                                               0);

    // check if semaphores were correctly initialized
    if (ret_empty || ret_full != KERN_SUCCESS) {
        perror("semaphore: ");
        exit(EXIT_FAILURE);
    }

    // listen for alarm signal
    signal(SIGALRM, sig_handler);

    THREAD_DATA td_arr [P_threads];
    pthread_t p_threads[P_threads];
    pthread_t n_threads[N_threads];

    // launch P threads
    for (size_t i = 0; i < P_threads; i++) {
        td_arr[i].starting_point = i;
        td_arr[i].data = &data;
        td_arr[i].jump_count = P_threads;
        td_arr[i].fd_out = fd_out;
        int thread_check = pthread_create(&p_threads[i],
                                          NULL,
                                          producer,
                                          (void*)&td_arr[i]);
        if (thread_check != 0) {
            perror("thread: ");
            exit(EXIT_FAILURE);
        }
    }

    // launch N threads
    for (size_t i = 0; i < N_threads; i++) {
        int thread_check = pthread_create(&n_threads[i],
                                          NULL,
                                          consumer,
                                          INT2VOIDP(fd_out));
        if (thread_check != 0) {
            perror("thread: ");
            exit(EXIT_FAILURE);
        }
    }

    // set an alarm for 1s
    alarm(1);

    // wait for P all threads to finish
    for (size_t i = 0; i < P_threads; i++) {
        pthread_join(p_threads[i], NULL);
    }

    // wait for N all threads to finish
    for (size_t i = 0; i < N_threads; i++) {
        pthread_join(n_threads[i], NULL);
    }

}

void occupation_v3 (const THREAD_DATA *td, int line) {
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

    // write to shared data structure
    write_to_shared_dt (td->starting_point, current_ts, occupation);

    /*pthread_mutex_lock(&mutex);
    admission_count ++;
    pthread_mutex_unlock(&mutex);*/
}

void write_to_shared_dt (int thread_id, size_t current_ts, int *occupation) {
    char buffer[256];

    const char *rooms[NUM_ROOMS] = {"wait_triage",
                                    "triage",
                                    "wait_doc_app",
                                    "doc_app"};

    for (int i = 0; i < NUM_ROOMS; i++) {
        sprintf(buffer,
                "%d$%zu,%s#%d\n",
                thread_id,
                current_ts,
                rooms[i],
                occupation[i]);

        semaphore_wait(empty);
        pthread_mutex_lock(&mutex); // obtain mutual exclusion
        strcpy(dt.buffer[fill], buffer);
        fill = (fill + 1) % MAX; // update fill var
        pthread_mutex_unlock(&mutex); // release mutual exclusion
        semaphore_signal(full);
    }

}
