//
// Created by Vitor Hugo on 09/05/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef COUGH_COUGH_QUEUE_H
#define COUGH_COUGH_QUEUE_H

typedef struct node {
    char formatted_data[128];
    struct node *_next;
}NODE;

typedef struct queue {
    NODE *first;
    int size;
}QUEUE;

void enqueue (QUEUE *queue, NODE *item);

NODE* dequeue (QUEUE *queue);

int get_queue_size (QUEUE *queue);

int is_queue_empty (QUEUE *queue);

NODE* peek (QUEUE* queue);


#endif //COUGH_COUGH_QUEUE_H
