#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) 
{
    int fd[2];
    int ret;
    char buff1[1024];
    char buff2[1024];
    pid_t pd;

    ret = pipe(fd);
    if (ret != 0) {
        perror("create pipe failed!");
        exit(1);
    }

    pd = fork();
    if (pd == -1) {
        perror("fork error!");
        exit(1);
    } else if (pd == 0) { 
        // Child process
        // Write to parent
        strcpy(buff1, "Message from child");
        write(fd[1], buff1, strlen(buff1) + 1); // Include null terminator
        printf("Child process(%d) sent information: %s\n", getpid(), buff1);

        // Read from parent
        memset(buff2, 0, sizeof(buff2)); // Clear buffer
        read(fd[0], buff2, sizeof(buff2));
        printf("Child process(%d) received information: %s\n", getpid(), buff2);
    } else { 
        // Parent process
        // Write to child
        strcpy(buff1, "Message from parent");
        write(fd[1], buff1, strlen(buff1) + 1); // Include null terminator
        printf("Parent process(%d) sent information: %s\n", getpid(), buff1);

        // Read from child
        memset(buff2, 0, sizeof(buff2)); // Clear buffer
        read(fd[0], buff2, sizeof(buff2));
        printf("Parent process(%d) received information: %s\n", getpid(), buff2);

        wait(NULL); // Wait for the child process to finish
    }

    return 0;
}
