#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#define sem_key 0x123
#define shm_key 0x321
#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int semid, sem_mtx, shmid;

void lock(){
	struct sembuf s={0, -1, 0};
	semop(sem_mtx, &s, 1);
}

void unlock(){
	struct sembuf s={0, 1, 0};
	semop(sem_mtx, &s, 1);
}

void sem_wait(int sem_num) {
	struct sembuf s={sem_num, -1, 0};
	semop(semid, &s, 1);
}
void sem_post(int sem_num) {
	struct sembuf s={sem_num, 1, 0};
	semop(semid, &s, 1);
}
void test(int i, int * buf) {
	if(*(buf+i) == HUNGRY && *(buf+(i+4)%N) != EATING && *(buf+(i+1)%N) != EATING){
		*(buf+i) = EATING;
		printf("philosphar %d is picked fork & EATING\n", i);
		sem_post(i);
	}
}
void pick_fork(int i, int * buf){
	lock();
	*(buf+i) = HUNGRY;
	printf("philosphar %d is HUNGRY\n", i);
	test(i, buf);
	unlock();
	sem_wait(i);
}
void put_fork(int i, int * buf){
	lock();
	*(buf+i)=THINKING;
	printf("philosphar %d leaved fork & THINKING\n", i);

	test((i+4)%N, buf);
	test((i+1)%N, buf);
	unlock();
}
void philosphar(int i, int * buf){
	printf("philosphar %d started.\n", i);
	int j=0;
	while(j<5){
		pick_fork(i, buf);
		sleep(rand()%3);
		put_fork(i, buf);
		j++;
	}
	exit(i);
}
void destroy(){
	semctl(semid, IPC_RMID, 0);
	semctl(sem_mtx, IPC_RMID, 0);
	shmctl(shmid, IPC_RMID, 0);
}

int main(){
	pid_t pid[N];
	semid = semget(IPC_PRIVATE, N, IPC_CREAT|0666);
	sem_mtx = semget(IPC_PRIVATE, 1, IPC_CREAT|0666);
	if(semid == -1 || sem_mtx == -1){
		printf("Error in creating semaphore.\n");
		exit(1);
	}
	int i;
	semctl(sem_mtx, 0, SETVAL, 1);
	for(i=0; i<N;i++){
		semctl(semid, i, SETVAL, 0);
	}
	shmid = shmget(shmid, N*sizeof(int), IPC_CREAT|0666);
	if(shmid == -1){
		printf("Error in creating shared memory\n");
		destroy();
		exit(2);
	}
	int * buf = (int *)shmat(shmid, NULL, 0);
	if(buf == (int *)-1){
		printf("Error in attaching memory\n");
		destroy();
		exit(3);
	}
	for(i=0; i<N; i++)
		*(buf+i) = THINKING;
	for(i=0; i<N; i++) {
		pid[i] = fork();
		if(pid[i] == 0){
			philosphar(i, buf);
		}
	}
	int * status;
	for(i=0; i<N; i++){
		waitpid(pid[i], &status, 0);
		if(WEXITSTATUS(status) == i) {
			printf("philosphar %d stopped !\n", i);
		}
	}
	destroy();
}