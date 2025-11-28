#include <stdio.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>

#define KEY_ENTER     10
#define KEY_BACKSPACE 127

void clear();

// net client

typedef struct {
  int sockfd;
} net;

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

void init_term() {
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_term_config.win) == -1){
    perror("ioctl");
    exit(1);
  }
  g_term_config.rows = g_term_config.win.ws_row;
  g_term_config.cols = g_term_config.win.ws_col;
  // printf("width: %d\n", g_term_config.win.ws_col);
  // fflush(stdout);
}

void enable_raw_mode() {
  // clear();
  
  struct termios raw_termios;

  tcgetattr(STDIN_FILENO, &g_term_config.orig_termios);
  raw_termios = g_term_config.orig_termios;
  raw_termios.c_lflag &= ~(ICANON | ECHO);
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
      case 'Q': // TODO: C-Q
        printf("Exiting...\n");
        exit(0);
      case KEY_ENTER: // Enter
        g_term_config.input[g_term_config.len] = '\0';
        append_to_buffer(&g_append_buffer, g_term_config.input);
        append_to_buffer(&g_append_buffer, "\n");

        bzero(g_term_config.input, g_term_config.len);
        g_term_config.len = 0;
        break;
        // term_cfg         
      // TODO shift enter S-e
      case KEY_BACKSPACE:
        g_term_config.input[--g_term_config.len] = '\0';
        break;
      default:
        if (g_term_config.len < 100)
          g_term_config.input[g_term_config.len++] = ch;
      // case 'a':
      //   print_buffer();
    }
  }

}

void draw() {
  
  write(STDOUT_FILENO, "\x1b[H", 3);
  write(STDOUT_FILENO, g_append_buffer.buf, g_append_buffer.len);
}

// TODO bar by winsize
void print_bar() {
  char escape_seq[16];
  snprintf(escape_seq, sizeof(escape_seq), "\x1b[%dH", g_term_config.rows - 2);
  write(STDOUT_FILENO, escape_seq, strlen(escape_seq));
  int i;
  // TODO preallocate space for n bars / memcpy
  for (i = 0; i < g_term_config.cols; i++) {
    write(STDOUT_FILENO, "─", 3);
  }
  
  snprintf(escape_seq, sizeof(escape_seq), "\x1b[%dH", g_term_config.rows - 1);
  write(STDOUT_FILENO, escape_seq, strlen(escape_seq));
  write(STDOUT_FILENO, g_term_config.input, g_term_config.len);
  // fflush(stdout);
}

void cleanup() {
  revert_canon();
  free_abuff(&g_append_buffer);

}

int main() {
  init_term();
  init_buffer(&g_append_buffer);
  // client
  enable_raw_mode();
  while (1) {
    handle_input();
    clear();
    draw();
    print_bar();
  }
  atexit(cleanup);
  


  return 0;
}
