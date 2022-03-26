//
// Created by Vitor Hugo on 21/03/2022.
//

#include "occupation.h"

void occupation(const DATASET data, int id, int fd, pid_t pid) {

    const char *rooms[NUM_ROOMS] = {"wait_triage", "triage", "wait_doc_app", "doc_app"};
    int occupation [NUM_ROOMS] = {};

    uint32_t timestamp = 0;

    // iterate over rooms
    for (size_t i = 0; i < NUM_ROOMS; i++) {

        // get timestamp
        timestamp = data.lines[id].areas_timestamps[i];

        // calculate occupation of room
        for (ssize_t j = id; j >= 0; j--) {
            int timestamp_to_compare = data.lines[j - 1].areas_timestamps[i + 1];
            if (timestamp < timestamp_to_compare) {
                occupation[i] ++;
            }
        }
        // write to file
        write_to_fd(id, timestamp, rooms[i], occupation[i], fd, pid);
    }

}

ssize_t n_years_dataset (DATASET data) {

    // get first timestamp
    size_t first_ts = data.lines->areas_timestamps[0];
    size_t last_ts = 0;

    // get last valid timestamp
    for (ssize_t i = data.num_lines - 1; i >= 0; i--) {
        for (ssize_t j = NUM_ROOMS; j >= 0; j--) {
            last_ts = data.lines[i].areas_timestamps[j];
            if (last_ts != NOT_VALID_TIMESTAMP) {
                break;
            }
        }
        if (last_ts != NOT_VALID_TIMESTAMP) {
            break;
        }
    }

    ssize_t years = (last_ts - first_ts) / ONE_YEAR_UNIX_TS;

    // include current year
    return years + 1;
}

void write_to_fd (int id, uint32_t timestamp, const char* room, int occupation,
                  int fd, pid_t pid) {

    char buffer[256];

    // put data into buffer
    sprintf(buffer, "PID:%d;id:%d;timestamp:%d;room:%s;occupation:%d.\n",
            pid,
            id,
            timestamp,
            room,
            occupation);

    // append buffer
    if (writen(fd, buffer, strlen(buffer)) != strlen(buffer)) {
        perror("write failed\n");
        exit(1);
    }

}

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