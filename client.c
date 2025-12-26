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
    abuff->buf = malloc(BUFF_CAPACITY * sizeof(*abuff->buf));
    abuff->len = 0;
    abuff->capacity=BUFF_CAPACITY ; // initial capacity
}

void append_to_buffer(struct append_buffer* abuff, char* str) {
    if (strlen(str) > BUFF_CAPACITY) {
        perror("Appended buffer is too large, skipping");
        return;
    } // TODO: fix
    size_t new_size = abuff->len + strlen(str) + 1;
    if (new_size > abuff->capacity) {
        abuff->capacity *= 2;
        // same as sizeof(char) but more generic if I wish to extend this behaviour in the future
        abuff->buf = realloc(abuff->buf, abuff->capacity * sizeof(*abuff->buf)); 
    }

    abuff->len += sizeof(*abuff->buf) * strlen(str);
    strcat(abuff->buf, str);
}

void free_abuff(struct append_buffer* abuff) {
  free(abuff->buf);
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
        g_term_config.input[g_term_config.len] = '\0';
        append_to_buffer(&g_append_buffer, g_term_config.input);
        append_to_buffer(&g_append_buffer, "\n");

        // send new line character over network instead
        g_term_config.input[g_term_config.len] = '\n'; // TODO: fix on server
        write(net.sockfd, g_term_config.input, g_term_config.len + 1);

        bzero(g_term_config.input, g_term_config.len + 1);
        g_term_config.len = 0;
        break;
        // term_cfg         
      case KEY_BACKSPACE:
        g_term_config.input[--g_term_config.len] = '\0';
        break;
      default:
        if (g_term_config.len < BUFSIZE)
          g_term_config.input[g_term_config.len++] = ch;
    }
  }

}

void draw() {
  
  write(STDOUT_FILENO, "\x1b[H", 3);
  write(STDOUT_FILENO, g_append_buffer.buf, g_append_buffer.len);
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
            "Syntax is: %s [OPTIONS] <server-addr> <server-portno>\n"
            "    <server-portno> accepts integer value between 1024-49151\n"
            "Options are:\n"
            "    --help: display help message"
            "\n", prog_name);
}

int main(int argc, char** argv) {
    // TODO: check every argument
    if (argc != 3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )  {
        print_usage(argv[0]);
        exit(1);
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
