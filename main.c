#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios prev_termios;

void die(const char* s) {
  perror(s);
  exit(1);
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &prev_termios) == -1)
      die("tcsetattr");
}

void enable_raw_mode() {
  
  struct termios raw;
  // uint8_t a;

  if (tcgetattr(STDIN_FILENO, &prev_termios) == -1)
    die("tcgetattr");
  raw = prev_termios;
  raw.c_iflag &= ~(IXON);
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  // raw.c_cc[VMIN] = 0;
  // raw.c_cc[VTIME] = 20;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");

  atexit(disable_raw_mode);
}

char to_ctrl(char c) {
  return c & 0x1f;
}

char editor_read_key() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1) die("read");
  }
  return c;
}
/*** input ***/
void editor_process_keypress() {
  char c = editor_read_key();
  switch (c) {
      case CTRL_KEY('q'):
    // case to_ctrl('q'):
      exit(0);
      break;
  }
}

int main() {
  // printf("\x1b[2J");
  enable_raw_mode();
  
  while (1) {
    editor_process_keypress();
  }

  return 0;
}
