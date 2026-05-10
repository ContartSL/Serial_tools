#include <stdio.h>
#include <fcntl.h>  // 文件控制选项
#include <termios.h> // 终端I/O接口, 用于串口属性配置
#include <unistd.h> //  POSIX 操作系统api

// 接受一个已经打开的文件描述fd 和波特率参数 但实际这里并没有使用
int set_serial_attr(int fd, int baud){  
    // 定义结构体, 用于存储串口(终端)的配置文件
    struct termios tty;
    if(tcgetattr(fd, &tty) < 0 ) return -1;
    // tcgetattr() 从文件描述符 fd 关联的设备 读取当前属性, 存入tty 成功为0 失败为-1

    cfsetospeed(&tty, B115200); // 输出
    cfsetispeed(&tty, B115200); // 输入

    tty.c_cflag |= (CLOCAL | CREAD);    // 忽略调节器控制线 启动接收器, 允许串口读取数据
    tty.c_cflag &= ~CSIZE;              //  ~CSIZE 是数据掩码. &= ~CSIZE 先清零数据位字段, 为后续设置8位数据位做准备
    tty.c_cflag |= CS8;                 // 设置数据位
    tty.c_cflag &= ~PARENB;             // 关闭奇偶校验 PARENB 是启动奇偶校验位的意思, 清零则表示无校验
    tty.c_cflag &= ~CSTOPB;             // 设置停止位 CSTOPB 为 1 则是两个停止位, 清零则代表一个停止位
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // 本地标志, 



    // 特殊控制字符 c_cc 配置
    tty.c_cc[VMIN] = 1;         // vmin 在非规范模式下,指定最少读取字符数, 设置为1表示 read 至少返回1个字节才结束
    tty.c_cc[VTIME]= 0;         // vtime 非规范模式下, 字符超时时间单位为0.1秒, 设置为0表示无超时
    return tcsetattr(fd, TCSANOW, &tty);
    // 应用配置
}

int main(){
    // 打开tty文件设备, O_RDWR 以只读打开, O_NOCTTY 防止该设备成为进程控制终端
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);

    // 如果打开失败 调用 perror 打印错误信息, 
    if (fd < 0 ){ perror("open"); return 1;}
    
    // 设置串口属性
    set_serial_attr(fd, B115200);

    // 定义字符缓冲区 256个字节 
    char buf[256];
    while(1){
        // 打印提示符 ">" 并理解刷新标准输出缓冲区
        printf(">");
        fflush(stdout);

        // 从标准输入读取最多256字节到 buf 返回直接字节数n
        int n = read(0, buf, sizeof(buf));
        // 写入串口
        write(fd, buf, n);
        // 从串口中读取数据, 输出
        n = read(fd, buf, sizeof(buf));
        write(1, buf, n);
    }
    close(fd);
}


/*
    while(1){
        // 打印提示符 ">" 并理解刷新标准输出缓冲区
        printf(">");
        fflush(stdout);

        // 从标准输入读取最多256字节到 buf 返回直接字节数n
        int n = read(0, buf, sizeof(buf));   1
        // 写入串口
        write(fd, buf, n);
        // 从串口中读取数据, 输出
        n = read(fd, buf, sizeof(buf));    3
        write(1, buf, n);
    }

    在这串代码中, read()会阻塞, 直到按下某个按键
    在按下按键之前 程序一直卡在 read() 1函数上
    完全没有执行 3的代码(串口读取)
    工控板启动时通过串口主动输出日志 会一直滞留咋内核缓冲区中
*/


