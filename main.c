#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "parser/parser.h"
#include "occupation/occupation.h"

// EC 4.d

int main(int argc, char** argv) {

    // argc : 4
    // argv : [
    //  "program_name",
    //  "number of child processes",
    //  "input path",
    //  "output path"
    // ]

    if (argc < 4) {
        perror("INCORRECT NUMBER OF ARG\n");
        exit (EXIT_FAILURE);
    }

    // child processes
    int N_processes = atoi(argv[1]);
    // input data pointer
    char *input_path = argv[2];
    // output data pointer
    char *output_path = argv[3];

    DATASET data = {};

    // load data
    clock_t time;
    time = clock();
    load_data(input_path, &data);
    time = clock() - time;
    printf("load data done in %f(s)\n", ((double)time)/CLOCKS_PER_SEC);

    // get number of years in DATASET
    ssize_t M_processes = n_years_dataset(data);

    // array of children pid(s)
    int pids [N_processes];

    // open pipe for communication between parent and child processes
    // fds[0] -> read-end
    // fds[1] -> write-end
    int fd_pipe[2];
    if (pipe(fd_pipe) < 0) {
        perror("piping failed\n");
        exit(EXIT_FAILURE);
    }

    // run processes for occupation calculation
    pid_t pid = 0;
    for (size_t i = 0; i < N_processes; i++) {
        if ((pid = fork()) < 0) {
            perror("fork failed\n");
            exit(EXIT_FAILURE);
        }

        // save child pid
        pids[i] = pid;

        // child code
        if (pid == 0) {
            // child process will only write to pipe -> close read-end from pipe
            if (close(fd_pipe[0]) < 0) {
                perror("close failed\n");
                exit(EXIT_FAILURE);
            }

            for (size_t j = i; j <= data.num_lines; j += N_processes) {
                occupation(data, j, fd_pipe[1], getpid());
            }
            // implicitly close write-end from pipe using exit()
            exit(EXIT_SUCCESS);
        }
    }

    // parent process will only read from pipe -> close write-end from pipe
    if (close(fd_pipe[1]) < 0) {
        perror("close failed\n");
        exit(EXIT_FAILURE);
    }

    // open file for writing
    int fd_out = open(output_path,
                      O_WRONLY | O_APPEND | O_TRUNC | O_CREAT,
                      S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out < 0) {
        perror("open failed\n");
        exit (EXIT_FAILURE);
    }

    // read from pipe
    ssize_t bytes_read = 0;
    char buffer [PIPE_SZ];
    memset(buffer, 0, PIPE_SZ);

    while((bytes_read = readn(fd_pipe[0], buffer, sizeof (buffer))) > 0) {
        if (writen(fd_out, buffer, bytes_read) < 0) {
            perror("write failed\n");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
