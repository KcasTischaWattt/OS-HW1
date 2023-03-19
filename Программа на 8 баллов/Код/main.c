#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"
#define SYNC_PIPE_NAME "sync_pipe"

void call_func(const char *rpipe, const char *wpipe) {
    if (dup2(open(rpipe, O_RDONLY), STDIN_FILENO) == -1 ||
        dup2(open(wpipe, O_WRONLY), STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./find_substring_indexes", "./find_substring_indexes", (char *)NULL);
    perror("execlp");
}

int main() {
    int sync_pipe_fd = open(SYNC_PIPE_NAME, O_RDONLY);
    if (sync_pipe_fd == -1) {
        perror("open sync_pipe");
        exit(1);
    }
    char buffer;
    read(sync_pipe_fd, &buffer, 1);
    close(sync_pipe_fd);
    call_func(PIPE1_NAME, PIPE2_NAME);

    int rfd = open(PIPE1_NAME, O_RDONLY);
    if (rfd == -1) {
        perror("open read_pipe");
        exit(1);
    }

    int wfd = open(PIPE2_NAME, O_WRONLY);
    if (wfd == -1) {
        perror("open write_pipe");
        exit(1);
    }

    return 0;
}
