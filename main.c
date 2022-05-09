#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#include "from_N_processes_to_file/from_N_processes_to_file.h"
#include "from_N_processes_to_parent_to_file/from_N_processes_to_parent_to_file.h"
#include "from_N_processes_to_parent_to_M_processes/from_N_processes_to_parent_to_M_processes.h"
#include "from_socket_to_file/from_socket_to_file.h"

#include "from_N_threads_to_file/from_N_threads_to_file.h"

#include "parser/parser.h"
#include "occupation/occupation.h"
#include "io/io.h"
#include "socket/socket.h"



int main(int argc, char** argv) {

    /*
     * argc : 4
     *  argv: ["program_name",
     *         "number of child processes",
     *         "input path",
     *         "output path"]
     */

    if (argc < 4) {
        printf("LINE %d: ", __LINE__ - 1);
        perror("INCORRECT NUMBER OF ARG\n");
        exit (EXIT_FAILURE);
    }

    // child processes / number of threads

    int N = (int)strtol(argv[1], NULL, 10);

    // input data pointer
    char *input_path = argv[2];
    // output data pointer
    char *output_path = argv[3];

    // load data
    DATASET data = {};
    clock_t time;
    time = clock();
    load_data(input_path, &data);
    time = clock() - time;
    printf("load data done in %f(s)\n", ((double)time)/CLOCKS_PER_SEC);

    // open file for writing
    int fd_out = open(output_path,
                      O_WRONLY | O_APPEND | O_TRUNC | O_CREAT,
                      S_IXUSR | S_IWUSR | S_IRUSR);
    if (fd_out < 0) {
        printf("LINE %d: ", __LINE__ - 1);
        perror("open failed\n");
        exit (EXIT_FAILURE);
    }

    //from_N_processes_to_file(fd_out, N, data);
    //from_N_processes_to_parent_to_file(fd_out, N, data);
    //from_N_processes_to_parent_to_M_processes(N, data);
    //from_parent_socket_to_file(fd_out, N, data);

    from_N_threads_to_file(N, data, fd_out);

    exit(EXIT_SUCCESS);
}
