#include <criterion/criterion.h>
#include "../../ll_queue.h"

void setup_queue() {}
void teardown_queue() {}
TestSuite(ll_queue, .init = setup_queue, .fini = teardown_queue);

Test(queue, is_empty_after_creation) {
    ll_queue* queue = malloc(sizeof(ll_queue));
    make_empty(queue);
    cr_assert(is_empty(queue));
}

Test(misc, passing) {
    cr_assert(1);
}
