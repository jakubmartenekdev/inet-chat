#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // POSIX not IPC V
#include <unistd.h>

#define N_THREADS 4


sem_t semaphore;
pthread_mutex_t mutex;
// pthread_mutex_t mutex;

void* routine(void* args) {
  //sem_wait(&semaphore);
  pthread_mutex_lock(&mutex);
  // critical section
  sleep(1);
  printf("Hello from routine\n");
  // end of critical section
  //sem_post(&semaphore);
  pthread_mutex_unlock(&mutex);
}

int main() {
  pthread_t routines[N_THREADS];

  if (sem_init(&semaphore, 0, 1) == -1) {
    perror("semaphore initialization failed!");
    exit(EXIT_FAILURE);
  };

  for (int i = 0; i < N_THREADS; i++) {
    pthread_create(&routines[i], NULL, routine, NULL);
  }


  for (int i = 0; i < N_THREADS; i++) {
    pthread_join(routines[i], NULL);
  }
    
  sem_close(&semaphore);

  return EXIT_SUCCESS;
  
}
