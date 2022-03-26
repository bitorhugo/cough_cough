//
// Created by Vitor Hugo on 26/03/2022.
//

#include "C_b.h"

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

void from_parent_to_M_processes (int fd_out, int *fd_pipe, size_t years) {

}