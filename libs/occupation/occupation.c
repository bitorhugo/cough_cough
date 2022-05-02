//
// Created by Vitor Hugo on 21/03/2022.
//

#include "occupation.h"
#include "../io/io.h"

void occupation(const DATASET data, int id, int fd_write, pid_t pid) {

    const char *rooms[NUM_ROOMS] = {"wait_triage", "triage", "wait_doc_app", "doc_app"};
    int occupation [NUM_ROOMS] = {};

    uint32_t timestamp = 0;

    // iterate over rooms
    for (size_t i = 0; i < NUM_ROOMS; i++) {

        // get timestamp
        timestamp = data.lines[id].areas_timestamps[i];

        // set an interval to search for occupation
        uint32_t min_interval = timestamp - ONE_WEEK_UNIX_TS;
        uint32_t max_interval = timestamp + ONE_WEEK_UNIX_TS;

        if (i == 0) {
            // admission column is ordered by default
            // calculate occupation for admission room
            for (ssize_t j = id; j > 0; j--) {
                uint32_t iter = data.lines[j - 1].areas_timestamps[i + 1];
                if (timestamp < iter) {
                    occupation[i] ++;
                }
            }
        }
        else {
            // cycle before current timestamp
            for (ssize_t j = id; j > 0; j--) {
                uint32_t iter_curr = data.lines[j - 1].areas_timestamps[i];
                uint32_t iter_next = data.lines[j - 1].areas_timestamps[i + 1];
                if (timestamp > iter_curr && iter_curr != NOT_VALID_TIMESTAMP) {
                    if (timestamp < iter_next) {
                        occupation[i] ++;
                    }
                }
                if (iter_curr < min_interval){
                    break;
                }
            }
            // cycle after current timestamp
            for (size_t j = id; j < data.num_lines; j++) {
                uint32_t iter_curr = data.lines[j + 1].areas_timestamps[i];
                uint32_t iter_next = data.lines[j + 1].areas_timestamps[i + 1];
                if (timestamp > iter_curr && iter_curr != NOT_VALID_TIMESTAMP) {
                    if (timestamp < iter_next) {
                        occupation[i] ++;
                    }
                }
                if (iter_curr > max_interval){
                    break;
                }
            }

        }
        // write to file
        write_to_fd(id, timestamp, rooms[i], occupation[i], fd_write, pid);
    }

}

void write_to_fd (int id, uint32_t timestamp, const char* room, int occupation,
                  int fd_write, pid_t pid) {

    char buffer[256];

    // put data into buffer
    sprintf(buffer, "%d$%d,%d,%s#%d\n",
            pid,
            id,
            timestamp,
            room,
            occupation);

    // append buffer
    if (writen(fd_write, buffer, strlen(buffer)) != strlen(buffer)) {
        perror("write failed\n");
        exit(1);
    }

}

void check_if_sorted (DATASET *data) {

    for (size_t i = 1; i < data->num_lines; i++) {
        size_t prev = data->lines[i - 1].areas_timestamps[1];
        size_t curr = data->lines[i].areas_timestamps[1];

        if (curr < prev && curr != 9999) {

            exit(23);
        }
        if (i == data->num_lines - 5) {
            printf("uauaua");
        }
    }

}

/*
 * for (size_t j = 0; j < data.num_lines; j++) {
                uint32_t iter_curr = data.lines[j].areas_timestamps[i];
                uint32_t iter_next = data.lines[j].areas_timestamps[i + 1];
                if (timestamp > iter_curr && iter_curr != NOT_VALID_TIMESTAMP) {
                    if (timestamp < iter_next) {
                        occupation[i] ++;
                    }
                }
            }
 */