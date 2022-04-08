//
// Created by Vitor Hugo on 21/03/2022.
//

#include "occupation.h"
#include "../io/io.h"

void occupation(const DATASET data, int id, int fd, pid_t pid) {

    const char *rooms[NUM_ROOMS] = {"wait_triage", "triage", "wait_doc_app", "doc_app"};
    int occupation [NUM_ROOMS] = {};

    uint32_t timestamp = 0;

    // iterate over rooms
    for (size_t i = 0; i < NUM_ROOMS; i++) {

        // get timestamp
        timestamp = data.lines[id].areas_timestamps[i];

        if (i == 0) {
            // calculate occupation for admission room
            for (ssize_t j = id; j >= 0; j--) {
                int timestamp_to_compare = data.lines[j - 1].areas_timestamps[i + 1];
                if (timestamp < timestamp_to_compare) {
                    occupation[i] ++;
                }
            }
        }
        else {
            // cycle current column
            for (size_t j = 0; j < data.num_lines; j++) {
                uint32_t iter_curr = data.lines[j].areas_timestamps[i];
                uint32_t iter_next = data.lines[j].areas_timestamps[i + 1];
                if (timestamp > iter_curr) {
                    if (timestamp < iter_next) {
                        occupation[i] ++;
                    }
                }
            }
        }
        // write to file
        write_to_fd(id, timestamp, rooms[i], occupation[i], fd, pid);
    }

}

void write_to_fd (int id, uint32_t timestamp, const char* room, int occupation,
                  int fd, pid_t pid) {

    char buffer[256];

    // put data into buffer
    sprintf(buffer, "%d$%d,%d,%s#%d\n",
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
