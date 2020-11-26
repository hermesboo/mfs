#include <stdio.h>
#include <stdlib.h>

/* read line buffer size base */
#define MFS_READLINE_BUFSIZE 1024
/* token buffer size */
#define MFS_TOKEN_BUFSIZE 64
/*  delimiters to be used in strtok() function */
#define MFS_TOKEN_DELIMITER " \t\r\n\a"

char *mfs_read_line(void) {
        int bufsize = MFS_READLINE_BUFSIZE;
        int position = 0;
        char *buffer = malloc(sizeof(char) * bufsize);
        int c;

        /* if we for some reason cant allocate buffer */
        if (!buffer) {
                fprintf(stderr, "MFS: allocation error\n");
                exit(EXIT_FAILURE);
        }
        /* how we read the line */
        while (1) {
                /* getting the characters */
                c = getchar();

                /* if we get EOF || newline we replace it with a null character
                 * and return what has been read */
                if (c == EOF || c == '\n') {
                        buffer[position] = '\0';
                        return buffer;
                        /* this one returns what has been read until whats above
                         * happens */
                } else {
                        buffer[position] = c;
                }
                /* so we dont lose track of where we are */
                position++;

                /* reallocate memory if we exceed the buffer size */
                if (position >= bufsize) {
                        bufsize += MFS_READLINE_BUFSIZE;
                        buffer = realloc(buffer, bufsize);
                        if (!buffer) {
                                fprintf(stderr, "MFS: Allocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
        }
}

char **mfs_split_line(char *line) {
        int bufsize = LSH_TOK_BUFSIZE, position = 0;
        char **tokens = malloc(bufsize * sizeof(char *));
        char *token;

        /* le allocation error */
        if (!tokens) {
                fprintf(stderr, "MFS: allocation error\n");
                exit(EXIT_FAILURE);
        }

        token strtok(line, MFS_TOKEN_DELIMITER);
        while (token != NULL) {
                tokens[position] = token;
                position++;

                if (position >= bufsize) {
                        bufsize += MFS_TOKEN_BUFSIZE;
                        tokens = realloc(tokens, bufsize * sizeof(char *));
                        if (!tokens) {
                                fprintf(stderr, "MFS: allocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
                /* dont forget to do a printf in this area to see how this
                 * function works */
                token = strtok(NULL, LSH_TOK_DELIM);
        }
        tokens[position] = NULL;
        return tokens;
}

int lsh_launch(char **args) {
        /* the type is pid_t and here we initialize 2 variables, pid and wpid */
        pid_t pid, wpid;
        int status;

        pid = fork();
        if (pid == 0) {
                if (execvp(args[0], args) == -1) {
                        perror("MFS");
                }
                exit(EXIT_FAILURE);
        } else if (pid < 0) {
                /* error when trying to fork */
                perror("MFS: Error when forking.\n");
        } else {
                do {
                        wpid = waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        return 1;
}

void mfs_loop(void) {
        char *line;
        char **args;
        int status;

        do {
                printf("mfs> ");
                line = mfs_read_line();
                args = mfs_split_line(line);
                status = lsh_execute(args);

                free(line);
                free(args);
        } while (status);
}

int main(int argc, char **argv) {
        /* main loop */
        mfs_loop();

        return EXIT_SUCCESS;
}
