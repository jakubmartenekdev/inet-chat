#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct node node;

typedef struct ll_queue ll_queue;

void make_empty(ll_queue* queue);

bool is_empty(ll_queue* queue);

bool has_single_element(ll_queue* queue);

void enqueue(ll_queue* queue, int value);

int dequeue(ll_queue* queue);

#endif // QUEUE_H
