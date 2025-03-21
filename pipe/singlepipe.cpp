#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>  // 非阻塞模式需要

int main(void) 
{
    int fd[2];
    int ret;
    char buff1[1024];
    char buff2[1024];

    ret = pipe(fd);
    if (ret != 0) {
        perror("create pipe failed!");
        exit(1);
    }

    strcpy(buff1, "Hello!");
    write(fd[1], buff1, strlen(buff1) + 1); // 写入数据包括终止符
    printf("send information: %s\n", buff1);

    memset(buff2, 0, sizeof(buff2)); // 使用 memset 清零缓冲区

    int bytes_read = read(fd[0], buff2, sizeof(buff2) - 1); // 确保不会溢出
    if (bytes_read > 0) {
        printf("received information: %s\n", buff2);
    } else {
        perror("read failed");
    }

    return 0;    
}
