#ifndef _IPC_H
#define _IPC_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#define sem_key 0x123
#define shm_key 0x321
#define MAX_SEATS 5

int semid, shmid;

void sem_wait(int sem_num) {
	struct sembuf s={sem_num, -1, 0};
	semop(semid, &s, 1);
}
void sem_post(int sem_num) {
	struct sembuf s={sem_num, 1, 0};
	semop(semid, &s, 1);
}
void init() {
	/**
	 * sem_num = 0 => accessSeats(1) semaaphore
	 * sem_num = 1 => barber(0) semaphore
	 * sem_num = 2 => customer(0) semaphore
	 */
	semid = semget(sem_key, 3, IPC_CREAT|0666);
	if(semid == -1){
		printf("Error in creating semaphore\n");
		exit(1);
	}
	shmid = shmget(shm_key, sizeof(int), IPC_CREAT|0666);
	if(shmid == -1){
		printf("Error in creating shared memory\n");
		exit(1);
	}
}
void sem_init(){
	if(semctl(semid, 0, SETVAL, 1) == -1){
		printf("Error in setting semaphore value.\n");
		exit(2);
	}
	if(semctl(semid, 1, SETVAL, 0) == -1){
		printf("Error in setting semaphore value.\n");
		exit(2);
	}
	if(semctl(semid, 2, SETVAL, 0) == -1){
		printf("Error in setting semaphore value.\n");
		exit(2);
	}
}
void destroy(){
	semctl(semid, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);
}
#endif // _IPC_H