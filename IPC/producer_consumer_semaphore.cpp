#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#define MAX 10

sem_t full, empty, mutex;

int buf[MAX];

void * producer(void * args) {
    int in=0;
    for(int i=0; i<20 ; i++) {
        sem_wait(&empty); // decrement emty count
        sem_wait(&mutex); // enter critical region
        buf[in] = i;  // put new item in buffer
        in =(in+1)%MAX;
        printf("producer produced item %d\n", i);
        sem_post(&mutex); // leave critical region
        sem_post(&full);  // increment count of full slots
    }
}

void * consumer(void * args) {
    int out=0;
    for(int i=0; i<20 ; i++) {
        sem_wait(&full);  // decrement full count
        sem_wait(&mutex); // enter critical section
        printf("consumer consumed item %d\n", buf[out]);
        out = (out+1)%10;
        sem_post(&mutex); // leave critical section
        sem_post(&empty); // increment count of empty slots
    }
}


int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, MAX);
    pthread_t cons, prod;
    pthread_create(&prod, NULL, &producer, NULL);
    pthread_create(&cons, NULL, &consumer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}
