//
// Created by Vitor Hugo on 25/04/2022.
//

#include "from_N_processes_to_parent_to_M_processes.h"

void from_N_processes_to_parent_to_M_processes (int N_processes,
                                                DATASET data) {
    // open pipe for communication between parent and child processes
    int pipe_N[2];
    if (pipe(pipe_N) < 0) {
        printf("LINE %d: ", __LINE__ - 1);
        perror("piping failed\n");
        exit(EXIT_FAILURE);
    }

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

            for (size_t j = i; j < data.num_lines; j += N_processes) {
                // write to fd_pipe first
                occupation(data, j, pipe_N[WRITE_END], getpid());
            }
            // implicitly close write-end from pipe using exit()
            exit(EXIT_SUCCESS);
        }
    }

    // parent process will only read from pipe -> close write-end from pipe
    if (close(pipe_N[WRITE_END]) < 0) {
        printf("LINE %d: ", __LINE__ - 1);
        perror("close failed\n");
        exit(EXIT_FAILURE);
    }

    from_parent_to_M_processes (pipe_N[READ_END],
                                n_years_dataset(data),
                                first_ts(data));

}