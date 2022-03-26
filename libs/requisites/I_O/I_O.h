//
// Created by Vitor Hugo on 26/03/2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifndef COUGH_COUGH_I_O_H
#define COUGH_COUGH_I_O_H

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

#endif //COUGH_COUGH_I_O_H
