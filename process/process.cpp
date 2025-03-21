#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t fpid;//fpid表示fork函数返回的值
    int count=0;
    int status = 0;

    fpid=fork();
    if (fpid < 0)
        printf("error in fork!\n");
    else if (fpid == 0) {
        printf("i am the child process, my process id is %d\n",getpid());
        printf("I’m children\n");
        count +=2;
        exit(-10);
    }
    else {
        printf("i am the parent process, my process id is %d\n",getpid());
        printf("I’m parent.\n");
        count++;
    }
    printf("统计结果是: %d\n",count);
    //父进程捕捉子进程的状态
    wait(&status);
    printf("parent: status: %d\n", WEXITSTATUS(status));
    return 0;
}
