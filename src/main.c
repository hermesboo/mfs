#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* read line buffer size base */
#define READLINE_BUFSIZE 1024
/* token buffer size */
#define TOKEN_BUFSIZE 64
/*  delimiters to be used in strtok() function */
#define TOKEN_DELIMITER " \t\r\n\a"
/* delimiters to be used in pipes strtok() function */
#define PIPE_DELIMITER "|"
/* pipe bufsize */
#define PIPE_TK_BUFSIZE 1024

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
        int bufsize = READLINE_BUFSIZE;
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
                        bufsize += READLINE_BUFSIZE;
                        buffer = realloc(buffer, bufsize);
                        if (!buffer) {
                                fprintf(stderr, "MFS: Allocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
                //                printf("%s\n", buffer);
        }
}
int parsing(char *line) {
        int i = 0;
        while (line[i] != '\0') {
                printf("main loop:%c\n", line[i]);
                if (line[i] == '|') {
                        printf("hahahah u bitch\n");
                        return 1;
                }
                i++;
        }
        return 0;
}

char **pipe_split(char *line) {
        /* pipe buffer size is bigger because bigger words 6head
         * pposition stands for pipe position*/
        int bufsize = PIPE_TK_BUFSIZE, pposition = 0;
        /* fpipetks means final pipe tokens */
        char **fpipetks = malloc(bufsize * sizeof(char *));
        char *pipetk;
        int testnum = 0;

        printf("Catching line before tokenization: %s\n", line);

        /* standard allocation error :) */
        if (!fpipetks) {
                fprintf(stderr, "MFS: Pipe error\n");
                exit(EXIT_FAILURE);
        }

        pipetk = strtok(line, PIPE_DELIMITER);
        while (pipetk != NULL) {
                fpipetks[pposition] = pipetk;
                pposition++;

                if (pposition >= bufsize) {
                        bufsize += PIPE_TK_BUFSIZE;
                        fpipetks = realloc(fpipetks, bufsize * sizeof(char *));
                        if (!fpipetks) {
                                fprintf(stderr, "MFS: Pipe allocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
                pipetk = strtok(NULL, PIPE_DELIMITER);
        }
        while (testnum < pposition) {
                printf("PIpe tokenized:%s\n", fpipetks[testnum]);
                testnum++;
        }

        fpipetks[pposition] = 0;
        int chucha = 0;
        while (chucha < pposition) {
                printf("give me the ufcking :%s\n", fpipetks[chucha]);
                chucha++;
        }
        printf("hahahah noooo ur so sexy aha:%s \n", fpipetks[2]);
        return fpipetks;
}

char **mfs_split_line(char *line) {
        int bufsize = TOKEN_BUFSIZE, position = 0;
        char **tokens = malloc(bufsize * sizeof(char *));
        char *token;
        int kenke = 0;
        /* le allocation error */
        if (!tokens) {
                fprintf(stderr, "MFS: allocation error\n");
                exit(EXIT_FAILURE);
        }

        token = strtok(line, TOKEN_DELIMITER);
        while (token != NULL) {
                tokens[position] = token;
                position++;

                if (position >= bufsize) {
                        bufsize += TOKEN_BUFSIZE;
                        tokens = realloc(tokens, bufsize * sizeof(char *));
                        if (!tokens) {
                                fprintf(stderr, "MFS: allocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
                /* dont forget to do a printf in this area to see how this
                 * function works */
                token = strtok(NULL, TOKEN_DELIMITER);
        }
        while (kenke < position) {
                printf("Tokenized test: %s\n", tokens[kenke]);
                kenke++;
        }
        tokens[position] = NULL;
        return tokens;
}

int pipe_launch(char **args) {
        pid_t pid, wpid;
        int fd[2];
        int status;
        int argsize = sizeof(args) / sizeof(int);
        char *ls[] = {"ls", 0};
        char *wc[] = {"wc", "-l", 0};

        printf("argument size:%d\n", argsize);

        /* file descriptor piping */
        pipe(fd);
        pid = fork();

        int savestdin = dup(0), savestdout = dup(1);

        /* error handling */
        if (pid == -1) {
                perror("couldnt fork\n");
        }
        if (pid == 0) {  // child process
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                execvp(ls[0], ls);
        } else {  // parent process
                dup2(fd[0], 0);
                close(fd[1]);
                dup2(savestdout, STDOUT_FILENO); /* reconnect stdout */
                execvp(wc[0], wc);
                dup2(savestdin, STDIN_FILENO);
        }
        printf("PID numb: %d\n", pid);
        return 1;
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

/* int pipe_exec(char **args) {
        int i;
        if (args[0] == NULL) {
                // empty commands
                return 1;
        }

        for (i = 0; i < mfs_num_builtins(); i++) {
                if (strcmp(args[0], builtin_str[i]) == 0) {
                        return (*builtin_func[i])(args);
                }
        }
        return pipe_launch(args);
}*/

void mfs_loop(void) {
        char *line;
        char **args;
        int status = 1;
        int numparse = 0;

        do {
                printf("mfs> ");
                line = mfs_read_line();
                /* we parse the line before doing anything to have a more clean
                 * and elegant solution */
                numparse = parsing(line);
                if (numparse == 1) {
                        args = pipe_split(line);
                        status = pipe_launch(args);
                        printf("status: %d", status);
                        free(line);
                        free(args);

                } else {
                        args = mfs_split_line(line);
                        status = mfs_execute(args);
                        free(line);
                        free(args);
                }

        } while (status);
}

int main(int argc, char **argv) {
        /* main loop */
        mfs_loop();

        return EXIT_SUCCESS;
}
