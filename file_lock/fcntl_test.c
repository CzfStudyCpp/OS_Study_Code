#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_PROCESSES 10  // 测试用较小数量，方便观察
#define SECTION_SIZE 200   // 每个进程写入的文件区域大小

void write_to_file(int id) {
    printf("Process %d (PID: %d): Starting...\n", id, getpid());
    fflush(stdout); // 确保日志实时输出

    int fd = open("poem.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct flock lock;
    lock.l_type = F_WRLCK;         // 写锁
    lock.l_whence = SEEK_SET;      // 锁定方式：从文件开头计算
    lock.l_start = (id - 1) * SECTION_SIZE; // 每个进程锁定不同的区域
    lock.l_len = 0;     // 锁定 SECTION_SIZE 大小的区域

    printf("Process %d (PID: %d): Trying to lock section %ld...\n", id, getpid(), lock.l_start);
    fflush(stdout);

    // 阻塞式加锁
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Process failed to lock file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Process %d (PID: %d): Locked section %ld, writing...\n", id, getpid(), lock.l_start);
    fflush(stdout);

    // 写入文件内容
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Process %d (PID: %d): 李白《静夜思》版本 %d\n床前明月光，疑是地上霜。\n举头望明月，低头思家乡（版本%d）。\n\n",
             id, getpid(), id, id);
    
    if (lseek(fd, lock.l_start, SEEK_SET) == -1) {
        perror("Error seeking in file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, strlen(buffer)) == -1) {
        perror("Error writing to file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 模拟写入耗时操作
    sleep(2);

    // 释放锁（使用 `close(fd)` 让系统自动释放）
    printf("Process %d (PID: %d): Unlocking section %ld and closing file...\n", id, getpid(), lock.l_start);
    fflush(stdout);

    close(fd);  // 自动释放锁
    printf("Process %d (PID: %d): Finished and exiting.\n", id, getpid());
    fflush(stdout);
}

int main() {
    pid_t pids[NUM_PROCESSES];

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // 子进程
            write_to_file(i + 1);
            exit(EXIT_SUCCESS);
        } else {
            // 记录子进程 PID
            printf("Parent: Created Process %d with PID: %d\n", i + 1, pid);
            fflush(stdout);
            pids[i] = pid;
        }
    }

    // 等待所有子进程结束
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        int status;
        pid_t pid = waitpid(pids[i], &status, 0);
        if (pid > 0) {
            printf("Parent: Process %d (PID: %d) has terminated.\n", i + 1, pid);
            fflush(stdout);
        }
    }

    printf("All processes have completed.\n");
    fflush(stdout);
    return 0;
}
