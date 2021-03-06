//
// Created by Vitor Hugo on 25/04/2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <occupation/occupation.h>
#include "io/io.h"

#ifndef COUGH_COUGH_FROM_N_PROCESSES_TO_FILE_H
#define COUGH_COUGH_FROM_N_PROCESSES_TO_FILE_H

/**
 * N child processes write to file
 * @param fd_out file descriptor to write to
 * @param N_processes number of child processes
 * @param data data
 */
void from_N_processes_to_file (int fd_out, int N_processes, DATASET data);

#endif //COUGH_COUGH_FROM_N_PROCESSES_TO_FILE_H
