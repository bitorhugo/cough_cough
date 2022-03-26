//
// Created by Vitor Hugo on 26/03/2022.
//
#include "occupation/occupation.h"
#include "requisites/I_O/I_O.h"

#ifndef COUGH_COUGH_C_B_H
#define COUGH_COUGH_C_B_H

/**
 * calculates number of years in Dataset
 * @param data dataset
 * @return number of years
 */
ssize_t n_years_dataset (DATASET data);

void from_parent_to_M_processes (int fd_out, int *fd_pipe, size_t years);

#endif //COUGH_COUGH_C_B_H
