//
// Created by Vitor Hugo on 05/04/2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "occupation/occupation.h"

#define WRITE_END 1
#define READ_END 0
#define PIPE_SZ 4096

#ifndef COUGH_COUGH_IO_H
#define COUGH_COUGH_IO_H


/**
 * reads from File Descriptor
 * @param fd file descriptor to read from
 * @param ptr variable to read to
 * @param n size in bytes to read
 * @return -1 if error occured | >0 number of bytes read
 */
ssize_t readn(int fd, void *ptr, size_t n);

/**
 *
 * write from File Descriptor
 * @param fd file descriptor to write from
 * @param ptr variable to write to
 * @param n size in bytes to written
 * @return -1 if error occurred | >0 number of bytes written
 */
ssize_t writen(int fd, const void *ptr, size_t n);

/**
 * write from parent process to output file all data
 * @param fd_out file descriptor of file to write to
 * @param fd_pipe file descriptor of pipe of whom to read
 */
void from_parent_to_file (int fd_out, int fd_pipe);

/**
 * calculates number of years in Dataset
 * @param data dataset
 * @return number of years
 */
ssize_t n_years_dataset (DATASET data);

/**
 * retrieves the first timestamp from data
 * @param data data
 * @return first unix timestamp
 */
uint32_t first_ts(DATASET data);

/**
 * sends data from the parent process to M child processes
 * @param fd_read file descriptor to read from
 * @param M number of M processes
 * @param first_ts first timestamp in data
 */
void from_parent_to_M_processes (int fd_read, size_t M, uint32_t first_ts);

/**
 * handler for signal
 * @param SIG signal received
 */
void handler (int SIG);

#endif //COUGH_COUGH_IO_H
