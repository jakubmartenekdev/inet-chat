// better ls :sunglasses:
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int main() {
    // DIR* pdir = opendir(".");
    // struct dirent* stream;
    //
    // while ((stream = readdir(pdir)) != NULL) {
    //     printf("%lu %d %s\n", stream->d_ino, stream->d_type, stream->d_name);
    // }
    //
    // closedir(pdir);    
    //

    // int termfd = open("/dev/tty", O_WRONLY);
    //
    // char* msg = "Hello from user space\n";
    // while (1) {
    //     write(STDOUT_FILENO, msg, strlen(msg));
    //     sleep(1);
    // }
    return EXIT_SUCCESS;
}
