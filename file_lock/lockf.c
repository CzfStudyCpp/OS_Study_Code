#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define NUM_PROCESSES 4 // 并发进程数量

void write_to_file(int id) {
    printf("Process %d (PID: %d): Starting...\n", id, getpid());

    int fd = open("poem.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    printf("Process %d (PID: %d): Trying to lock the file...\n", id, getpid());

    // 加锁
    if (lockf(fd, F_LOCK, 0) == -1) {
        perror("Process failed to lock file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Process %d (PID: %d): File locked. Writing to the file...\n", id, getpid());

    // 写入文件内容
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Process %d (PID: %d): 李白《静夜思》\n床前明月光，疑是地上霜。\n举头望明月，低头思故乡。\n\n", id, getpid());
    if (write(fd, buffer, strlen(buffer)) == -1) {
        perror("Error writing to file");
        lockf(fd, F_ULOCK, 0);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 模拟耗时操作
    sleep(2);

    // 解锁
    printf("Process %d (PID: %d): Unlocking the file...\n", id, getpid());
    if (lockf(fd, F_ULOCK, 0) == -1) {
        perror("Process failed to unlock file");
    }

    close(fd);
    printf("Process %d (PID: %d): Finished and exiting.\n", id, getpid());
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
            // 父进程记录子进程PID
            printf("Parent: Created Process %d with PID: %d\n", i + 1, pid);
            pids[i] = pid;
        }
    }

    // 父进程等待所有子进程完成
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        int status;
        pid_t pid = waitpid(pids[i], &status, 0);
        if (pid > 0) {
            printf("Parent: Process with PID: %d has terminated.\n", pid);
        }
    }

    printf("All processes have completed.\n");
    return 0;
}
