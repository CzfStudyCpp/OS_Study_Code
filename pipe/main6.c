#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(void) 
{
	int fd[2];
	int ret;
	char buff1[1024];
	char buff2[1024];
	pid_t pd;

	ret = pipe(fd);
	if (ret !=0) {
		printf("create pipe failed!\n");
		exit(1);
	}

	pd = fork();
	if (pd == -1) {
		printf("fork error!\n");
		exit(1);
	} else if (pd == 0) {
		//bzero(buff2, sizeof(buff2));
		//sprintf(buff2, "%d", fd[0]);
		close(fd[1]);

		close(0);
		dup(fd[0]);
		close(fd[0]);
		
		execlp("./od.exe", "./od.exe", "-c", 0);
		printf("execl error!\n");
		exit(1);
	} else {
		close(fd[0]);
	
		strcpy(buff1, "Hello!");
		write(fd[1], buff1, strlen(buff1)); 

		close(fd[1]);
	}
	
	return 0;	
}
