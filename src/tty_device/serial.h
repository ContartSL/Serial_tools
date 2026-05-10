#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>

typedef struct serial_ctx serial_t;

serial_t* serial_create(void);

void serial_destroy(serial_t *self);

typedef struct serial_ctx{
    int (*open)(serial_t *self, const char *path, int baud);
    int (*close)(serial_t *self);
    int (*read)(serial_t *self, void *buf, size_t count);
    int (*write)(serial_t *self, const void *buf, size_t count);
    int fd;
}serial_t;

#endif