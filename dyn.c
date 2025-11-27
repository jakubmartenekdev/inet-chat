#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct append_buffer {
  char* buf;
  int len;
};

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

int main() {
  struct append_buffer abuff;
  init_buffer(&abuff);
  append_to_buffer(&abuff, "Hello");
  append_to_buffer(&abuff, " World!");

  printf("%s\n", abuff.buf);
  
  // abuff.buf = malloc(1);
  free_abuff(&abuff);
  

  return 0;
}
