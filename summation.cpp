/**
 *  Summation of N numbers
 *  @author Rajmani Arya
 *  @date 19-09-2015
 */
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <algorithm>

#define N 1000000000
#define NUMTHREADS 10

pthread_barrier_t barr;
pthread_mutex_t mut;
long long int sum = 0;

float timeDiff(clock_t t1, clock_t t2) {
    float tm = (((float)t2 - (float)t1)/1000000.0F)*1000;
    return tm;
}

void * run (void * args) {
    pthread_barrier_wait(&barr);
    int s = *(int *) args;
    int from = (s*N)/NUMTHREADS;
    int to = ((s+1)*N)/NUMTHREADS;
    pthread_mutex_lock(&mut);
    printf("computing slice %d\n", s);
    pthread_mutex_unlock(&mut);

    long long int tmpSum = 0;
    for (int i = from+1; i <= to; ++i){
        tmpSum += i;
    }
    //pthread_mutex_lock(&mut);
    sum += tmpSum;
    //pthread_mutex_unlock(&mut);
}
int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int main(){
    clock_t t1, t2;
    pthread_barrier_init(&barr, NULL, NUMTHREADS);
    pthread_mutex_init(&mut, NULL);
	pthread_t pid[NUMTHREADS];
    t1 = clock();
    for (int i = 1; i < NUMTHREADS; ++i) {
        pthread_create(&pid[i], NULL, &run, (void *) &a[i]);
    }
    run(&a[0]);
    for (int i = 1; i < NUMTHREADS; ++i) {
        pthread_join(pid[i], NULL);
    }
    t2 = clock();
    printf("time taken to compute %f\n", timeDiff(t1, t2));
    printf("Sum is %lld\n", sum);
    sum =0;
    t1 = clock();
    for (int i = 1; i <= N ; ++i){
    	sum += i;
    }
    t2 = clock();
    printf("time taken to compute %f\n", timeDiff(t1, t2));
    printf("Sum is %lld\n", sum);
    pthread_barrier_destroy(&barr);
	return 0;
}