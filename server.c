#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/signal.h>

#define NCONN 100
#define BUFSIZE 256

#define N_THREADS 8

typedef struct task {
    void (*execute) (int arg); 
    int clisockfd_idx;
} task;

typedef struct message {
    char buffer[BUFSIZE];
    int clisockfd_idx;
} message;

int clisockfd[NCONN];
int pipefd[2];

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;
task* tasks[NCONN]; // queue of tasks
int head_idx;
int bytes_read;

void enqueue_task(task* task) {
    pthread_mutex_lock(&queue_mutex);
    tasks[head_idx] = task;
    head_idx++;
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond);
}

void* spawn_worker(void* args) {
    task* task;
    while (1) {
        // TODO: Add debug log  DEBUG
        // printf("About to lock in\n");
        pthread_mutex_lock(&queue_mutex);
        while (head_idx == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        // printf("Awaken from dark slumber\n");
        task = tasks[0];
        for (int i = 0; i < head_idx - 1; i++)
            tasks[i] = tasks[i + 1];
        head_idx--;
        pthread_mutex_unlock(&queue_mutex);
        task->execute(task->clisockfd_idx);
    }
}

void handle_connection(int clisockfd_idx) {
    char buffer[BUFSIZE];
    message msg;
    // bzero(&msg, sizeof(msg));
    while (1) {
        bzero(buffer,BUFSIZE);
        bytes_read = read(clisockfd[clisockfd_idx], &buffer, BUFSIZE-1);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("Client disconnected (EOF)\n");
            }
            else {
                printf("Read error\n");
            }
            break;
        }
        printf("[SERVER] received message: %s\n",buffer);
        // buffer[bytes_read - 1] = '\0'; // TODO: delete
        memcpy(msg.buffer, buffer, bytes_read);
        // strcpy(msg.buffer, buffer); // TODO: memcpy
        msg.clisockfd_idx = clisockfd_idx;
        write(pipefd[1], &msg, sizeof(msg));
        kill(getpid(), SIGUSR1);
    }
    close(clisockfd[clisockfd_idx]);
    clisockfd[clisockfd_idx] = 0;
}

void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void broadcast_msg(int sig) {
    message msg;
    int n;
    if ((n = read(pipefd[0], &msg, sizeof(msg))) == -1)
        error("reading from pipe");

    for (int i = 0; i < NCONN; i++) {
        if (clisockfd[i] != 0 && clisockfd[i] != clisockfd[msg.clisockfd_idx]) {
            printf("[BROADCAST] Attempting to write to client %d (fd=%d)\n", i, clisockfd[i]);
            int res = write(clisockfd[i], &msg.buffer, bytes_read);
            if (res == -1) error("writing to socket failed");
            printf("[SERVER] sent broadcast message...\n");
        }
    }  
}

void print_usage(char* prog_name) {
    printf("Usage:\n"
            "Syntax is: %s [OPTIONS] <server-portno>\n"
            "    <server-portno> accepts integer value between 1024-49151\n"
            "Options are:\n"
            "    --help: display help message"
            "\n", prog_name);
}

int main(int argc, char *argv[]) {

    if (argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0  ) {
        print_usage(argv[0]);
        exit(1);
    }

    int sockfd, tempsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
    pthread_t threads[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, spawn_worker, NULL);
    }

    if (pipe(pipefd) == -1) {
        error("creating pipe failed");
    }
    signal(SIGUSR1, broadcast_msg);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    bzero(&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, NCONN);
    clilen = sizeof(cli_addr);

    while (1) {

        tempsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);

        printf("client connected\n");
        if (tempsockfd < 0) error("ERROR on accept");
        int i;
        for (i = 0; i < NCONN; i++) {
            if (clisockfd[i] == 0) {
                clisockfd[i] = tempsockfd;
                break;
            }
        }
        task t = {
            .execute = handle_connection,
            .clisockfd_idx = i
        };
        enqueue_task(&t);
    }

    // TODO: Presunut do atexit() a nastavit key handler
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);

    return EXIT_SUCCESS;
}
