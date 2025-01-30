#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // argc : Argument Count, ex. argc=3 : ./myprogram arg1 arg2
    // argv : Argument Vector, ex. argv[0]="./myprogram", argv[1]="arg1", argv[2]="arg2"

    // Check if the user has entered a command
    if (argc < 2) {
        printf("Please enter a UNIX command\n");
        return 1; // Indicates failure
    }

    // Create a child process using fork()
    pid_t pid = fork();
    // Error Handling - Check if fork() failed
    if (pid < 0) {
        printf("Error: cannot fork\n");
        exit(1); // Exit with failure status
    }
    // Child Process Execution
    else if (pid == 0) {
        // execvp: int execvp(const char *file, char *const argv[]);
        execvp(argv[1], &argv[1]);
        printf("Error: execution failed\n"); // Only runs if execvp fails
        exit(1); // Exit with failure status
    }
    // Parent Process Handling
    else {
        wait(NULL); // Parent waits until child completes execution
    }

    return 0; // Indicates success
}
