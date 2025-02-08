#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  // For fork()

void* print_message(void* message) {
    printf("%s\n", (char*)message);
    return NULL;
}

int main() {
    pid_t pid = fork();  // Create a child process

    if (pid < 0) {
        // Error handling
        perror("fork failed");
        return 1;
    }

    pthread_t thread1, thread2;

    if (pid > 0) {
        // Parent Process
        pthread_create(&thread1, NULL, print_message, "First thread from parent process");
        pthread_create(&thread2, NULL, print_message, "Second thread from parent process");

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    } else {
        // Child Process
        pthread_create(&thread1, NULL, print_message, "First thread from child process");
        pthread_create(&thread2, NULL, print_message, "Second thread from child process");

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }

    return 0;
}
