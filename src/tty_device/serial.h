#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct serial_ctx serial_t;

typedef struct {
    const char *path;
    int baudrate;
    int data_bits;
    int stop_bits;
    char parity;
} serial_config_t;

serial_t* serial_create(void);
void serial_destroy(serial_t *self);
int serial_configure(serial_t *self, const serial_config_t *cfg);

typedef struct serial_ctx {
    int (*open)(serial_t *self, const serial_config_t *cfg);
    int (*close)(serial_t *self);
    int (*read)(serial_t *self, void *buf, size_t count);
    int (*write)(serial_t *self, const void *buf, size_t count);
    int fd;
} serial_t;

#ifdef __cplusplus
}
#endif

#endif