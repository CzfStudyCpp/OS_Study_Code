#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork(); // 创建子进程

    if (pid < 0) {
        // 错误处理
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // 子进程路径
        printf("This is the child process. PID: %d, Parent PID: %d\n", getpid(), getppid());
    } else {
        // 父进程路径
        printf("This is the parent process. PID: %d, Child PID: %d\n", getpid(), pid);
    }

    printf("Process %d is finishing.\n", getpid());
    return 0;
}