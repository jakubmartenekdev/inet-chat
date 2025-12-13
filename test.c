#include <stdio.h>
#include <unistd.h>
int main() {
    while(1) {
        fprintf(stdout, "Hello World\n");
        fprintf(stdout, "Hello World");
        fprintf(stdout, "Hello World");
        fprintf(stdout, "Hello World");
        fprintf(stdout, "Hello World");
        fflush(stdout);
        sleep(5);
        break;
    }

    return 0;
}
