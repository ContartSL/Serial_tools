#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

int set_serial_attr(int fd, int baud){
    struct termios tty;
    if(tcgetattr(fd, &tty)<0) return -1;
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= -CSTOPB;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    return tcsetattr(fd, TCSANOW, &tty);
}

int main(){
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0){ perror("open"); return 1;}
    set_serial_attr(fd, B115200);

    char buf[256];
    fd_set rtds;
    int maxfd = (fd > 0) ? fd: 0;

    while(1){
        FD_ZERO(&rtds);
        FD_SET(0, &rtds);
        FD_SET(fd, &rtds);

        if(select(maxfd +1, &rtds, NULL, NULL, NULL) < 0){
            perror("select");
            break;
        }

        if(FD_ISSET(0, &rtds)){
            int n = read(0, buf, sizeof(buf));
            if(n > 0) write(fd, buf, n);
        }

        if(FD_ISSET(fd, &rtds)){
            int n = read(fd, buf, sizeof(buf));
            if(n > 0) write(1, buf, n);
        }
    }
    close(fd);
    return 0;
}
