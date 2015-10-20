#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#define sem_key 0x123
#define shm_key 0x321
#define MAX 10

int pCnt=0, cCnt=0, value=1;
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
	 * sem_num = 0 => empty(MAX) semaaphore
	 * sem_num = 1 => full(0) semaphore
	 * sem_num = 2 => mutex(1) semaphore
	 */
	semid = semget(sem_key, 3 , IPC_CREAT|0666);
	if(semid == -1){
		printf("Error in creating semaphore\n");
		exit(1);
	}
	semctl(semid, 0, SETVAL, 0);
	semctl(semid, 1, SETVAL, MAX);
	semctl(semid, 2, SETVAL, 1);
	shmid = shmget(shm_key, MAX*sizeof(int), IPC_CREAT|0666);
	if(shmid == -1){
		printf("Error in creating semaphore\n");
		exit(1);	
	}
}
void destroy(){
	semctl(semid, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);
}
void consumer(int * buf){
	printf("Consumer started\n");
	int i;
	for(i=0; i<20; i++){
		sem_wait(0);
		sem_wait(2);
		printf("consumer consumed item %d\n", buf[cCnt]);
		cCnt=(cCnt+1)%MAX;
		sem_post(2);
		sem_post(1);
	}
}

void producer(int * buf) {
	printf("Producer started.\n");
	int i;
	for(i=0; i<20; i++){
		sem_wait(1);
		sem_wait(2);
		printf("Producer Produced item %d\n", value);
		buf[pCnt]=value++;
		pCnt=(pCnt+1)%MAX;
		sem_post(2);
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
		consumer(buffer);
		shmdt(buffer);
		exit(2);
	} else {
		int * buffer = (int *)shmat(shmid, NULL, 0);
		if(buffer == (int *)-1){
			printf("Error in Attaching Memory\n");
			exit(1);
		}
		producer(buffer);
		shmdt(buffer);
	}
	destroy();
	return 0;
}
