#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define ALIGNMENT 512 // 硬盘扇区对齐

int main() {
    const char *file_path = "test_file.txt";
    const size_t buffer_size = 4096; // 缓冲区大小，通常为块设备的倍数

    // 打开文件，使用 O_DIRECT 标志
    int fd = open(file_path, O_RDWR|O_CREAT | O_DIRECT);
    if (fd == -1) {
        perror("打开文件失败");
        return -1;
    }

    // 分配对齐的内存
    void *buffer;
    if (posix_memalign(&buffer, ALIGNMENT, buffer_size) != 0) {
        perror("内存对齐失败");
        close(fd);
        return -1;
    }

    // 准备写入数据
    memset(buffer, 'A', buffer_size); // 将缓冲区填充为'A'

    // 写入文件
    ssize_t bytes_written = write(fd, buffer, buffer_size);
    if (bytes_written == -1) {
        perror("写入文件失败");
        free(buffer);
        close(fd);
        return -1;
    } else {
        printf("写入了 %zd 字节\n", bytes_written);
    }

    // 从文件读取数据
    memset(buffer, 0, buffer_size); // 清空缓冲区
    ssize_t bytes_read = pread(fd, buffer, buffer_size, 0); // 从文件开头读取
    if (bytes_read == -1) {
        perror("读取文件失败");
        free(buffer);
        close(fd);
        return -1;
    } else {
        printf("读取了 %zd 字节: %s\n", bytes_read, (char *)buffer);
    }

    // 释放资源
    free(buffer);
    close(fd);

    return 0;
}

