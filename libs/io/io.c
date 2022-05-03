//
// Created by Vitor Hugo on 05/04/2022.
//

#include "io.h"

int flag = 0;

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

void from_parent_to_file (int fd_out, int fd_pipe) {

    // read from pipe
    ssize_t bytes_read = 0;
    char buffer [PIPE_SZ];
    memset(buffer, 0, PIPE_SZ);

    while((bytes_read = readn(fd_pipe, buffer, sizeof (buffer))) > 0) {
        if (writen(fd_out, buffer, bytes_read) < 0) {
            perror("write failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

void from_parent_to_M_processes (int fd_read, size_t M, uint32_t first_ts) {

    // create M pipes
    int pipes[M][2];
    pid_t pid = 0;
    pid_t pids [M];

    // launch M processes
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
            //signal(SIGUSR1, &handler);

            // we need an ID to identify the pipe
            int pipe_id = (int) i;

            // close WR_END of pipe
            (void)close(pipes[pipe_id][WRITE_END]);

            // dup stdin for pipe[READ_END] in process fds
            dup2(pipes[pipe_id][READ_END], STDIN_FILENO);

            execlp( "python3","python3", "../py/hello.py", NULL);
            perror("exec:");
            exit(EXIT_FAILURE);

        }

        // close RD_END of pipe in parent process
        (void) close(pipes[i][READ_END]);
    }

    ssize_t bytes_read;
    size_t buffer_sz = PIPE_SZ;
    char *buffer = calloc(buffer_sz, sizeof(char));

    char delin [] = "\n";
    char *str = NULL;
    int count = 0;

    while (1) {
        // read from pipe_N
        bytes_read = readn(fd_read, buffer, PIPE_SZ);

        if (bytes_read <= 0) break;

        // concatenate not parsed str to beginning of buffer
        if (str != NULL) {
            // reallocate buffer to fit str and paste it to the beginning
            buffer = realloc(buffer, buffer_sz + count);
            memmove(buffer + count, buffer, buffer_sz);
            memcpy(buffer, str, count);
            // update buffer size
            buffer_sz += count;
        }

        // check if able to parse last segment of data
        if (buffer[buffer_sz - 1] != '\n') {
            count = 0;
            for (size_t i = buffer_sz - 1; buffer[i] != '\n'; i--) {
                count ++;
            }
            if (str != NULL) free(str);
            str = calloc(count, sizeof(char));
            memcpy(str,(buffer + buffer_sz - count), count);
            memset(buffer + (buffer_sz - count), 0, count);
        }

        char *token = NULL, *tmp = NULL;
        size_t arr_sz = 11;
        char timestamp [arr_sz], occupation [arr_sz], final_str[arr_sz * 2];
        memset(timestamp, 0, arr_sz);
        memset(occupation, 0, arr_sz);
        memset(final_str, 0, arr_sz * 2);

        token = strtok(buffer, "\n");

        while (token != NULL) {

            // get timestamp and save it on timestamp_value
            tmp = strstr(token, ",");
            memcpy(timestamp, (tmp + 1), arr_sz - 1);
            uint32_t timestamp_value = (uint32_t) strtol(timestamp,
                                                         NULL,
                                                         10);

            // get occupation and save it on occupation_value
            tmp = strstr(token, "#");
            int i;
            for (i = 1; tmp[i] != '\0'; i++) {
                occupation[i - 1] = tmp[i];
            }

            // concatenate timestamp to occupation
            strcpy(final_str, timestamp);
            strcat(final_str, ";");
            strcat(final_str, occupation);
            strcat(final_str, "\n");

            // write to pipe
            int pipe_id = (int)(timestamp_value - first_ts) / ONE_YEAR_UNIX_TS;
            if (writen(pipes[pipe_id][WRITE_END],
                       final_str,
                       strlen(final_str)) < 0) {
                perror("write2: ");
                exit(EXIT_FAILURE);
            }
            memset(final_str, 0, arr_sz * 2);
            memset(occupation, 0, arr_sz);

            // get next token
            token = strtok(NULL, delin);
        }

        // reset buffer
        memset(buffer, 0, buffer_sz);
        buffer_sz = PIPE_SZ;
    }

    free(buffer);
    free(str);

    // when finished send signal to M processes
    for (size_t i = 0; i < M; i++) {
        close(pipes[i][WRITE_END]);
        //kill(pids[i], SIGUSR1);
    }
    wait(NULL);

}

