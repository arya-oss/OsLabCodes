#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
 
#define N 5
enum state { THINKING, HUNGRY, EATING};
 
sem_t mutex;
sem_t chops[N];

state pState[N];

void test(int p) {
    if (pState[p] == HUNGRY && pState[(p+4)%N] != EATING && pState[(p+1)%N] != EATING) {
        printf("Philosopher %d takes fork %d and %d\n",p ,(p+4)%N, p);
        pState[p] = EATING;
        printf("Philosopher %d is Eating\n", p);
        sem_post(&chops[p]);
    }
}
 
void pick_fork(int p) {
    sem_wait(&mutex);
    pState[p] = HUNGRY;
    printf("Philosopher %d is Hungry\n",p);
    test(p);
    sem_post(&mutex);
    sem_wait(&chops[p]);
    sleep(1);
}
 
void put_fork(int p) {
    sem_wait(&mutex);
    printf("Philosopher %d putting fork %d and %d down.\n", p, (p+4)%N, p);
    pState[p] = THINKING;
    printf("Philosopher %d is thinking\n", p);
    test((p+4)%N);
    test((p+1)%N);
    sem_post(&mutex);
    sleep(1);
}


void * philospher(void * args) {
	int * i = (int *) args;
    while(true) {
        pick_fork(*i);
        sleep(2);
        put_fork(*i);
    }
}

int main(int argc,char * argv[]) {

    pthread_t pid[N];
    int ph_id[] = {0, 1, 2, 3, 4};

    sem_init(&mutex,0,1);

    for(int i=0;i<N;i++)
        sem_init(&chops[i], 0, 0);
    for(int i=0;i<N;i++) {
        pthread_create(&pid[i],NULL, &philospher, (void *) &ph_id[i]);
    }
    for(int i=0;i<N;i++)
        pthread_join(pid[i],NULL);

    return 0;
}