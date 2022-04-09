//
// Created by Vitor Hugo on 05/04/2022.
//

#include "io.h"

ssize_t readn(int fd, void *ptr, size_t n) {
    size_t nleft;
    ssize_t nread;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break; /* error, return amount read so far */
        } else if (nread == 0) {
            break; /* EOF */
        }
        nleft -= nread;
        ptr += nread;
    }
    return(n - nleft); /* return >= 0 */
}

ssize_t writen(int fd, const void *ptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break; /* error, return amount written so far */
        } else if (nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return(n - nleft); /* return >= 0 */
}

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

uint32_t first_ts(DATASET data) {
    return data.lines[0].areas_timestamps[0];
}

void from_parent_to_M_processes (int *fd_pipe, size_t M, uint32_t first_ts) {

    // create M pipes
    int pipes[M][2];
    pid_t pid = 0;
    pid_t pids [M];

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

        // save process pid
        pids[i] = pid;

        // child code
        if (pid == 0) {
            // listens for signal
            signal(SIGUSR1, &handler);

            // we need an ID to identify the pipe
            int pipe_id = (int) i;

            // close WR_END of pipe
            (void)close(pipes[pipe_id][WRITE_END]);

            // waits for parent to write to pipe
            char parsed_data [PIPE_SZ];

            int bytes_read = 0;
            do {
                bytes_read = readn(pipes[pipe_id][READ_END],
                                   parsed_data,
                                   PIPE_SZ);
                if (bytes_read < 0) {
                    perror("read: \n");
                    exit(EXIT_FAILURE);
                }
            }while (bytes_read > 0);
        }

        // close RD_END of pipe in parent process
        (void) close(pipes[i][READ_END]);
    }

    // read from N_processes pipe
    ssize_t bytes_read = 0;
    size_t buffer_sz = PIPE_SZ + 1; // +1 for '\0'
    char *buffer = calloc(buffer_sz, sizeof(char));

    char delin [] = "\n";
    char *str = NULL;
    char *tmp = NULL;
    size_t loop = 0;

    while (1) {
        // read PIPE_SZ bytes into buffer
        bytes_read = readn(fd_pipe[READ_END], buffer, PIPE_SZ);

        if (bytes_read <= 0) break;

        // concatenate not parsed str to beginning of buffer
        if (loop > 0 && str != NULL) {
            if (tmp != NULL) free(tmp);
            tmp = calloc(buffer_sz, sizeof(char));
            strcpy(tmp, buffer);

            // update buffer size to fit the unparsed data
            buffer_sz = PIPE_SZ + strlen(str) + 1; // +1 for '\0'
            buffer = realloc(buffer, buffer_sz);
            memset(buffer, 0, buffer_sz);

            strcpy(buffer, str);
            strcat(buffer, tmp);
        }

        // check if able to parse last segment of data
        if (buffer[buffer_sz - 2] != '\n') {
            int count = 0;
            for (size_t i = buffer_sz - 2; buffer[i] != '\n'; i--) {
                count ++;
            }
            if (str != NULL) free(str);
            str = calloc(count + 1, sizeof(char));
            memcpy(str,(buffer + buffer_sz - 2 - (count - 1)), count);
            memset(buffer + ((buffer_sz - 1) - count), 0, count);
        }
        printf("%s", buffer);
        // 'first' denotes the first occurrence of 'timestamp:' in token
        char *token = NULL, *first = NULL;
        // 'timestamp' denotes the actual timestamp we want to retrieve
        size_t ts_char_count = 10;
        char timestamp [ts_char_count];

        token = strtok(buffer, "\n");

        for (ssize_t i = 0; token != NULL; i++) {
            first = strstr(token, ",");
            memcpy(timestamp, (first + 1), ts_char_count);
            uint32_t timestamp_value = (uint32_t) strtol(timestamp,
                                                         NULL,
                                                         10);

            // write to pipe
            size_t pipe_id = (timestamp_value - first_ts) / ONE_YEAR_UNIX_TS;

            if (writen(pipes[pipe_id][WRITE_END],
                       token,
                       strlen(token)) < 0) {
                perror("write2: ");
                exit(EXIT_FAILURE);
            }

            // get next token
            token = strtok(NULL, delin);
        }

        loop ++;
    }

    free (buffer);
    free(tmp);
    free(str);

    // when finished send signal to M processes
    for (size_t i = 0; i < M; i++) {
        kill(pids[i], SIGUSR1);
    }

}

void handler () {
    //dup2();
    //execvp();
}