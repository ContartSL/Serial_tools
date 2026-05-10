#ifndef MYPTHREAD_H
#define MYPTHREAD_H

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "serial.h"

void *thread_keyboard(void *arg);
void *thread_serial(void *arg);
void *thread_stdin_to_serial(void *arg);
void *thread_serial_to_stdout(void *arg);

#endif