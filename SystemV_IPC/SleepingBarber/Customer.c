#include "IPC.h"

void customer(int i, int * buf){
	printf("customer %d started.\n", i);
	sem_wait(0);
	if(*buf > 0){
		*buf = *buf-1;
		printf("customer %d sat down\n", i);
		sem_post(2);
		sem_post(0);
		sem_wait(1);
		printf("Customer %d is having hair cut\n", i);
	} else {
		sem_post(0);
		printf("customer %d left sallon\n", i);
	}
	exit(i);
}

int main(int argc, char const *argv[])
{
	init();
	pid_t pid[10]; int i;

	for(i=0; i<10; i++) {
		pid[i] = fork();
		if(pid[i] == 0){
			int * buf = (int *)shmat(shmid, NULL, 0);
			if(buf == (int *)-1){
				printf("Error in attaching memory.\n");
				exit(1);
			} 
			customer(i, buf);
		}
	}

	int * status;
	for(i=0; i<10; i++){
		waitpid(pid[i], &status, 0);
		if(WEXITSTATUS(status) == i) {
			printf("Customer %d stopped !\n", i);
		}
	}
	exit(0);
}