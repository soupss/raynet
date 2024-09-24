#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct QueueNode {
    void *data;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *front;
    QueueNode *rear;
    int length;
} Queue;

Queue *queue_create();
int queue_add(Queue *q, void *data);
void *queue_remove(Queue *q);
void **queue_get_array(Queue *q);

#endif
