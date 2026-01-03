
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void* routine(void* arg) {
    // int n = 
    while (1) {
        sleep(1);
        printf("%d\n", *(int *)arg);
    }
}

void overwrite_stack() {
    char buffer[1024];
    memset(buffer, 0xFF, sizeof(buffer));  // Fill stack with garbage
    // This likely overwrites where n was
}

void breaks() {
    pthread_t t;
    int n = 10;
    pthread_create(&t, NULL, routine, (void*)&n);
    sleep(5);

}

int main() {
    int arr[10];
    arr[0] = 20;
    breaks();
    

    // overwrite_stack();
    sleep(10);

    return 0;
}
