//
// Created by Vitor Hugo on 17/03/2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef COUGH_COUGH_PARSER_H
#define COUGH_COUGH_PARSER_H


#define NUM_AREAS 5
#define NOT_VALID_TIMESTAMP 9999
#define ONE_YEAR_UNIX_TS 31556926

typedef struct line {
    uint32_t areas_timestamps[NUM_AREAS];
}LINE;

typedef struct dataset {
    LINE *lines;
    uint32_t num_lines;
}DATASET;

/**
 * loads data from filename into memory
 * @param filename file to open
 * @param data dataset for data to be stored
 */
void load_data(const char* filename, DATASET *data);

/**
 * calculates file size: numbers of characters until 'EOF' is reached
 * @param filename file opened
 * @return size of file
 */
int file_sz (const char *filename);

/**
 * parses retrieved data from file into the dataset
 * @param buffer
 * @param data
 */
void parse_buffer (char* buffer, DATASET *data);

/**
 * calculates line size: number of characters until '\n' is reached
 * @param buffer buffer to read
 * @return size of line
 */
int get_line_size (const char* buffer);

/**
 * deallocates supporting data
 * @param arr dt to be freed
 * @param size size of dt
 */
void free_double_buffer (char** arr, int size);


#endif //COUGH_COUGH_PARSER_H
