#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define MSG_SIZE 80

struct my_msg_st {
	long int msg_type;
	char msg[MSG_SIZE];
};

int main(void)
{
	int msgid;
	int ret;
	struct my_msg_st msg;

	msgid = msgget((key_t)1235, 0666|IPC_CREAT);
	if (msgid == -1) {
		printf("msgget failed!\n");
		exit(1);
	}

	msg.msg_type = 0;	
	ret = msgrcv(msgid, &msg, MSG_SIZE, 0, 0);
	if (ret == -1) {
		printf("msgrcv failed!\n");
		exit(1);
	}

	printf("received: %s\n", msg.msg);

	ret = msgctl(msgid, IPC_RMID, 0);
	if (ret == -1) {
		printf("msgctl(IPC_RMID) failed!\n");
		exit(1);
	}

	return 0;
}
