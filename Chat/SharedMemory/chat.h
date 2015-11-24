#ifndef _CHAT_H
#define _CHAT_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#define shmkey1 0x123
#define shmkey2 0x124
#define semkey 0x134

int shmid1, shmid2, semid;

void create_semaphore(){
	semid = semget(semkey, 2, IPC_CREAT|0666);
	if(semid == -1){
		printf("Error in creating semaphore !\n");
		exit(2);
	}
}

void sem_init(){
	semctl(semid, 0, SETVAL, 0);
	semctl(semid, 1, SETVAL, 0);
}

void sem_wait(int semno){
	struct sembuf sb = {semno, -1, 0};
	semop(semid, &sb, 1);
}

void sem_post(int semno){
	struct sembuf sb = {semno, 1, 0};
	semop(semid, &sb, 1);
}

void create_shm_memory(){
	shmid1 = shmget(shmkey1, 140, IPC_CREAT|0666);
	if(shmid1 == -1){
		printf("Error in creating shared memory !\n");
		exit(1);
	}
	shmid2 = shmget(shmkey2, 140, IPC_CREAT|0666);
	if(shmid2 == -1){
		printf("Error in creating shared memory !\n");
		exit(1);
	}
}

char * attach_shared_memory1(){
	char * ch = (char *)shmat(shmid1, NULL, 0);
	if(ch == (char *)-1){
		printf("Error in attaching memory\n");
		exit(1);
	}
	return ch;
}

char * attach_shared_memory2(){
	char * ch = (char *)shmat(shmid2, NULL, 0);
	if(ch == (char *)-1){
		printf("Error in attaching memory\n");
		exit(1);
	}
	return ch;
}

#endif // _CHAT_H