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

/**
 * enqueues a node
 * @param queue data structure
 * @param item item to enqueue
 */
void enqueue (QUEUE *queue, NODE *item);

/**
 * dequeues a node from queue
 * @param queue data structure
 * @return dequeued node
 */
NODE* dequeue (QUEUE *queue);

/**
 * retrieves queue size
 * @param queue data structure
 * @return size of queue
 */
int get_queue_size (QUEUE *queue);

/**
 * checks if queue is empty
 * @param queue data structure
 * @return 1 if empty || 0 if not empty
 */
int is_queue_empty (QUEUE *queue);

/**
 * retrieves first node from queue
 * @param queue data structure
 * @return first node
 */
NODE* peek (QUEUE* queue);

#endif //COUGH_COUGH_QUEUE_H
