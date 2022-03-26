//
// Created by Vitor Hugo on 17/03/2022.
//

#include "parser.h"

void load_data(const char* filename, DATASET *data) {
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("open file");
        exit(1);
    }

    int sz = file_sz(filename);

    char *buffer = calloc(sz, sizeof(char));
    if (buffer == NULL) {
        perror("calloc failed");
        exit(1);
    }

    if (read(file, buffer, sz + 1) < 0) {
        perror("read failed");
        exit(1);
    }

    if (close(file) < 0) {
        perror("close failed");
        exit(1);
    }

    parse_buffer(buffer, data);
}

int file_sz (const char *filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

void parse_buffer (char* buffer, DATASET *data) {
    if (buffer == NULL) {
        perror("buffer is null");
        exit(1);
    }

    size_t i = 0;
    char** arr = NULL;
    char _1delim [] = "\r\n";
    char* token = strtok(buffer, _1delim);

    // parse on '\r\n'
    while (token != NULL) {
        arr = realloc(arr, sizeof (char*) * (i + 1));
        arr[i] = calloc(get_line_size(token), sizeof(char));
        strcpy(arr[i], token);
        token = strtok(NULL, _1delim);
        i++;
    }

    // free memory from first buffer .
    free(buffer);

    char _2delim [] = ";";
    data->num_lines = i;
    data->lines = calloc(data->num_lines, sizeof(LINE));

    // parse on ';'
    for (i = 0; i < data->num_lines; i++) {
        // iterate over rooms array
        data->lines[i].areas_timestamps[0] = atoi(strtok(arr[i], _2delim));
        for (size_t j = 1; j < NUM_AREAS; j++) {
            data->lines[i].areas_timestamps[j] = atoi(strtok(NULL, _2delim));
        }
    }

    // free memory from second buffer ..
    free_double_buffer (arr, data->num_lines);
}

int get_line_size (const char* buffer) {
    int i;
    for (i = 0; buffer[i] != '\0'; i++);
    return i + 1; // take account the '\0' at the end
}

void free_double_buffer (char** arr, int size) {
    for (int i = 0; i < size; i++) {
        free(arr[i]);
    }
    free(arr);
}
