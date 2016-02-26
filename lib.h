#include <ev.h>
#include <unistd.h>
#ifndef LIB_H
#define LIB_H

void worker(int sv,int i,const char * home_dir);
ssize_t sock_fd_write(int sock, void *buf, ssize_t buflen, int fd);
ssize_t sock_fd_read(int sock, void *buf, ssize_t bufsize, int *fd);

#endif
