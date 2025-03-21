#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define SERVER_PORT 666
#define HEADER_TAG "FBEB"  // 包标识
#define HEADER_SIZE 8      // 包头部长度（4字节标识 + 4字节数据长度）

int main(void) {
    int sock;
    struct sockaddr_in server_addr;

    // 创建Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 配置服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // 绑定端口
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    // 监听
    if (listen(sock, 128) < 0) {
        perror("listen");
        close(sock);
        return 1;
    }

    printf("等待客户端的连接...\n");

    while (1) {
        struct sockaddr_in client;
        int client_sock, len;
        char client_ip[64] = {0};
        socklen_t client_addr_len = sizeof(client);

        // 接受客户端连接
        client_sock = accept(sock, (struct sockaddr *)&client, &client_addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        printf("client ip : %s\t port: %d\n",
               inet_ntop(AF_INET, &client.sin_addr.s_addr, client_ip, sizeof(client_ip)),
               ntohs(client.sin_port));

        // 读取数据包头
        char header_buffer[HEADER_SIZE];
        len = read(client_sock, header_buffer, HEADER_SIZE);
        if (len != HEADER_SIZE) {
            perror("read header");
            close(client_sock);
            continue;
        }

        // 验证包标识
        if (strncmp(header_buffer, HEADER_TAG, 4) != 0) {
            printf("Invalid packet tag\n");
            close(client_sock);
            continue;
        }

        // 获取数据长度
        uint32_t data_length = 0;
        memcpy(&data_length, header_buffer + 4, 4); // 从包头的后4字节读取数据长度
        data_length = ntohl(data_length);           // 转为主机字节序

        printf("Data length: %u\n", data_length);

        // 读取数据内容
        char *data_content = (char *)malloc(data_length + 1);
        if (data_content == NULL) {
            perror("malloc");
            close(client_sock);
            continue;
        }

        len = read(client_sock, data_content, data_length);
        if (len != data_length) {
            perror("read data");
            free(data_content);
            close(client_sock);
            continue;
        }

        data_content[data_length] = '\0'; // 确保字符串结束
        printf("Received data: %s\n", data_content);

        // 构造响应数据包
        const char *response_message = "Data received successfully";
        uint32_t response_length = strlen(response_message);
        uint32_t response_length_network = htonl(response_length); // 转为网络字节序

        char response_packet[HEADER_SIZE + response_length];
        memcpy(response_packet, HEADER_TAG, 4);                      // 添加包标识
        memcpy(response_packet + 4, &response_length_network, 4);    // 添加数据长度
        memcpy(response_packet + HEADER_SIZE, response_message, response_length); // 添加数据内容

        // 发送响应数据包
        write(client_sock, response_packet, sizeof(response_packet));

        // 释放资源
        free(data_content);
        close(client_sock);
    }

    close(sock);
    return 0;
}
