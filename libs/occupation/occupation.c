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

void write_to_fd (int id, uint32_t timestamp, const char* room, int occupation,
                  int fd, pid_t pid) {

    char buffer[256];

    // put data into buffer
    sprintf(buffer, "PID:%d;id:%d;timestamp:%d;room:%s;occupation:%d\n",
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

