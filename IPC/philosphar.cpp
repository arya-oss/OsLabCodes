#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define nPhilos 5

sem_t s[nPhilos];

enum state { THINKING, HUNGRY, EATING };

void * run(void * args) {
	int * r = (int *) args;
	printf("Philosphar %d Thinking.\n", *r);
	sem_wait(&s[*r]);
	printf("Philosphar %d became hungry.\n", *r);
	sem_wait(&s[(*r+1)%nPhilos]);
	printf("Philosphar %d start eating.\n", *r);
	sleep(2);
	sem_post(&s[(*r+1)%nPhilos]);
	sem_post(&s[*r]);
}

int main() {
	pthread_t p[nPhilos];
	for (int i = 0; i < nPhilos; ++i){
		sem_init(&s[i], 0, 1);
	}
	for (int i = 0; i < nPhilos; ++i) {
		if(pthread_create(&p[i], NULL, & run, (void *) &i) != 0){
			printf("Error !\n");
			exit(1);
		}
	}
	for (int i = 0; i < nPhilos; ++i){
		pthread_join(p[i],NULL);
	}
	return 0;
}
