#include "client.h"

#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

net_info net;
struct append_buffer g_append_buffer;
term_config g_term_config;

void clear() {
    write(STDOUT_FILENO, "\x1b[H", 3);
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

void init_buffer(struct append_buffer* abuff) {
    abuff->arr = malloc(BUFF_CAPACITY * sizeof(*abuff->arr));
    abuff->len = 0;
    abuff->capacity=BUFF_CAPACITY ; // initial capacity
}

void append_to_buffer(struct append_buffer* abuff, char* str) {
    if (strlen(str) > BUFSIZE) {
        perror("Appended buffer is too large, skipping");
        return;
    } // TODO: fix
    char* new_str = malloc(strlen(str) + 1);
    strcpy(new_str, str);
    // size_t new_size = abuff->len + strlen(str) + 1;
    abuff->len++;
    if (abuff->len > abuff->capacity) {
        abuff->capacity *= 2;
        // same as sizeof(char) but more generic if I wish to extend this behaviour in the future
        abuff->arr = realloc(abuff->arr, abuff->capacity * sizeof(*abuff->arr)); 
    }

    // abuff->len += sizeof(*abuff->arr) * strlen(str);
    abuff->arr[abuff->len - 1] = new_str;
    // strcat(abuff->arr, str);
}

void free_abuff(struct append_buffer* abuff) {
    for (int i = 0; i < abuff->len; i++) {
        free(abuff->arr[i]);
    }
    free(abuff->arr);
}

void window_resize(int sig) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_term_config.win);
    g_term_config.rows = g_term_config.win.ws_row;
    g_term_config.cols = g_term_config.win.ws_col;
}

void init_term() {
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_term_config.win) == -1){
        perror("ioctl");
        exit(1);
    }
    g_term_config.rows = g_term_config.win.ws_row;
    g_term_config.cols = g_term_config.win.ws_col;

    signal(SIGWINCH, window_resize);
    // printf("width: %d\n", g_term_config.win.ws_col);
}

void enable_raw_mode() {
    // clear();

    struct termios raw_termios;

    tcgetattr(STDIN_FILENO, &g_term_config.orig_termios);
    raw_termios = g_term_config.orig_termios;
    raw_termios.c_lflag &= ~(ICANON | ECHO | ISIG); // Disable read batching, echoing and C-c
    raw_termios.c_iflag &= ~(IXON | IXOFF); // Disable C-a C-q

    raw_termios.c_cc[VMIN] = 0;
    raw_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios);
}

void revert_canon() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_term_config.orig_termios);
}

void handle_input() {
    char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1){
        switch (ch){
            case CTRL_KEY('q'):
                printf("Exiting...\n");
                exit(0);
            case KEY_ENTER:
                // name 'f' 'e' 'r' 'o'
                // input '' '' '' '' 'h' 'e' 'l' 'l' 'o' '\n' '\0' 
                g_term_config.input[g_term_config.len] = '\n';
                g_term_config.input[g_term_config.len + 1] = '\0';

                memmove(g_term_config.input + strlen(net.username),
                        g_term_config.input,
                        strlen(g_term_config.input) + 1);
                memcpy(g_term_config.input, net.username, strlen(net.username));

                append_to_buffer(&g_append_buffer, g_term_config.input);
                write(net.sockfd, g_term_config.input, strlen(net.username) + g_term_config.len + 1);

                bzero(g_term_config.input, g_term_config.len + 1);
                g_term_config.len = 0;
                break;
                // term_cfg         
            case KEY_BACKSPACE:
                g_term_config.len--;
                g_term_config.input[g_term_config.len] = '\n';
                g_term_config.input[g_term_config.len + 1] = '\0';
                break;
            default:
                if (g_term_config.len < BUFSIZE - 1) // leave space for \0
                    g_term_config.input[g_term_config.len++] = ch;
        }
    }
}

void draw() {
    write(STDOUT_FILENO, "\x1b[H", 3);

    if (g_append_buffer.len <= 0) return;
    
    int draw_area = g_term_config.rows - 4;
    int off = 0;
    int j = 0;

    for (int i = 0; i < g_append_buffer.len - off; i++) {
        if (g_append_buffer.len > draw_area && off == 0) {
            off = g_append_buffer.len - draw_area;
            j = 0;
        }
        write(STDOUT_FILENO, g_append_buffer.arr[j + off], strlen(g_append_buffer.arr[j + off]));
        j++;
    }
}

void print_bar() {
    char escape_seq[16];
    char line[g_term_config.cols * 3 + 1];
    int i;

    snprintf(escape_seq, sizeof(escape_seq), "\x1b[%dH", g_term_config.rows - 2);
    write(STDOUT_FILENO, escape_seq, strlen(escape_seq));
    for (i = 0; i < g_term_config.cols; i++) {
        memcpy(line + i * 3, "─", 3); // pointer arithmetic
    }
    memcpy(line + i * 3, "\0", 1);

    write(STDOUT_FILENO, line, strlen(line)); // get number of bytes before \0

    snprintf(escape_seq, sizeof(escape_seq), "\x1b[%dH", g_term_config.rows - 1);
    write(STDOUT_FILENO, escape_seq, strlen(escape_seq));
    write(STDOUT_FILENO, g_term_config.input, g_term_config.len);
}

void cleanup() {
    free_abuff(&g_append_buffer);
    freeaddrinfo(net.res);
    revert_canon();

}

void recv_serv_msg() {
    char buffer[BUFSIZE];
    int n;

    n = read(net.sockfd, buffer, sizeof(buffer));
    if (n > 0) {
        // append buffer accepts strings, thus we need to null terminate read buff
        // TODO: receive raw bytes from server OR with \n at the end at which point I dont need abuff \n below
        buffer[n] = '\0'; 
        append_to_buffer(&g_append_buffer, buffer);
        // append_to_buffer(&g_append_buffer, "\n");
    } else if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
        perror("Reading from server failed");
        exit(1);
    }  

}

void print_usage(char* prog_name) {
    printf("Usage:\n"
            "Syntax is: %s <server-addr> <server-portno> [OPTIONS]\n"
            "    <server-portno> accepts integer value between 1024-49151\n"
            "Options are:\n"
            "    -h (short for --help): show help message\n"
            "    -n (short for --name): sets up displayed username, up to 10 characters\n", prog_name);
}

int main(int argc, char** argv) {
    if (argc < 3)  {
        print_usage(argv[0]);
        exit(1);
    }
    int name_is_set = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
            if (i + 1 > argc) break;
            if (strlen(argv[i + 1]) > 10) {
                print_usage(argv[0]);
                exit(1);
            }
            snprintf(net.username, sizeof(net.username), "[%s] ", argv[i + 1]); 
            name_is_set = 1;
        }
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            exit(1);
        }
    }
    if (!name_is_set) {
        strcpy(net.username, "[Anonymous] ");
    }

    init_term();
    init_buffer(&g_append_buffer);
    net.sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&net.hints, sizeof(net.hints));
    net.hints.ai_family = AF_INET;
    // net.hints.ai_family = AF_UNSPEC;
    net.hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(argv[1], argv[2], &net.hints, &net.res) == -1) {
        perror("getaddrinfo");
        exit(1);
    }

    struct addrinfo* temp = net.res;
    while (temp != NULL) {
        // char str[INET6_ADDRSTRLEN];
        if (temp->ai_addr->sa_family == AF_INET) {
            net.servaddr = (struct sockaddr_in *)temp->ai_addr;
            // printf("%s\n", inet_ntop(AF_INET, &p->sin_addr, str, sizeof(str)));
            // printf("port: %d\n", ntohs(p->sin_port));
        }

        temp = temp->ai_next;
    }
    if (connect(net.sockfd, net.res->ai_addr, net.res->ai_addrlen) == -1) {
        perror("connect");
        exit(1);
    }

    // TODO: study more use cases of fcntl
    int flags = fcntl(net.sockfd, F_GETFL, 0);
    fcntl(net.sockfd, F_SETFL, flags | O_NONBLOCK);
    enable_raw_mode();
    while (1) {
        recv_serv_msg();
        handle_input();
        clear();
        draw();
        print_bar();

        usleep(1000000 / 30); // ~30 FPS
    }
    atexit(cleanup);



    return 0;
}
