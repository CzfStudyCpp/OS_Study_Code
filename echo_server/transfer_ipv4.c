#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    // 将字符串形式的IPv4地址转换为网络字节序
    const char *ipv4_str = "192.168.1.1";
    struct in_addr ipv4_addr;
    if (inet_pton(AF_INET, ipv4_str, &ipv4_addr) != 1) {
        perror("inet_pton");
        return 1;
    }
    printf("IPv4 address (network byte order): 0x%x\n", ipv4_addr.s_addr);

    // 将网络字节序的IPv4地址转换为主机字节序
    uint32_t host_order_ip = ntohl(ipv4_addr.s_addr);
    printf("IPv4 address (host byte order): 0x%x\n", host_order_ip);

    // 将主机字节序的IPv4地址转换为字符串形式
    struct in_addr host_ipv4_addr;
    host_ipv4_addr.s_addr = htonl(host_order_ip); // htonl to convert back to network byte order
    char host_ipv4_str_buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &host_ipv4_addr, host_ipv4_str_buf, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        return 1;
    }
    printf("IPv4 address (text form by host byte order): %s\n", host_ipv4_str_buf);

    // 将网络字节序的IPv4地址转换为字符串形式
    char ipv4_str_buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &ipv4_addr, ipv4_str_buf, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        return 1;
    }
    printf("IPv4 address (text form): %s\n", ipv4_str_buf);

    // 对比主机字节序的IP地址字符串形式与网络字节序的IP地址字符串形式
    if (strcmp(ipv4_str_buf, host_ipv4_str_buf) == 0) {
        printf("IP addresses match.\n");
    } else {
        printf("IP addresses do not match.\n");
    }

    return 0;
}
