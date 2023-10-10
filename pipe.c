#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Invalid argument");
        return EINVAL;
    }

    int num_cmds = argc - 1;
    int pipes[num_cmds - 1][2];

    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe creation failed");
            return errno;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            return errno;
        }

        if (pid == 0) {
            // Child process
            if (i != 0) {
                // Redirect input from the previous pipe
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("Dup2 failed");
                    return errno;
                }
                // Close unused pipe ends
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
            }

            if (i != num_cmds - 1) {
                // Redirect output to the next pipe
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("Dup2 failed");
                    return errno;
                }
                // Close unused pipe ends
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            // Close all pipe ends in the child process
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute the command
            execlp(argv[i + 1], argv[i + 1], NULL);
            perror("Exec failed");
            return errno;
        }
    }

    // Close all pipe ends in the parent process
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }

    return 0;
}
