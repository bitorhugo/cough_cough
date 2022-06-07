//
// Created by Vitor Hugo on 02/04/2022.
//

#include "io/io.h"

#include <sys/socket.h>
#include <libproc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef COUGH_COUGH_SERVER_H
#define COUGH_COUGH_SERVER_H

// Size of the listen Queue
#define LISTENQ 5

/**
 * creates a socket client
 * @param socket_path socket path
 * @return descriptor referencing the socket created
 */
int create_socket_client(char *socket_path);

/**
 * creates a socket server
 * @param socket_path socket path
 * @return file descriptor referencing the socket created
 */
int create_socket_server(char *socket_path);

/**
 * writes the content received by the socket to file
 * @param listenfd file descriptor referencing the socket
 * @param fd_out file to write to
 */
void from_socket_to_file (int listenfd, int fd_out);

#endif //COUGH_COUGH_SERVER_H
