//
// Created by Vitor Hugo on 25/04/2022.
//

#include "from_socket_to_file.h"

void from_parent_socket_to_file (int fd_out, int N_processes,
                          DATASET data) {

    // open pipe for communication between parent and child processes
    int pipe_N[2];
    if (pipe(pipe_N) < 0) {
        printf("LINE %d: ", __LINE__ - 1);
        perror("piping failed\n");
        exit(EXIT_FAILURE);
    }

    // create server socket
    int listenfd = create_socket_server("/tmp/socket");

    // array of children pid(s)
    int children_pids [N_processes];

    // run processes for occupation calculation
    pid_t pid;
    for (size_t i = 0; i < N_processes; i++) {
        if ((pid = fork()) < 0) {
            printf("LINE %d: ", __LINE__ - 1);
            perror("fork failed\n");
            exit(EXIT_FAILURE);
        }

        // save child pid
        children_pids[i] = pid;

        // child code
        if (pid == 0) {
            // child process will only write to pipe -> close read-end from pipe
            if (close(pipe_N[READ_END]) < 0) {
                printf("LINE %d: ", __LINE__ - 1);
                perror("close failed\n");
                exit(EXIT_FAILURE);
            }

            // create a client socket
            int client_socket = create_socket_client("/tmp/socket");

            for (size_t j = i; j < data.num_lines; j += N_processes) {
                // write to socket
                occupation(data, j, client_socket, getpid());

            }
            // implicitly close write-end from pipe using exit()
            exit(EXIT_SUCCESS);
        }
    }

    from_socket_to_file(listenfd, fd_out);
}
