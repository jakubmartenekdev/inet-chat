// O(1) implementation of queue linked list

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
    int value;
    struct node* next;
} node;

typedef struct ll_queue {
    node* head;
    node* rear;
} ll_queue;

void make_empty(ll_queue* queue) {
    queue->head = NULL;
    queue->rear = NULL;
}

bool is_empty(ll_queue* queue) {
    return (queue->head == NULL && queue->rear == NULL) ? true : false;
}

bool has_single_element(ll_queue* queue) {
    return (queue->head == queue->rear) ? true : false;
}

void enqueue(ll_queue* queue, int value) {
    node* new = malloc(sizeof(node));
    new->value = value;
    new->next = NULL;

    if (is_empty(queue)) {
        queue->head = new;
        queue->rear = new;
    } else {
        queue->rear->next = new;
        queue->rear = new;
    }
}

int dequeue(ll_queue* queue) {
    if (is_empty(queue)) return -1;

    int result = queue->head->value;

    if (has_single_element(queue)) {
        node* tmp = queue->head;
        make_empty(queue);
        free(tmp);
    } else {
        node* tmp = queue->head;
        queue->head = queue->head->next;
        free(tmp);
    }
    return result;
}

// TODO: free linked list

// int main() {
//     ll_queue* queue = malloc(sizeof(ll_queue));
//     make_empty(queue);
//
//     enqueue(queue, 1);
//     enqueue(queue, 2);
//     enqueue(queue, 3);
//     enqueue(queue, 8);
//
//     for (int i = 0; i < 4; i++) {
//         printf("%d\n", dequeue(queue));
//     }
//
//
//     return 0;
// }
