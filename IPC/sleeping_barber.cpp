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

#define TNUMS 10 // no. of customers

sem_t barbr,cust;
pthread_mutex_t accessSeats;

struct Resource {
	int numOfFreeSeats;
	Resource(int seats) {
		numOfFreeSeats = seats;
	}
};

void * run_barber (void * args) {
    struct Resource * obj = (Resource *) args;
    while(true) {
    	pthread_mutex_lock(&accessSeats);
    	if(obj->numOfFreeSeats == 5)
    		printf("waiting for customer and Barber has slept.\n");
    	pthread_mutex_unlock(&accessSeats);
    	sem_wait(&cust);
    	pthread_mutex_lock(&accessSeats);
    	obj->numOfFreeSeats++;
    	sem_post(&barbr);
    	pthread_mutex_unlock(&accessSeats);
    	printf("Barber is cutting hair\n");
    	sleep(3);
    }
}

void * run_costumer (void * args) {
    struct Resource * obj = (Resource *) args;
    bool notCut = true;
    while(notCut) {
    	pthread_mutex_lock(&accessSeats);
    	if(obj->numOfFreeSeats > 0){
    		printf("Customer Id %d Just sat down. %d\n", pthread_self(), obj->numOfFreeSeats);
    		obj->numOfFreeSeats--;
    		sem_post(&cust);	// notify barber
    		pthread_mutex_unlock(&accessSeats);
    		printf("Customer %d waiting.\n", pthread_self());
    		sem_wait(&barbr);
    		notCut = false;
    		printf("Customer %d is having his hair cut.\n", pthread_self());
    	} else {
    		pthread_mutex_unlock(&accessSeats);
    		printf("No seat available Customer %d left saloon.\n", pthread_self());
    		notCut = false;
    	}
    }
    
}

int main() {
    int res;
    Resource * r = new Resource(5);
    sem_init(&barbr, 0, 0);
    sem_init(&cust, 0, 0);
    pthread_mutex_init(&accessSeats, NULL);
    pthread_t barber, customer[TNUMS];

    res = pthread_create(&barber, NULL, &run_barber, (void *)r);
    if(res){
     	printf("Unable to create thread !\n");
     	exit(EXIT_FAILURE);
    }

    for(int i=0; i<TNUMS; i++){
    	sleep(1);
    	if(pthread_create(&customer[i],NULL, &run_costumer, (void *) r)){
    		printf("Unable to create thread !\n");
    		exit(EXIT_FAILURE);
    	}
    }
    pthread_join(barber, NULL);
    for (int i = 0; i < TNUMS; ++i)
    	pthread_join(customer[i], NULL);
    return 0;
}
