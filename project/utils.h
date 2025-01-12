#pragma once

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// set a fd to non-blocking mode
static inline void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl(F_GETFL)");
        exit(1);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl(F_SETFL)");
        exit(1);
    }
}
