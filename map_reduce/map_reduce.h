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

SHARED_DT *dts;

void map_reduce (int N_producers,
                 int N_consumers,
                 const DATASET *data,
                 int fd_out);



#endif //COUGH_COUGH_MAP_REDUCE_H
