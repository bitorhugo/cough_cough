//
// Created by Vitor Hugo on 09/05/2022.
//
#include <parser/parser.h>
#include "io/io.h"
#include "../from_N_threads_to_file/from_N_threads_to_file.h"

#include <mach/semaphore.h>
#include <mach/mach_init.h>
#include <mach/task.h>
#include <mach/mach_error.h>

// avoid warning from casting to voidptr
#define INT2VOIDP(i) (void*)(uintptr_t)(i)

#define MAX_DT_SZ 100
#define ARR_SZ 128

#ifndef COUGH_COUGH_P_THREADS_N_THREADS_H
#define COUGH_COUGH_P_THREADS_N_THREADS_H

typedef struct consumer_data {
    int *fds;
    uint32_t first_ts;
}CONSUMER_DATA;

typedef struct shared_dt {
    char buffer[MAX_DT_SZ][ARR_SZ];
}SHARED_DT;

SHARED_DT dt;

semaphore_t empty, full;

void P_threads_N_threads(int p_threads, int n_threads, DATASET data, int);

void occupation_v3 (const THREAD_DATA *td, int line);

void write_to_shared_dt (int thread_id, size_t current_ts, int *occupation);

#endif //COUGH_COUGH_P_THREADS_N_THREADS_H
