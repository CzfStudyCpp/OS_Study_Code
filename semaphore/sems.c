#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)						   
#else
    union semun {
        int val;                             
        struct semid_ds *buf;    
        unsigned short int *array; 
        struct seminfo *__buf;  
    };
#endif     

static sem_initial(int semid) 
{
	int ret;
	
	union semun semun;
	semun.val = 1;
	ret = semctl(semid, 0, SETVAL, semun);
	if (ret == -1) {
		fprintf(stderr, "semctl failed!\n");
	}
	
	return ret;
}

static int  sem_p(int semid)
{
	int ret;
	
	struct sembuf sembuf;
	sembuf.sem_op = -1;
	sembuf.sem_num = 0;
	sembuf.sem_flg = SEM_UNDO;
	ret = semop(semid, &sembuf, 1);	
	if (ret == -1) {
		fprintf(stderr, "sem_p failed!\n");
	}
	
	return ret;
}

static int  sem_v(int semid)
{
	int ret;
	
	struct sembuf sembuf;
	sembuf.sem_op = 1;
	sembuf.sem_num = 0;
	sembuf.sem_flg = SEM_UNDO;
	ret = semop(semid, &sembuf, 1);	
	if (ret == -1) {
		fprintf(stderr, "sem_v failed!\n");
	}
	
	return ret;
}

int main(int argc, char* argv[]) 
{
	int i;
	int ret;
	int semid;

	/* 获取信号量 */
	semid = semget((key_t)1234, 1, 0666 | IPC_CREAT);
	if (semid == -1) {
		printf("semget failed!\n");
		exit(1);
	}

	/* 初始化信号量 */
	if (argc > 1) {
		ret = sem_initial(semid);
		if (ret == -1) {
			exit(1);
		}
	}

	for (i=0; i<5; i++) {

		if (sem_p(semid) == -1) {
			exit(1);
		}
		
		/* 模拟临界区----begin */
		printf("Process(%d) In\n", getpid());		
		sleep(3);
		printf("Process(%d) Out\n", getpid());
              /* 模拟临界区----end */ 

		if (sem_v(semid) == -1) {
			exit(1);
		}
			  
		sleep(1);
	}

    /* 删除信号量 */
	   
	return 0;
}

