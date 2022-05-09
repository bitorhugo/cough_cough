//
// Created by Vitor Hugo on 09/05/2022.
//

#ifndef COUGH_COUGH_P_THREADS_N_THREADS_H
#define COUGH_COUGH_P_THREADS_N_THREADS_H

#include <parser/parser.h>
#include "../from_N_threads_to_file/from_N_threads_to_file.h"

QUEUE shared_queue = {};

void P_threads_N_threads(int p_threads, int n_threads, DATASET data, int);

void occupation_v3 (const THREAD_DATA *td, int line);

void write_to_shared_queue (int thread_id, size_t current_ts, int *occupation);

#endif //COUGH_COUGH_P_THREADS_N_THREADS_H
