//
// Created by Vitor Hugo on 26/03/2022.
//

#include "C_a.h"

void from_parent_to_file (int fd_out, int *fd_pipe) {

    // read from pipe
    ssize_t bytes_read = 0;
    char buffer [PIPE_SZ];
    memset(buffer, 0, PIPE_SZ);

    while((bytes_read = readn(fd_pipe[READ_END], buffer, sizeof (buffer))) > 0) {
        if (writen(fd_out, buffer, bytes_read) < 0) {
            perror("write failed\n");
            exit(EXIT_FAILURE);
        }
    }
}
