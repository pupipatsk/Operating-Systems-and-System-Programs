#include<stdio.h>
#include<pthread.h>

void* print_arg(void* data) {
    char* str = (char*)data;
    while (1)
    {
        printf("%s\n", str);
        sleep(1);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        printf("usage: %s arg1 arg2\n", argv[0]);
        return 1;
    }

    pthread_t t1, t2;

    // Create two threads, each printing one argument
    pthread_create(&t1, NULL, print_arg, argv[1]);
    pthread_create(&t2, NULL, print_arg, argv[2]);

    // Wait for both threads to finish execution
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
