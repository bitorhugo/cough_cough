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

#define MAX_DT_SZ 128
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

/**
 * producer consumer paradigm
 * @param p_threads number of producer threads
 * @param n_threads number of consumer threads
 * @param data data
 */
void P_threads_N_threads(int p_threads, int n_threads, DATASET data, int);

/**
 * calculates occupation
 * @param td thread data
 * @param line line number
 */
void occupation_v3 (const THREAD_DATA *td, int line);

/**
 * producer threads write to a shared array
 * @param thread_id thread id
 * @param current_ts current unix timestamp
 * @param occupation number of people in the room
 */
void write_to_shared_dt (int thread_id, size_t current_ts, int *occupation);

/**
 * consumer threads write to file
 * @param buffer data
 * @param cd data
 */
void write_to_fd_v3 (char *buffer, const CONSUMER_DATA *cd);

#endif //COUGH_COUGH_P_THREADS_N_THREADS_H
