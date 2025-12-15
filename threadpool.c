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
pthread_cond_t queue_cond;
task* tasks[256];
int head_idx;


void enqueue_task(task* task) {
    pthread_mutex_lock(&queue_mutex);
    tasks[head_idx] = task;
    head_idx++;
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond);
}

void* spawn_worker(void*) {
    task* task;
    while (1) {
        printf("About to lock in\n");
        pthread_mutex_lock(&queue_mutex);
        while (head_idx == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        printf("Awaken from dark slumber\n");
        task = tasks[0];
        for (int i = 0; i < head_idx - 1; i++)
            tasks[i] = tasks[i + 1];
        head_idx--;
        pthread_mutex_unlock(&queue_mutex);
        task->execute();
    }
}

int main() {
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
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
    pthread_cond_destroy(&queue_cond);
    return 0;
}
