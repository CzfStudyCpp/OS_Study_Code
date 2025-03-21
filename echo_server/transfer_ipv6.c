#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    // 将字符串形式的IPv6地址转换为网络字节序
    const char *ipv6_str = "2409:8a5c:a643:f854:fc84:1719:a895:5";
    struct in6_addr ipv6_addr;
    if (inet_pton(AF_INET6, ipv6_str, &ipv6_addr) != 1) {
        perror("inet_pton");
        return 1;
    }
    printf("IPv6 address (network byte order): 0x");
    for(int i = 0; i < 16; i++) {
        printf("%02x", ipv6_addr.s6_addr[i]);
    }
    printf("\n");

    // 将网络字节序的IPv6地址转换为字符串形式
    char ipv6_str_buf[INET6_ADDRSTRLEN];
    if (inet_ntop(AF_INET6, &ipv6_addr, ipv6_str_buf, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        return 1;
    }
    printf("IPv6 address (text form): %s\n", ipv6_str_buf);

    return 0;
}
