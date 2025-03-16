#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80 /* The maximum length command */

/* Function to execute a command */
void execute_command(char **args) {
    if (execvp(args[0], args) == -1) {
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    char *args[MAX_LINE/2 + 1];    /* command line arguments */
    int should_run = 1;            /* flag to determine when to exit program */
    char input[MAX_LINE];          /* buffer to store the input command */
    char last_command[MAX_LINE];   /* buffer to store the last command */
    int has_history = 0;           /* flag to track if there's a command in history */
    pid_t pid;                     /* process id */
    int background;                /* flag for background processes */

    while (should_run) {
        background = 0;
        printf("/n");
        printf("osh> ");
        fflush(stdout);

        /* Read the command from user */
        if (fgets(input, MAX_LINE, stdin) == NULL) {
            printf("\n");
            exit(0);
        }

        /* Remove trailing newline */
        input[strcspn(input, "\n")] = 0;

        /* Check for history command (!!) */
        if (strcmp(input, "!!") == 0) {
            if (has_history) {
                printf("%s\n", last_command);
                strcpy(input, last_command);
            } else {
                printf("No commands in history.\n");
                continue;
            }
        } else {
            /* Save command to history if it's not empty */
            if (strlen(input) > 0) {
                strcpy(last_command, input);
                has_history = 1;
            }
        }

        /* Parse the command into tokens */
        char *token;
        int i = 0;
        int input_redirect = 0;
        int output_redirect = 0;
        char *input_file = NULL;
        char *output_file = NULL;
        int pipe_present = 0;
        char *args2[MAX_LINE/2 + 1];  /* Arguments for the second command */
        int j = 0;

        token = strtok(input, " ");
        while (token != NULL && i < MAX_LINE/2) {
            /* Check for pipe */
            if (strcmp(token, "|") == 0) {
                pipe_present = 1;
                args[i] = NULL;
                token = strtok(NULL, " ");
                while (token != NULL && j < MAX_LINE/2) {
                    args2[j] = token;
                    j++;
                    token = strtok(NULL, " ");
                }
                args2[j] = NULL;
                break;
            }
            /* Check for input redirection */
            else if (strcmp(token, "<") == 0) {
                input_redirect = 1;
                token = strtok(NULL, " ");
                if (token != NULL) {
                    input_file = token;
                }
            }
            /* Check for output redirection */
            else if (strcmp(token, ">") == 0) {
                output_redirect = 1;
                token = strtok(NULL, " ");
                if (token != NULL) {
                    output_file = token;
                }
            }
            else {
                args[i] = token;
                i++;
            }
            token = strtok(NULL, " ");
        }

        /* If no pipe was found, NULL terminate args */
        if (!pipe_present) {
            args[i] = NULL;
        }

        /* Check if there are any arguments */
        if (i == 0) {
            continue;
        }

        /* Check if the last token is & */
        if (!pipe_present && strcmp(args[i-1], "&") == 0) {
            background = 1;    /* Set background flag */
            args[i-1] = NULL;  /* Remove & from arguments */
            i--;
        }

        /* Check for the exit command */
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            continue;
        }

        if (pipe_present) {
            int pipefd[2];
            pid_t pid2;

            if (pipe(pipefd) < 0) {
                perror("Pipe creation failed");
                continue;
            }

            /* Fork first child for first command */
            pid = fork();

            if (pid < 0) {
                perror("Fork failed");
                continue;
            }
            else if (pid == 0) {
                /* First child process */
                /* Close read end of pipe */
                close(pipefd[0]);
                
                /* Redirect stdout to write end of pipe */
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);

                /* Execute first command */
                execute_command(args);
            }
            else {
                /* Parent process */
                /* Fork second child for second command */
                pid2 = fork();

                if (pid2 < 0) {
                    perror("Fork failed");
                    continue;
                }
                else if (pid2 == 0) {
                    /* Second child process */
                    /* Close write end of pipe */
                    close(pipefd[1]);
                    
                    /* Redirect stdin to read end of pipe */
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);

                    /* Execute second command */
                    execute_command(args2);
                }
                else {
                    /* Parent process */
                    close(pipefd[0]);
                    close(pipefd[1]);
                    wait(NULL);
                    wait(NULL);
                }
            }
        }
        else {
            /* Fork a child process */
            pid = fork();

            if (pid < 0) {    /* Error occurred */
                fprintf(stderr, "Fork failed\n");
                return 1;
            }
            else if (pid == 0) {    /* Child process */
                /* Handle input redirection */
                if (input_redirect) {
                    int fd = open(input_file, O_RDONLY);
                    if (fd < 0) {
                        perror("Error opening input file");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                /* Handle output redirection */
                if (output_redirect) {
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("Error opening output file");
                        exit(EXIT_FAILURE);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                /* Execute the command */
                execute_command(args);
            }
            else {    /* Parent process */
                /* If not background, wait for child */
                if (!background) {
                    wait(NULL);
                }
                else {
                    printf("[Background process started with PID: %d]\n", pid);
                }
            }
        }
    }

    return 0;
}