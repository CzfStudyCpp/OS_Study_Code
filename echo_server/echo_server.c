#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_PORT 666

int main(void) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    if (listen(sock, 128) < 0) {
        perror("listen");
        close(sock);
        return 1;
    }

    printf("等待客户端的链接...\n");

    while (1) {
        struct sockaddr_in client;
        int client_sock, len;
        char client_ip[64] = {0};  // 初始化为0，避免潜在的垃圾数据
        socklen_t client_addr_len = sizeof(client);

        client_sock = accept(sock, (struct sockaddr *)&client, &client_addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        printf("client ip : %s\t port: %d\n",
               inet_ntop(AF_INET, &client.sin_addr.s_addr, client_ip, sizeof(client_ip)),
               ntohs(client.sin_port));
        
        //读取客户端发送的数据
        char buffer[256];
        len = read(client_sock, buffer, sizeof(buffer) - 1);
        if (len < 0) {
            perror("read");
        } else {
            buffer[len] = '\0';  // 确保字符串以'\0'结尾
            printf("Received message len:%d from client: %s\n", len, buffer);

            // 根据客户端发送的消息进行回复
            const char *response;
            if (strcmp(buffer, "Hello") == 0) {
                response = "And you?";
            } else if (strcmp(buffer, "Are you a server?") == 0) {
                response = "Yes";
            } else if (strcmp(buffer, "Is it sunny today?") == 0) {
                response = "No";
            } else {
                response = "I don't know";
            }
            write(client_sock, response, strlen(response));
        }

        printf("write finished.len:%d\n", len);
        
        if (close(client_sock) < 0) {
            perror("close client socket");
        }
    }

    if (close(sock) < 0) {
        perror("close server socket");
    }

    return 0;
}
