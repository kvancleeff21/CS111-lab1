#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s program1 [program2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_programs = argc - 1;
    int pipes[num_programs - 1][2];

    // Create pipes
    for (int i = 0; i < num_programs - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_programs; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {
            // Child process
            if (i > 0) {
                // Redirect input from the previous pipe
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
            }

            if (i < num_programs - 1) {
                // Redirect output to the current pipe
                dup2(pipes[i][1], 1);
                close(pipes[i][1]);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < num_programs - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Attempt to execute the program
            if (execlp(argv[i + 1], argv[i + 1], (char *)NULL) == -1) {
                fprintf(stderr, "Error: Failed to execute '%s'. Error code: %d\n", argv[i + 1], errno);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Close all pipe file descriptors in the parent process
    for (int i = 0; i < num_programs - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < num_programs; i++) {
        wait(NULL);
    }

    return 0;
}
