#include "queue.h"

Queue *queue_create() {
    Queue *q = malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    q->length = 0;
    return q;
}

void queue_destroy(Queue *q) {
    if (!q) return;
    QueueNode *curr = q->front;
    while(curr != NULL) {
        QueueNode *next = curr->next;
        free(curr);
        if (next == NULL) {
            break;
        }
        else {
            curr = next;
            free(next);
        }
    }
    free(q);
}

int queue_add(Queue *q, void *data) {
    if (!q) return -1;
    QueueNode *node = malloc(sizeof(QueueNode));
    node->data = data;
    node->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->length++;
    return 0;
}

void *queue_remove(Queue *q) {
    if (!q || !q->front) { return NULL; }
    QueueNode *node = q->front;
    void *data = node->data;
    q->front = node->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(node);
    q->length--;
    return data;
}

void **queue_get_array(Queue *q) {
    if (!q || q->length == 0) { return NULL; }
    void **elements = malloc(q->length * sizeof(void *));
    QueueNode *current = q->front;
    int i = 0;
    while (current != NULL && i < q->length) {
        elements[i] = current->data;
        current = current->next;
        i++;
    }
    return elements;
}
