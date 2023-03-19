#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

void call_func(int read_fd, int write_fd) {
    if (dup2(read_fd, STDIN_FILENO) == -1 || dup2(write_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./find_substring_indexes", "find_substring_indexes", (char *)NULL);
    perror("execlp");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t chd1 = fork();
    if (chd1 == 0) {
        close(pipe1[0]);
        char buffer[BUFSIZ];
        int input_file = open(argv[1], O_RDONLY);
        if (input_file == -1) {
            perror("open input_file");
            exit(1);
        }

        int bread;
        while ((bread = read(input_file, buffer, BUFSIZ)) > 0) {
            write(pipe1[1], buffer, bread);
        }

        close(input_file);
        close(pipe1[1]);
        exit(0);
    }

    pid_t chd2 = fork();
    if (chd2 == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        call_func(pipe1[0], pipe2[1]);
        exit(0);
    }

    pid_t chd3 = fork();
    if (chd3 == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        char buffer[BUFSIZ];
        int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_file == -1) {
            perror("open output_file");
            exit(1);
        }

        int bread;
        while ((bread = read(pipe2[0], buffer, BUFSIZ)) > 0) {
            write(output_file, buffer, bread);
        }
        close(output_file);
        close(pipe2[0]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    waitpid(chd1, NULL, 0);
    waitpid(chd2, NULL, 0);
    waitpid(chd3, NULL, 0);

    return 0;
}
