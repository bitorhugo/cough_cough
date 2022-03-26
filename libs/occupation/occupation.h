//
// Created by Vitor Hugo on 21/03/2022.
//
#include "parser/parser.h"


#ifndef COUGH_COUGH_OCCUPATION_H
#define COUGH_COUGH_OCCUPATION_H

#define NUM_ROOMS 4
#define PIPE_SZ 4096

void occupation(DATASET data, int timestamp_number, int fd, pid_t pid);

ssize_t n_years_dataset (DATASET data);

void write_to_fd (int id, uint32_t timestamp, const char* room,
                  int occupation, int fd, pid_t pid);

ssize_t readn(int fd, void *ptr, size_t n);

ssize_t writen(int fd, const void *ptr, size_t n);

#endif //COUGH_COUGH_OCCUPATION_H
