#include <stdio.h>
#include <stdlib.h>

typedef struct stack_ll {
    int value;
    struct stack_ll* next;
} stack_ll;

stack_ll* head = NULL;

void push(int value) {
    //if (head == NULL) {
    //  perror("push");
    // exit(1);
    //}   
    stack_ll* new_node = malloc(sizeof(stack_ll));
    new_node->value = value;
    new_node->next = head;

    head = new_node;
}

int pop() {
    stack_ll* p_current = head;
    int res = head->value;
    head = head->next;
    free(p_current);
    return res;
}

int main() {

    return 0;
}

