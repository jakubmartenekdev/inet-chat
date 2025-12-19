#ifndef CLIENT_H
#define CLIENT_H

#include <netdb.h>
#include <termios.h>
#include <sys/ioctl.h>

#define KEY_ENTER     10
#define KEY_BACKSPACE 127
#define CTRL_KEY(k) ((k) & 0x1F)

#define BUFF_CAPACITY 256

struct append_buffer {
    char* buf;
    size_t len;
    size_t capacity;
};

typedef struct {
    int sockfd;
    char buf[100];
    struct addrinfo hints;
    struct addrinfo* res;
    struct sockaddr_in* servaddr;
} net_info;

typedef struct {
    struct termios orig_termios;
    struct winsize win;
    char input[100];
    int len;
    int cols;
    int rows;
} term_config;

extern net_info net;
extern struct append_buffer g_append_buffer;
extern term_config g_term_config;

void clear(void);
void init_buffer(struct append_buffer* abuff);
void append_to_buffer(struct append_buffer* abuff, char* str);
void free_abuff(struct append_buffer* abuff);
void window_resize(int sig);
void init_term(void);
void enable_raw_mode(void);
void revert_canon(void);
void handle_input(void);
void draw(void);
void print_bar(void);
void cleanup(void);
void recv_serv_msg(void);
int main(int argc, char** argv);

#endif
