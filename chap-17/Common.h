
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <error.h>
#include <signal.h>

#define    SERV_PORT      43211
#define    LISTENQ        1024
#define    MAXLINE        4096