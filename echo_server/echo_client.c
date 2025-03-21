#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>  // 包含 exit() 函数

#define SERVER_PORT 666
#define SERVER_IP "127.0.0.1"

void send_message(const char *message) {
    int sockfd;
    struct sockaddr_in serveaddr;
    ssize_t n;  // 使用 ssize_t 类型来处理 read() 返回的字节数
    char buffer[64];  // 初始化 buffer

    printf("sending message: %s...\n", message);

    // 创建 TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&serveaddr, 0, sizeof(struct sockaddr_in));
    serveaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &serveaddr.sin_addr);
    serveaddr.sin_port = htons(SERVER_PORT);

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr *)&serveaddr, sizeof(serveaddr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    // 发送消息
    write(sockfd, message, strlen(message));

    // 读取服务器响应
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';  // 确保读取的内容是一个以 '\0' 结尾的字符串
        printf("received: %s\n", buffer);
    } else {
        printf("error!!!\n");
    }

    printf("finished!\n");

    // 关闭 socket
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fputs("Usage: ./echo_client message \n", stderr);
        exit(1);  // exit() 需要 <stdlib.h>
    }

    send_message(argv[1]);

    // 如果服务器返回"And you?", 重新连接并发送 "Me Too!"
    if (strcmp(argv[1], "Hello") == 0) {
        sleep(1); // 等待一秒再重连
        send_message("Me Too!");
    }

    return 0;
}
