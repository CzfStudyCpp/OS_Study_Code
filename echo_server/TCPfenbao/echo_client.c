#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define SERVER_PORT 666
#define SERVER_IP "127.0.0.1"
#define HEADER_TAG "FBEB"  // 包标识
#define HEADER_SIZE 8      // 包头部长度（4字节标识 + 4字节数据长度）

void send_message(const char *message) {
    int sockfd;
    struct sockaddr_in serveaddr;
    ssize_t n;

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

    // 构造数据包
    uint32_t data_length = strlen(message);           // 获取数据长度
    data_length = htonl(data_length);                 // 转为网络字节序

    char packet[HEADER_SIZE + strlen(message)];
    memcpy(packet, HEADER_TAG, 4);                    // 添加包标识
    memcpy(packet + 4, &data_length, 4);              // 添加数据长度
    memcpy(packet + HEADER_SIZE, message, strlen(message)); // 添加数据内容

    // 发送数据包
    write(sockfd, packet, sizeof(packet));

    // 接收服务器响应
    char header_buffer[HEADER_SIZE];
    n = read(sockfd, header_buffer, HEADER_SIZE);
    if (n != HEADER_SIZE) {
        perror("read header");
        close(sockfd);
        exit(1);
    }

    // 验证包标识
    if (strncmp(header_buffer, HEADER_TAG, 4) != 0) {
        printf("Invalid response packet tag\n");
        close(sockfd);
        exit(1);
    }

    // 解析数据长度
    uint32_t response_length = 0;
    memcpy(&response_length, header_buffer + 4, 4); // 从包头的后4字节读取数据长度
    response_length = ntohl(response_length);       // 转为主机字节序

    printf("Response length: %u\n", response_length);

    // 接收数据内容
    char *response_content = (char *)malloc(response_length + 1);
    if (response_content == NULL) {
        perror("malloc");
        close(sockfd);
        exit(1);
    }

    n = read(sockfd, response_content, response_length);
    if (n != response_length) {
        perror("read response content");
        free(response_content);
        close(sockfd);
        exit(1);
    }

    response_content[response_length] = '\0'; // 确保字符串结束
    printf("Received response: %s\n", response_content);

    // 释放资源
    free(response_content);
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fputs("Usage: ./client message\n", stderr);
        exit(1);
    }

    send_message(argv[1]);

    return 0;
}
