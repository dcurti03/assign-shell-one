/*
 * CSCE 30503-0001 Operating Systems
 * Dylan Curtis 11/08/26
 * Week 11-12 Assignment: Shell Assignment Version One
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BSIZE 100

/* Track the currently running child process so SIGINT can interrupt it using kill(). */
pid_t child = 0;

/* Handle SIGINT (Ctrl+C).
 * If an external command is running, send SIGINT to it.
 * The shell itself continues running. */
void handle_sigint(int sig) {
    if (child > 0) {
        kill(child, SIGINT);
    }
}

int main(int argc, char *argv[]) {
    char buffer[BSIZE];
    int status;

    /* Parse commands into a NULL-terminated char *args[] array for execvp(). */
    char *args[20];

    /* Register a signal handler for SIGINT. */
    signal(SIGINT, handle_sigint);

    /* Continuously read input from the user. */
    while (1) {
        printf("shell> ");
        fgets(buffer, BSIZE, stdin);

        /* Remove the newline character */
        buffer[strcspn(buffer, "\n")] = '\0';

        /* Parse command-line arguments into args[]. */
        int i = 0;
        args[i] = strtok(buffer, " ");

        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
        }

        /* Ignore blank lines */
        if (args[0] == NULL) {
            continue;
        }

        /* Internal command: exit */
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        /* Internal command: cd */
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] != NULL) {
                chdir(args[1]);
            }
            continue;
        }

        /* Create a child process using fork(). */
        child = fork();

        if (child == 0) {
            /* Execute external commands with arguments. */
            execvp(args[0], args);
            /* Only runs if execvp fails */
            perror("execvp");
            exit(1);
        } else {
            /* Parent waits for child to finish. */
            waitpid(child, &status, 0);
            child = 0;
        }
    }
    return 0;
}