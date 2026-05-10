#include "mypthread.h"

void *thread_keyboard(void *arg){
    int fd = *(int*)arg;
    char buf[256];
    while(1){
        int n = read(0, buf, sizeof(buf));
        if(n > 0) write(fd, buf, n);
    }
    return NULL;
}

void *thread_serial(void *arg){
    int fd = *(int*)arg;
    char buf[256];
    while(1){
        int n = read(fd, buf, sizeof(buf));
        if(n > 0) write(1, buf, n);
    }
    return NULL;
}

void *thread_stdin_to_serial(void *arg){
    serial_t *ser = (serial_t *)arg;
    char buf[256];
    ssize_t n;

    while(1){
        n = read(0, buf, sizeof(buf));
        if(n <= 0){
            if (n == 0) break;
            if(errno != EINTR) break;
            continue;
        }
        if (ser->write(ser, buf, n) != n){
            perror("write to serial");
            break;
        }
    }
    return NULL;
}

void *thread_serial_to_stdout(void *arg){
    serial_t *ser = (serial_t *)arg;
    char buf[256];
    ssize_t n;

    while(1){
        n = ser->read(ser, buf, sizeof(buf));
        if(n <= 0){
            if(n == 0) break;
            if(errno != EINTR) break;
            continue;
        }
        if(write(1, buf, n) != n){
            perror("write to stdout");
            break;
        }
    }
    return NULL;
}
