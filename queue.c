#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct queue_ll{
    int value;
    struct queue_ll* next;
} queue_ll;

queue_ll* head = NULL;

void enqueue(int value) {
    queue_ll* current = head;
    queue_ll* new = malloc(sizeof(queue_ll));
    new->value = value;
    new->next = NULL;
    if (current != NULL) {
        while(current->next != NULL) {
            current = current->next;
        }

        current->next = new;
    } else {
        head = new;
    }

}

int dequeue() {
    if (head == NULL) {
        write(STDERR_FILENO, "dequeue", 7);
        return -1;
    }
    queue_ll* tmp = head;
    int res = tmp->value;
    if (head->next == NULL) {
        head = NULL;
        free(tmp);
    } else {
        head = head->next;
        free(tmp);
    }
    return res;
}

int main() {
    enqueue(1);
    enqueue(2);
    enqueue(3);
    enqueue(5);

    for (int i = 0; i < 4; i++) {
        printf("%d\n", dequeue());
    }
    
    return 0;
}
