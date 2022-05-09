//
// Created by Vitor Hugo on 09/05/2022.
//


#include "queue.h"

void enqueue (QUEUE *queue, NODE *item) {
    if (queue == NULL) {
        perror("queue: ");
       return;
    }
    if (item == NULL) {
        perror("item: ");
        return;
    }


    if (is_queue_empty(queue)) {
        queue->first = item;
        queue->size ++;
    }
    else {
        NODE *iter = queue->first;
        while (iter->_next != NULL) iter = iter->_next;
        iter->_next = item;
        queue->size ++;
    }

}

NODE* dequeue (QUEUE *queue) {
    if (queue == NULL) {
        perror("queue: ");
        return NULL;
    }
    if (is_queue_empty(queue)) return NULL;
    NODE *item = queue->first;
    queue->first = item->_next;
    return item;
}

int get_queue_size (QUEUE *queue) {
    return queue->size;
}

int is_queue_empty (QUEUE *queue) {
    return queue->size < 1 ? 1 : 0;
}

NODE* peek (QUEUE* queue) {
    if (queue == NULL) {
        perror("queue: ");
        return NULL;
    }
    if (is_queue_empty(queue)) return NULL;
    return queue->first;
}