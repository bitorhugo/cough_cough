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

int create_socket_client(char *socket_path);

int create_socket_server(char *socket_path);

#endif //COUGH_COUGH_SERVER_H
