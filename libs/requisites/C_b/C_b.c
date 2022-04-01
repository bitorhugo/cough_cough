//
// Created by Vitor Hugo on 26/03/2022.
//

#include "C_b.h"

ssize_t n_years_dataset (DATASET data) {

    // get first timestamp
    size_t first_ts = data.lines->areas_timestamps[0];

    // get last valid timestamp
    size_t last_ts = 0;
    ssize_t i = NUM_ROOMS;
    do {
        last_ts = data.lines[data.num_lines - 1].areas_timestamps[i];
        i--;
    } while (last_ts == NOT_VALID_TIMESTAMP);

    // calculate number of years
    ssize_t years = (last_ts - first_ts) / ONE_YEAR_UNIX_TS;

    // include current year
    return years + 1;
}

void from_parent_to_M_processes (int *fd_pipe, size_t M) {

    // create M pipes
    int pipes[M][2];
    pid_t pid = 0;

    for (size_t i = 0; i < M; i++) {
        //open pipe for each child process
        if((pipe(pipes[i])) < 0) {
            perror("pipe failed\n");
            exit(EXIT_FAILURE);
        }

        // create child
        if ((pid = fork()) < 0) {
            perror("fork failed\n");
            exit(EXIT_FAILURE);
        }

        // child code
        if (pid == 0) {
            // listens for signal
            signal(SIGUSR1, &handler);

            // we need an ID to identify the pipe
            int pipe_id = (int) i;

            // waits for parent to write to pipe
            char parsed_data [PIPE_SZ];
            memset(parsed_data, 0, PIPE_SZ);
            int bytes_read = 0;
            do {
                bytes_read = readn(pipes[pipe_id][READ_END],
                                   parsed_data,
                                   PIPE_SZ);
                if (bytes_read < 0) {
                    perror("read: \n");
                    exit(EXIT_FAILURE);
                }
            }while (bytes_read >= 0);

            exit(EXIT_SUCCESS);
        }
    }

    // read from pipe
    ssize_t bytes_read = 0;
    char buffer [PIPE_SZ];
    memset(buffer, 0, PIPE_SZ);
    char delin [] = "\n";

    while ((bytes_read = readn(fd_pipe[READ_END],
                              buffer,
                              sizeof (buffer))) > 0) {

        // first denotes the first occurrence of 'timestamp:' in token
        char *token = NULL, *first = NULL;
        //timestamp denotes the actual timestamp we want to retrieve
        size_t str_size = strlen("timestamp");
        char timestamp [str_size + 1];
        size_t ts_char_count = 10;

        /*
         * In order to calculate the difference in years between the timestamps
         * given, we need to retrieve the first year of the dataset
         */
        uint32_t first_year = 0;
        token = strtok(buffer, "\n");

        for (ssize_t i = 0; i <= bytes_read; i++) {
            first = strstr(token, "timestamp:");
            memcpy(timestamp, (first + str_size), ts_char_count);
            uint32_t timestamp_value = (uint32_t) strtol(timestamp,
                                                NULL,
                                                10);

            if (i == 0) {
                first_year = timestamp_value;
            }

            // write to pipe
            size_t pipe_id = (timestamp_value - first_year) / ONE_YEAR_UNIX_TS;
            if (writen(pipes[pipe_id][WRITE_END],
                       token,
                       strlen(token)) < 0) {
                perror("write: ");
                exit(EXIT_FAILURE);
            }

            // get next token
            token = strtok(NULL, delin);
        }
    }

}

void handler () {

}