#include <criterion/criterion.h>
#include "../../ll_queue.h"

ll_queue* test_queue = NULL;

void setup_queue() {
    test_queue = malloc(sizeof(ll_queue));
    make_empty(test_queue);
}
void teardown_queue() {
    free_queue(test_queue);
}
TestSuite(ll_queue, .init = setup_queue, .fini = teardown_queue);

Test(ll_queue, is_empty_after_creation) {
    cr_assert(is_empty(test_queue));
}

Test(ll_queue, dequeue_last_element) {
    enqueue(test_queue, 1);
    cr_assert_eq(dequeue(test_queue), 1);
    cr_assert(is_empty(test_queue));
}

Test(ll_queue, enqueue_multiple_elements) {
    enqueue(test_queue, 1);
    enqueue(test_queue, 2);
    enqueue(test_queue, 3);
    enqueue(test_queue, 5);
    cr_assert(!is_empty(test_queue));
}

Test(ll_queue, do_not_allow_dequeue_when_queue_is_empty) {
    cr_assert_eq(dequeue(test_queue), -1);
}

