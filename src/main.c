#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* read line buffer size base */
#define MFS_READLINE_BUFSIZE 1024
/* token buffer size */
#define MFS_TOKEN_BUFSIZE 64
/*  delimiters to be used in strtok() function */
#define MFS_TOKEN_DELIMITER " \t\r\n\a"

/* we declare these because we going to use pointers to their location before
 * actually using them */
int mfs_cd(char **args);
int mfs_help(char **args);
int mfs_exit(char **args);

/* built in commands */
char *builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {&mfs_cd, &mfs_help, &mfs_exit};

int mfs_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

/* actual implementation of the builtins */
int mfs_cd(char **args) {
        if (args[1] == NULL) {
                fprintf(stderr, "MFS: Expected argument to \"cd\"\n");
        } else {
                if (chdir(args[1]) != 0) {
                        perror("MFS CD");
                }
        }
        return 1;
}

int mfs_help(char **args) {
        int i;
        printf("My friendly shell :)\n");
        printf("Builtin commands are the following:\n");

        for (i = 0; i < mfs_num_builtins(); i++) {
                printf("  %s\n", builtin_str[i]);
        }

        printf("Use man [command] for information on other programs.\n");
        return 1;
}

int mfs_exit(char **args) { return 0; }

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

                printf("%c\n", c);

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
        int bufsize = MFS_TOKEN_BUFSIZE, position = 0;
        char **tokens = malloc(bufsize * sizeof(char *));
        char *token;

        /* le allocation error */
        if (!tokens) {
                fprintf(stderr, "MFS: allocation error\n");
                exit(EXIT_FAILURE);
        }

        token = strtok(line, MFS_TOKEN_DELIMITER);
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
                token = strtok(NULL, MFS_TOKEN_DELIMITER);
        }
        tokens[position] = NULL;
        return tokens;
}

int mfs_launch(char **args) {
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

int mfs_execute(char **args) {
        int i;

        if (args[0] == NULL) {
                /* empty command */
                return 1;
        }

        for (i = 0; i < mfs_num_builtins(); i++) {
                if (strcmp(args[0], builtin_str[i]) == 0) {
                        return (*builtin_func[i])(args);
                }
        }

        return mfs_launch(args);
}

void mfs_loop(void) {
        char *line;
        char **args;
        int status;

        do {
                printf("mfs> ");
                line = mfs_read_line();
                args = mfs_split_line(line);
                status = mfs_execute(args);

                free(line);
                free(args);
        } while (status);
}

int main(int argc, char **argv) {
        /* main loop */
        mfs_loop();

        return EXIT_SUCCESS;
}
