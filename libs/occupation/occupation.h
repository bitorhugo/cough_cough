//
// Created by Vitor Hugo on 21/03/2022.
//
#include "parser/parser.h"
#include "requisites/I_O/I_O.h"

#ifndef COUGH_COUGH_OCCUPATION_H
#define COUGH_COUGH_OCCUPATION_H

#define NUM_ROOMS 4
#define PIPE_SZ 64000

void occupation(DATASET data, int timestamp_number, int fd, pid_t pid);

void write_to_fd (int id, uint32_t timestamp, const char* room,
                  int occupation, int fd, pid_t pid);

#endif //COUGH_COUGH_OCCUPATION_H
