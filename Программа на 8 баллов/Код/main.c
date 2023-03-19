#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PIPE1_NAME "pipe1"
#define PIPE2_NAME "pipe2"
#define SYNC_PIPE_NAME "sync_pipe"

void call_func(const char *rpipe, const char *wpipe) {
    int readfd = open(rpipe, O_RDONLY);
    if (readfd == -1) {
        perror("open rpipe");
        exit(1);
    }

    int writefd = open(wpipe, O_WRONLY);
    if (writefd == -1) {
        perror("open wpipe");
        exit(1);
    }

    if (dup2(readfd, STDIN_FILENO) == -1 ||
        dup2(writefd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    execlp("./find_substring_indexes", "find_substring_indexes", (char *)NULL);
    perror("execlp");
}

void sync(const char *spipe) {
    int syncfd = open(spipe, O_RDONLY);
    if (syncfd == -1) {
        perror("open spipe");
        exit(1);
    }
    char buffer;
    read(syncfd, &buffer, 1);
    close(syncfd);
}

int main() {
    sync(SYNC_PIPE_NAME);
    call_func(PIPE1_NAME, PIPE2_NAME);
    return 0;
}
