#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>  // 添加头文件

typedef void (*spawn_proc_pt)(void *data);//定义函数指针别名
static void worker_process_cycle(void *data);
static void start_worker_processes(int n);
pid_t spawn_process(spawn_proc_pt proc, void *data, const char *name);  // 修改类型

int main(int argc, char **argv) {
    start_worker_processes(4);
    // 管理子进程
    wait(NULL);
}

void start_worker_processes(int n) {
    for (int i = n - 1; i >= 0; i--) {
        spawn_process(worker_process_cycle, (void *)(intptr_t)i, "worker process");
    }
}

pid_t spawn_process(spawn_proc_pt proc, void *data, const char *name) {
    pid_t pid = fork();

    switch (pid) {
        case -1:
            fprintf(stderr, "fork() failed while spawning \"%s\"\n", name);
            return -1;
        case 0:
            proc(data);
            return 0;
        default:
            break;
    }
    printf("start %s %ld\n", name, (long int)pid);
    return pid;
}

static void worker_process_init(int worker) {
    cpu_set_t cpu_affinity;
    CPU_ZERO(&cpu_affinity);
    CPU_SET(worker % CPU_SETSIZE, &cpu_affinity);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpu_affinity) == -1) {
        fprintf(stderr, "sched_setaffinity() failed\n");
    }
}

void worker_process_cycle(void *data) {
    int worker = (intptr_t)data;
    worker_process_init(worker);

    for (;;) {
        sleep(10);
        printf("pid %ld ,doing ...\n", (long int)getpid());
    }
}
