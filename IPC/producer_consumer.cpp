#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mu;
pthread_cond_t cnd;

struct Resource {
  int n;
  int valueSet;
  Resource(){
    valueSet=false;
    n = 0;
  }
  int get(){
    pthread_mutex_lock(&mu);
    while(!valueSet){
      printf("consumer waiting\n");
      pthread_cond_wait(&cnd, &mu);
    }
    printf("consumed : %d\n", n);
    valueSet = false;
    pthread_cond_signal(&cnd);
    pthread_mutex_unlock(&mu);
    return n;
 }
 void put(int n){
    pthread_mutex_lock(&mu);
    while(valueSet){
      printf("producer waiting\n");
      pthread_cond_wait(&cnd, &mu);
    }
    this->n = n;
    printf("produced : %d\n", n);
    valueSet = true;
    pthread_cond_signal(&cnd);
    pthread_mutex_unlock(&mu);
  }
};

void * run_prod (void * args) {
    struct Resource * obj = (Resource *) args;
    int i=0;
    while(i < 10){
     obj->put(i++);
    }
}

void * run_cons (void * args) {
    struct Resource * obj = (Resource *) args;
    int i=0;
    while(i < 10){
     obj->get();
     i++;
    }
}

int main() {
    int res;

    Resource * r = new Resource();

    pthread_mutex_init(&mu, NULL);
    pthread_cond_init(&cnd, NULL);
    pthread_t prod_t, cons_t;

    res = pthread_create(&cons_t, NULL, &run_cons, (void *)r);
    if(res){
     printf("Unable to create thread !\n");
     exit(EXIT_FAILURE);
    }
    res = pthread_create(&prod_t, NULL, &run_prod, (void *)r);
    if(res){
     printf("Unable to create thread !\n");
     exit(EXIT_FAILURE);
    }
    pthread_join(cons_t, NULL);
    pthread_join(prod_t, NULL);
    return 0;
}
