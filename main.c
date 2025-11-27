#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void clear();

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
      case 'Q':
        printf("Exiting...\n");
        exit(0);
      // case 'a':
      //   print_buffer();
    }
  }
  append_to_buffer(&g_append_buffer, (char[]){ch, '\0'});

}

void draw() {
  
  write(STDOUT_FILENO, "\x1b[H", 3);
  write(STDOUT_FILENO, g_append_buffer.buf, g_append_buffer.len);
}

// TODO
void print_bar() {
  write(STDOUT_FILENO, "\x1b[10H", 6);
  write(STDOUT_FILENO, "--------------", 14);
  // fflush(stdout);
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
  // get terminal size
  atexit(revert_canon);
  


  return 0;
}
