#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define N_THREADS 4


void execute() {
    printf("Hello am working!\n");
    sleep(1);
}

typedef struct task {
   void (*execute) (); 

} task;

pthread_mutex_t queue_mutex;
task* tasks[256];
int head_idx;


void enqueue_task(task* task) {
    tasks[head_idx] = task;
    head_idx++;
}

void* spawn_worker(void*) {
    while (1) {
        int ready = 0;
        task* task;
        pthread_mutex_lock(&queue_mutex);
        if (head_idx > 0) {
            task = tasks[0];
            for (int i = 0; i < head_idx - 1; i++)
                tasks[i] = tasks[i + 1];
            head_idx--;
            ready = 1;
        } 
        pthread_mutex_unlock(&queue_mutex);
        if (ready) {
            ready = 0;
            task->execute();
        }
    }
}

int main() {
    pthread_t threads[N_THREADS];
        
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, spawn_worker, NULL);
    }

    for (int i = 0; i < 20; i++) {
        task t = {
            .execute = execute     
        };
        enqueue_task(&t);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&queue_mutex);
    return 0;
}
