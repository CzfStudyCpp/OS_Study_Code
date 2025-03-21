#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myhandle(int sig) 
{
	printf("Catch a signal : %d\n", sig);
	signal(SIGINT,SIG_DFL);
}

int main(void) 
{
	struct sigaction act;

	act.sa_handler = myhandle;
	sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

	sigaction(SIGINT, &act, 0);

	while (1) {
        
	}

	return 0;
}
