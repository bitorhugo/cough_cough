//
// Created by Vitor Hugo on 21/03/2022.
//
#include "parser/parser.h"

#ifndef COUGH_COUGH_OCCUPATION_H
#define COUGH_COUGH_OCCUPATION_H

#define NUM_ROOMS 4
#define PIPE_SZ 4096

void occupation(DATASET data, int timestamp_number, int fd, pid_t pid);

void write_to_fd (int id, uint32_t timestamp, const char* room,
                  int occupation, int fd, pid_t pid);

void check_if_sorted (DATASET *data);

size_t get_interval (DATASET data, uint32_t ts);

#endif //COUGH_COUGH_OCCUPATION_H
