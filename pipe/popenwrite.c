#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE   1024

int main(void)
{
    FILE *file;
    char buff[BUFF_SIZE + 1];
    int cnt;

    // 使用正确的命令
    file = popen("cat > cat.txt", "w");
    if (!file) {
        perror("popen failed!");
        exit(1);
    }

    // 写入数据
    strcpy(buff, "hello world!");
    cnt = fwrite(buff, sizeof(char), strlen(buff), file);
    if (cnt < strlen(buff)) {
        printf("Write operation incomplete!\n");
    } else {
        printf("Data written successfully!\n");
    }

    // 关闭管道
    pclose(file);

    return 0;   
}
