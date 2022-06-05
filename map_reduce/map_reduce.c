//
// Created by Vitor Hugo on 24/05/2022.
//

#include "map_reduce.h"


void map_reduce (int N_producers, int N_consumers, const DATASET *data,
                 int fd_out) {
    // matrix of semaphores
    // for each shared dt we have a two semaphores (empty and full)
    dts = calloc(N_consumers, sizeof(dt));


    // create semaphores for producers and consumers
    for (int i = 0; i < N_consumers; i++) {


    }
}