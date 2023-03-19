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

void call_func(const char *src_pipe, const char *dst_pipe) {
    if (dup2(open(src_pipe, O_RDONLY), STDIN_FILENO) == -1 ||
        dup2(open(dst_pipe, O_WRONLY), STDOUT_FILENO) == -1) {
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

    pid_t chd1, chd2;

    chd1 = fork();
    if (chd1 == 0) {
        char buffer[BUFSIZ];
        int input_fd = open(argv[1], O_RDONLY);
        if (input_fd == -1) {
            perror("open input_file");
            exit(1);
        }

        int dst_pipe_fd = open(PIPE1_NAME, O_WRONLY);
        if (dst_pipe_fd == -1) {
            perror("open write_pipe");
            exit(1);
        }

        int bytes_read;
        while ((bytes_read = read(input_fd, buffer, BUFSIZ)) > 0) {
            write(dst_pipe_fd, buffer, bytes_read);
        }

        close(dst_pipe_fd);
        close(input_fd);

        int src_pipe_fd = open(PIPE2_NAME, O_RDONLY);
        if (src_pipe_fd == -1) {
            perror("open read_pipe");
            exit(1);
        }

        int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open output_file");
            exit(1);
        }

        while ((bytes_read = read(src_pipe_fd, buffer, BUFSIZ)) > 0) {
            write(output_fd, buffer, bytes_read);
        }

        close(src_pipe_fd);
        close(output_fd);
        exit(0);
    }

    chd2 = fork();
    if (chd2 == 0) {
        call_func(PIPE1_NAME, PIPE2_NAME);
        exit(0);
    }

    waitpid(chd1, NULL, 0);
    waitpid(chd2, NULL, 0);

    unlink(PIPE1_NAME);
    unlink(PIPE2_NAME);

    return 0;
}

