#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "serial.h"

static int set_serial_attr(int fd, int baud){
    struct termios tty;
    if(tcgetattr(fd, &tty) < 0) return -1;

    speed_t speed;
    switch (baud) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 57600:  speed = B57600; break;
        case 115200: speed = B115200; break;
        default:     return -1;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    return tcsetattr(fd, TCSANOW, &tty);
}

static int serial_open_impl(serial_t *self, const char *path, int baud){
    if (!self) return -1;
    int fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);

    if(fd < 0) return -1;
    if(set_serial_attr(fd, baud) < 0){
        close(fd);
        return -1;
    }
    
    self->fd = fd;
    return 0;
}

static int serial_close_impl(serial_t *self){
    if (!self || self-> fd < 0) return -1;
    int ret = close(self->fd);
    self->fd = -1;
    return ret;
}

static int serial_read_impl(serial_t *self, void *buf, size_t count){
    if(!self || self->fd < 0) return -1;
    return read(self->fd, buf, count);
}

static int serial_write_impl(serial_t *self, const void *buf, size_t count){
    if(!self || self->fd< 0) return -1;
    return write(self->fd, buf, count);
}

serial_t* serial_create(void){
    serial_t *self = (serial_t*)malloc(sizeof(serial_t));
    if(!self) return NULL;
    self->open = serial_open_impl;
    self->close = serial_close_impl;
    self->read = serial_read_impl;
    self->write = serial_write_impl;
    return self;
}

void serial_destroy(serial_t *self){
    if(self){
        if(self-> fd >= 0) self->close(self);
        free(self);
    }
}

