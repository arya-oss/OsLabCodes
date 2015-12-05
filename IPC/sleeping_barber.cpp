/**
 * Problem : IPC Problem Sleeping Barbar
 * Author: Rajmani Arya
 * Usage : g++ sleeping_barbaer.cpp -o sleeping_barber -lpthred
 * Run : ./sleeping_barber
 */
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX 5
#define TNUMS 10 // no. of customers

sem_t barbr,cust;
pthread_mutex_t accessSeats;

void * run_barber (void * args) {
    int * nofs = (int *) args;
    while(true) {
    	pthread_mutex_lock(&accessSeats);
    	if(*nofs == MAX)
    		printf("waiting for customer and Barber has slept.\n");
    	pthread_mutex_unlock(&accessSeats);
    	sem_wait(&cust);
    	pthread_mutex_lock(&accessSeats);
    	*nofs = *nofs+1;
    	sem_post(&barbr);
    	pthread_mutex_unlock(&accessSeats);
    	printf("Barber is cutting hair\n");
    	sleep(3);
    }
}

void * run_costumer (void * args) {
	int * nofs = (int *) args;
    pthread_mutex_lock(&accessSeats);
    if(*nofs > 0){
    	printf("Customer Id %d Just sat down. %d\n", pthread_self(), *nofs);
    	*nofs=*nofs-1;
    	sem_post(&cust);	// notify barber
    	pthread_mutex_unlock(&accessSeats);
    	printf("Customer %d waiting.\n", (int)pthread_self());
    	sem_wait(&barbr);
    	printf("Customer %d is having his hair cut.\n", (int)pthread_self());
    } else {
    	pthread_mutex_unlock(&accessSeats);
    	printf("No seat available Customer %d left saloon.\n", (int)pthread_self());
    }
}

int main() {
    int res; int * args;
    *args = MAX;
    sem_init(&barbr, 0, 0);
    sem_init(&cust, 0, 0);
    pthread_mutex_init(&accessSeats, NULL);
    pthread_t barber, customer[TNUMS];

    res = pthread_create(&barber, NULL, &run_barber, (void *)args);
    if(res){
     	printf("Unable to create thread !\n");
     	exit(EXIT_FAILURE);
    }

    for(int i=0; i<TNUMS; i++){
    	if(pthread_create(&customer[i],NULL, &run_costumer, (void *)args)){
    		printf("Unable to create thread !\n");
    		exit(EXIT_FAILURE);
    	}
    }
    pthread_join(barber, NULL);
    for (int i = 0; i < TNUMS; ++i)
    	pthread_join(customer[i], NULL);
    return 0;
}
