#include "IPC.h"

void barber(int * buf){
	int i;
	for (i = 0; i < 10; ++i){
		sem_wait(0);
		if(*buf == MAX_SEATS){
			printf("Barber has Slept\n");
		}
		sem_post(0);
		sem_wait(2); // acquire customer semaphore
		printf("customer wakiing up barber\n");
		sem_wait(0); // acquire access seats
		*buf = *buf+1;
		sem_post(1); // release barber
		sem_post(0); // release access seats
		printf("Barber is cutting hair\n");
		sleep(3);
	}
}

int main(int argc, char const *argv[])
{
	init();
	sem_init();
	int * buf = (int *)shmat(shmid, NULL, 0);
	if(buf == (int *)-1){
		printf("Error in attaching memory.\n");
	} else {
		*buf = MAX_SEATS;
		barber(buf);
	}
	destroy();
	return 0;
}