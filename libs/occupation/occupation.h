//
// Created by Vitor Hugo on 21/03/2022.
//
#include "parser/parser.h"

#ifndef COUGH_COUGH_OCCUPATION_H
#define COUGH_COUGH_OCCUPATION_H

#define NUM_ROOMS 4

/**
 * calculates occupation
 * @param data data to be used
 * @param id line number
 * @param fd_write file descriptor to write to
 * @param pid pid of process
 */
void occupation(DATASET data, int id, int fd_write, pid_t pid);

/**
 * writes data to file descriptor
 * @param id line number
 * @param timestamp timestamp
 * @param room room which calculation was made
 * @param occupation number of people in the room
 * @param fd file descriptor
 * @param pid process id
 */
void write_to_fd (int id, uint32_t timestamp, const char* room,
                  int occupation, int fd, pid_t pid);

/**
 * checks if data is sorted
 * @param data data to be used
 */
void check_if_sorted (DATASET *data);

#endif //COUGH_COUGH_OCCUPATION_H
