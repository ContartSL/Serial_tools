#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "serial.h"
#include "mypthread.h"

int main(){
    serial_t *ser = serial_create();

    if(!ser){
        fprintf(stderr, "创建串口实例失败\n");
        return 1;
    }

    if(ser->open(ser, "/dev/ttyUSB0", 115200) < 0){
        perror("打开串口失败");
        serial_destroy(ser);
        return 1;
    }

    printf("串口已经打开, 开始实时遗传 \n");

    pthread_t tid_stdin, tid_serial;
    int ret;

    ret = pthread_create(&tid_stdin, NULL, thread_stdin_to_serial, ser);
    if (ret != 0){
        errno = ret;
        perror("创建 stdin->serial 线程失败");
        ser->close(ser);
        serial_destroy(ser);
        return 1;
    }

    ret = pthread_create(&tid_serial, NULL, thread_serial_to_stdout, ser);
    if(ret != 0){
        errno = ret;
        perror("创建 serial-> stdout 线程失败");
        pthread_cancel(tid_serial); // 尝试取消已创建的线程
        pthread_join(tid_serial, NULL);
        ser->close(ser);
        serial_destroy(ser);
        return 1;
    }

    pthread_join(tid_stdin, NULL);
    pthread_join(tid_serial, NULL);

    printf("\n程序退出\n");
    ser->close(ser);
    serial_destroy(ser);
    return 0;



}
