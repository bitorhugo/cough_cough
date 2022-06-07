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
    uint32_t admissions_line[MAX_DT_SZ];
}SHARED_DT_HASH;

typedef struct consumer_hash_data {
    DATASET* data;
    int id;
    int fd_out;
}CONSUMER_HASH_DATA;

/**
 * use of map reduce paradigm
 * @param N_producers number of producer threads
 * @param N_consumers number of reducer threads
 * @param data data to reduce
 * @param fd_out file descriptor to write to
 */
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

/**
 * calculates the occupation given the line number and data
 * @param admission_line line number
 * @param chd data
 */
void occupation_v4 (uint32_t admission_line, const CONSUMER_HASH_DATA *chd);

/**
 * writes to file the reduced solution
 * @param chd data
 * @param admission_line line number
 * @param occupation number of people in the room
 */
void H_write_to_fd (const CONSUMER_HASH_DATA *chd,
                    uint32_t admission_line,
                    int *occupation);

#endif //COUGH_COUGH_MAP_REDUCE_H
