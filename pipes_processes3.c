#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string to grep>\n", argv[0]);
        return 1;
    }

    int pipe1[2]; 
    int pipe2[2]; 

    pipe(pipe1);
    pipe(pipe2);

    pid_t pid1 = fork();

    if (pid1 == 0) {
        dup2(pipe1[0], STDIN_FILENO); 
        close(pipe1[1]); 
        dup2(pipe2[1], STDOUT_FILENO); 
        close(pipe2[0]); 

        char *grep_args[] = {"grep", argv[1], NULL};
        execvp("grep", grep_args);
        perror("execvp failed");
        exit(1);
    } else {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            dup2(pipe2[0], STDIN_FILENO); // Get input from grep output
            close(pipe2[1]); 

            char *sort_args[] = {"sort", NULL};
            execvp("sort", sort_args);
            perror("execvp failed");
            exit(1);
        } else {
            close(pipe1[0]); 
            close(pipe2[1]); /

            char *cat_args[] = {"cat", "scores", NULL};
            dup2(pipe1[1], STDOUT_FILENO); // Send output to grep input
            execvp("cat", cat_args);
            perror("execvp failed");
            exit(1);
        }
    }

    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}
