#include <__wctype.h>
//
// Created by Vitor Hugo on 09/05/2022.
//
#include <queue/queue.h>
#include <pthread.h>
#include "P_threads_N_threads.h"

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_fd = PTHREAD_MUTEX_INITIALIZER;

semaphore_t empty, full;

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

__unused _Noreturn void* consumer(void *args) {

    size_t fd_out = (size_t)args;

    char buffer[128];
    for (int i = 0 ;; i++) {
        semaphore_wait(full);
        pthread_mutex_lock(&mutex);
        strcpy(buffer, dt.buffer[use]);
        use = (use + 1) % MAX_DT_SZ;
        if (writen(fd_out, buffer, strlen(buffer)) != strlen(buffer)) {
            perror("write failed\n");
            exit(1);
        }
        pthread_mutex_unlock(&mutex);
        semaphore_signal(empty);
    }
}

_Noreturn void* consumer_years(void *args) {

    CONSUMER_DATA *cd = (CONSUMER_DATA *)args;

    char buffer [128];

    // consume timestamp to buf
    for (int j = 0 ;; j++) {
        semaphore_wait(full);
        pthread_mutex_lock(&mutex);
        strcpy(buffer, dt.buffer[use]);
        use = (use + 1) % MAX_DT_SZ;
        pthread_mutex_unlock(&mutex);
        semaphore_signal(empty);

        write_to_fd_v3 (buffer,cd);
    }
}

void P_threads_N_threads(int P_threads, int N_threads, DATASET data,
                         int fd_out) {

    // create semaphores for producers and consumers
    kern_return_t ret_empty = semaphore_create(mach_task_self(),
                           &empty,
                           SYNC_POLICY_FIFO,
                           MAX_DT_SZ);

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

    // launch producer threads
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

    int num_years = n_years_dataset(data);
    int fds[num_years];
    // open files for writing
    for (int i = 0; i < num_years; i++) {
        char filename[32] = "../data/year";
        sprintf((filename + 12), "%d.txt", i);
        int fd = open(filename,
                          O_WRONLY | O_APPEND | O_TRUNC | O_CREAT,
                          S_IXUSR | S_IWUSR | S_IRUSR);
        if (fd < 0) exit(EXIT_FAILURE);
        fds[i] = fd;
    }

    // create consumer data
    CONSUMER_DATA cd;
    cd.fds = fds;
    cd.first_ts = first_ts(data);

    // launch consumer threads
    for (size_t i = 0; i < N_threads; i++) {
        int thread_check = pthread_create(&n_threads[i],
                                          NULL,
                                          consumer_years,
                                          &cd);
        /*int thread_check = pthread_create(&n_threads[i],
                                          NULL,
                                          consumer_years,
                                          INT2VOIDPTR(fd_out);*/
        if (thread_check != 0) {
            perror("thread: ");
            exit(EXIT_FAILURE);
        }
    }

    // set an alarm for 1s
    alarm(1);

    // wait for all producer threads to finish
    for (size_t i = 0; i < P_threads; i++) {
        pthread_join(p_threads[i], NULL);
    }

    // send signal to finish consumer threads
    // ....

    // wait for all consumer threads to finish
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

    pthread_mutex_lock(&alarm_mutex);
    admission_count ++;
    pthread_mutex_unlock(&alarm_mutex);
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
        fill = (fill + 1) % MAX_DT_SZ; // update fill var
        pthread_mutex_unlock(&mutex); // release mutual exclusion
        semaphore_signal(full);
    }

}

void write_to_fd_v3 (char *buffer, const CONSUMER_DATA *cd) {

    char timestamp[11];

    char *tmp = strstr(buffer, "$");
    memcpy(timestamp, (tmp + 1), 10);
    uint32_t timestamp_value = (uint32_t) strtol(timestamp,
                                                     NULL,
                                                     10);
    if (timestamp_value != NOT_VALID_TIMESTAMP) {
        int year = (int) (timestamp_value - cd->first_ts) / ONE_YEAR_UNIX_TS;

        // write to fd
        pthread_mutex_lock(&mutex_fd);
        if (writen(cd->fds[year], buffer, strlen(buffer)) != strlen(buffer)) {
            perror("write failed\n");
            exit(1);
        }
        pthread_mutex_unlock(&mutex_fd);
    }

}
