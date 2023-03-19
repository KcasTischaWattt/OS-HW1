#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"

void call_func(const char *read_pipe, const char *write_pipe) {
    if (dup2(open(read_pipe, O_RDONLY), STDIN_FILENO) == -1 ||
        dup2(open(write_pipe, O_WRONLY), STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./find_substring_indexes", "find_substring_indexes", (char *)NULL);
    perror("execlp");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    if (mkfifo(PIPE1_NAME, 0666) == -1 || mkfifo(PIPE2_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    pid_t chd1, chd2, chd3;

    chd1 = fork();
    if (chd1 == 0) {
        char buffer[BUFSIZ];
        int input_file = open(argv[1], O_RDONLY);
        if (input_file == -1) {
            perror("open input_file");
            exit(1);
        }

        int pfd = open(PIPE1_NAME, O_WRONLY);
        if (pfd == -1) {
            perror("open pipe");
            exit(1);
        }

        int bread;
        while ((bread = read(input_file, buffer, BUFSIZ)) > 0) {
            write(pfd, buffer, bread);
        }

        close(input_file);
        close(pfd);
        exit(0);
    }

    chd2 = fork();
    if (chd2 == 0) {
        call_func(PIPE1_NAME, PIPE2_NAME);
        exit(0);
    }

    chd3 = fork();
    if (chd3 == 0) {
        char buffer[BUFSIZ];
        int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_file == -1) {
            perror("open output_file");
            exit(1);
        }

        int pfd = open(PIPE2_NAME, O_RDONLY);
        if (pfd == -1) {
            perror("open pipe");
            exit(1);
        }

        int bread;
        while ((bread = read(pfd, buffer, BUFSIZ)) > 0) {
            write(output_file, buffer, bread);
        }

        close(output_file);
        close(pfd);
        exit(0);
    }

    waitpid(chd1, NULL, 0);
    waitpid(chd2, NULL, 0);
    waitpid(chd3, NULL, 0);

    unlink(PIPE1_NAME);
    unlink(PIPE2_NAME);

    return 0;
}

