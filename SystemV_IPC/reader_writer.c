#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#define sem_key 0x123
#define shm_key 0x321

int semid, shmid;

void sem_wait(int sem_num) {
	struct sembuf s={sem_num, -1, 0};
	semop(semid, &s, 0);
}
void sem_post(int sem_num) {
	struct sembuf s={sem_num, 1, 0};
	semop(semid, &s, 0);
}
void init() {
	/** 
	 * sem_num = 0 => mutex(1) semaaphore
	 * sem_num = 1 => write_block(1) semaphore
	 */
	semid = semget(sem_key, 2, IPC_CREAT|0666);
	if(semid == -1){
		printf("Error in creating semaphore\n");
		exit(1);
	}
	semctl(semid, 0, SETVAL, 1);
	semctl(semid, 1, SETVAL, 1);
	shmid = shmget(shm_key, sizeof(int), IPC_CREAT|0666);
	if(shmid == -1){
		printf("Error in creating semaphore\n");
		exit(1);	
	}
}
void destroy(){
	semctl(semid, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);
}
void writer(int * buf){
	printf("Writer started\n");
	int i;
	for(i=0; i<20; i++){
		sem_wait(1);
		*buf = *buf+1;
		printf("Writer written value %d\n", *buf);
		sem_post(1);
	}
}

void reader(int * buf) {
	printf("Reader started.\n");
	int i, readers=0;
	for(i=0; i<20; i++){
		sem_wait(0);
		readers = readers+1;
		if(readers==1)
			sem_wait(1);
		sem_post(0);
		printf("Readers read value %d\n", *buf);
		sem_wait(0);
		readers = readers-1;
		if(readers == 0)
			sem_post(1);
		sem_post(0);
	}
}

int main() {
	init();
	if(fork() == 0) {
		int *buffer = (int *)shmat(shmid, NULL, 0);
		if(buffer == (int *)-1){
			printf("Error in Attaching Memory\n");
			exit(1);
		}
		reader(buffer);
		shmdt(buffer);
		exit(2);
	} else {
		int * buffer = (int *)shmat(shmid, NULL, 0);
		if(buffer == (int *)-1){
			printf("Error in Attaching Memory\n");
			exit(1);
		}
		writer(buffer);
		shmdt(buffer);
	}
	destroy();
	return 0;
}
