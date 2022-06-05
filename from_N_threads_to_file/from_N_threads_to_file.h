//
// Created by Vitor Hugo on 06/05/2022.
//

#include <stdint.h>

#include "parser/parser.h"
#include "occupation/occupation.h"
#include "io/io.h"

#ifndef COUGH_COUGH_FROM_N_THREADS_TO_FILE_H
#define COUGH_COUGH_FROM_N_THREADS_TO_FILE_H

size_t admission_count;

typedef struct thread_data {
    DATASET *data;
    size_t starting_point;
    size_t jump_count;
    int fd_out;
    int n_consumers;
}THREAD_DATA;

void from_N_threads_to_file (int N_threads, DATASET data, int  fd_out);

void occupation_v2 (const THREAD_DATA *td, int line);

void write_to_fd_v2 (int fd_out, int pthread_id,
        size_t current_ts, int *occupation);
#endif //COUGH_COUGH_FROM_N_THREADS_TO_FILE_H
