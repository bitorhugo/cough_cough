//
// Created by Vitor Hugo on 24/05/2022.
//
#include <parser/parser.h>
#include <mach/semaphore.h>
#include <mach/mach_init.h>
#include <mach/task.h>
#include <mach/mach_error.h>

#include "../P_threads_N_threads/P_threads_N_threads.h"

#ifndef COUGH_COUGH_MAP_REDUCE_H
#define COUGH_COUGH_MAP_REDUCE_H

typedef struct shared_dt_hash {
    uint32_t admissions[MAX_DT_SZ];
}SHARED_DT_HASH;

void map_reduce (int N_producers,
                 int N_consumers,
                 DATASET *data,
                 int fd_out);

/**
 * computes the hash to know to which buffer to send admission
 * @param admission admission to hash
 * @param N number of buffers
 * @return id of buffer to send admission line
 */
int hash (uint32_t admission, int N);

/**
 * launch semaphores for shared buffers
 * @param N number of buffers
 */
void launch_semaphores (int N);

/**
 * deallocates mem for dynamic dts used
 */
void free_map_reduce ();

#endif //COUGH_COUGH_MAP_REDUCE_H
