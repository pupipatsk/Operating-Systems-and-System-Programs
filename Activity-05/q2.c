#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 10

void* print_thread_number(void* arg) {
    int thread_num = *((int*)arg);
    printf("This is thread %d\n", thread_num);
    free(arg);  // Free allocated memory for thread number
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        int* thread_num = malloc(sizeof(int)); // Allocate memory for thread number
        *thread_num = i + 1;
        pthread_create(&threads[i], NULL, print_thread_number, thread_num);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
