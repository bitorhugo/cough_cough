//
// Created by Vitor Hugo on 25/04/2022.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <occupation/occupation.h>
#include "io/io.h"
#include "socket/socket.h"

#ifndef COUGH_COUGH_FROM_SOCKET_TO_FILE_H
#define COUGH_COUGH_FROM_SOCKET_TO_FILE_H

void from_parent_socket_to_file (int fd_out, int N_processes,
                          DATASET data);

#endif //COUGH_COUGH_FROM_SOCKET_TO_FILE_H
