#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>

#define KEY_ENTER     10
#define KEY_BACKSPACE 127
#define CTRL_KEY(k) ((k) & 0x1F)

void clear();

// net client
typedef struct {
  int sockfd;
  char buf[100];
  struct addrinfo hints;
  struct addrinfo* res;

} net_info;

  net_info net;

// ---- Append buffer ----
struct append_buffer {
  char* buf;
  int len;
};

struct append_buffer g_append_buffer;

void init_buffer(struct append_buffer* abuff) {
    abuff->buf = NULL;
    abuff->len = 0;
}

/**
* Appends a string to a dynamic string buffer
* @param abuff append buffer
* @param str newly appended string
*/
void append_to_buffer(struct append_buffer* abuff, char* str) {
  // TODO double realloc space and shrink buffer 
  if (abuff->buf == NULL) {
    abuff->buf = malloc(strlen(str) + 1);
    if (abuff->buf == NULL) {
      perror("Allocating space failure");
      exit(1);
    }
    abuff->len = strlen(str);

    strcpy(abuff->buf, str);
  }
  else {
    abuff->len += strlen(str);
    abuff->buf = realloc(abuff->buf, abuff->len + 1);
    strcat(abuff->buf, str);
  }
  
  
}

void free_abuff(struct append_buffer* abuff) {
  free(abuff->buf);
}


// ---- Terminal ----
typedef struct {
  struct termios orig_termios;
  struct winsize win;
  char input[100];
  int len;
  int cols;
  int rows;
} term_config;

term_config g_term_config;

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
  raw_termios.c_lflag &= ~(ICANON | ECHO | ISIG);
  raw_termios.c_iflag &= ~(IXON | IXOFF);

  raw_termios.c_cc[VMIN] = 0;
  raw_termios.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_termios);
}

void revert_canon() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_term_config.orig_termios);
}

void clear() {
  write(STDOUT_FILENO, "\x1b[H", 3);
  write(STDOUT_FILENO, "\x1b[2J", 4);
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

        bzero(g_term_config.input, g_term_config.len);
        g_term_config.len = 0;
        break;
        // term_cfg         
      case KEY_BACKSPACE:
        g_term_config.input[--g_term_config.len] = '\0';
        break;
      default:
        if (g_term_config.len < 100)
          g_term_config.input[g_term_config.len++] = ch;
    }
  }

}

void draw() {
  
  write(STDOUT_FILENO, "\x1b[H", 3);
  write(STDOUT_FILENO, g_append_buffer.buf, g_append_buffer.len);
}

// TODO resize bar with window size
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

int main(int argc, char** argv) {
  if (argc != 3) {
    perror("Usage: <hostname> <port>\n");
    exit(1);
  }
  
  init_term();
  init_buffer(&g_append_buffer);
  net.sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&net.hints, sizeof(net.hints));
  net.hints.ai_family = AF_INET;
  net.hints.ai_socktype = SOCK_STREAM;
  
  if (getaddrinfo(argv[1], NULL, &net.hints, &net.res) == -1) {
    perror("getaddrinfo");
    exit(1);
  }
  // client
  enable_raw_mode();
  while (1) {
    handle_input();
    clear();
    draw();
    print_bar();

  // TODO: handle time delay
    usleep(1000000 / 30); // ~30 FPS
  }
  atexit(cleanup);
  


  return 0;
}
