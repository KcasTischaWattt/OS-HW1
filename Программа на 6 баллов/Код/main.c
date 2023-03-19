#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

void call_func(int read_fd, int write_fd) {
    if (dup2(read_fd, STDIN_FILENO) == -1 || dup2(write_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./find_substring_indexes", "find_substring_indexes", (char *)NULL);
    perror("execlp");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_filename> <output_filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pfd1[2], pfd2[2];
    if (pipe(pfd1) == -1 || pipe(pfd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1, pid2;
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        close(pfd1[0]);
        close(pfd2[1]);
        int input_fd = open(argv[1], O_RDONLY);
        if (input_fd == -1) {
            perror("open input_file");
            exit(EXIT_FAILURE);
        }

        int bytes_read;
        char buffer[BUFSIZ];
        while ((bytes_read = read(input_fd, buffer, BUFSIZ)) > 0) {
            write(pfd1[1], buffer, bytes_read);
        }

        close(pfd1[1]);
        close(input_fd);

        int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("open output_file");
            exit(EXIT_FAILURE);
        }

        while ((bytes_read = read(pfd2[0], buffer, BUFSIZ)) > 0) {
            write(output_fd, buffer, bytes_read);
        }

        close(pfd2[0]);
        close(output_fd);
        exit(EXIT_SUCCESS);
    }

    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        close(pfd1[1]);
        close(pfd2[0]);
        call_func(pfd1[0], pfd2[1]);
        exit(EXIT_SUCCESS);
    }

    close(pfd1[0]);
    close(pfd1[1]);
    close(pfd2[0]);
    close(pfd2[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
