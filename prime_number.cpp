#include <pthread.h>
#include <cstdio>
#include <cstdlib>

int N, TNUMS;
int prime[100000], j = 0;

pthread_mutex_t mutex;
pthread_barrier_t barr;

int isPrime(int num){
	if (num==2 || num==3)
		return 1;
	if (num%2==0 || num%3==0 || num == 1)
		return 0;
	int i=5, w=2;
	while (i*i <= num){
		if (num%i == 0)
			return 0;
		i += w;
		w = 6-w;
	}
	return 1;
}
void * run (void * args) {
	int tok = *(int * )args;
	int tmp_arr[10000];
	int k=0,low = tok*(N/TNUMS),high = (tok+1)*(N/TNUMS);
	pthread_barrier_wait(&barr);
	for (int i = low+1; i <= high; ++i){
		if(isPrime(i))
			tmp_arr[k++] = i;
		//printf("R\n");
	}
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < k; ++i){
		prime[j++] = tmp_arr[i];
	}
	pthread_mutex_unlock(&mutex);
}

int A[] = {	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
			17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,
			34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
			51,52,53,54,55,56,57,58,59,60,61,62,63,64};

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&mutex, NULL);
	pthread_barrier_init(&barr, NULL, TNUMS);
	if(argc < 3) {
		printf("Usage : ./a.out <THREAD_COUNT 64> <RANGE THREAD_COUNT>\n");
		exit(1);
	}
	TNUMS = atoi(argv[1]);
	N = atoi(argv[2]);
	printf("prime no. between 1 and %d\n", N);
	
	pthread_t pid[TNUMS];
	for (int i = 1; i < TNUMS; ++i){
		pthread_create(&pid[i], NULL, &run, &A[i]);
	}
	
	run(&A[0]);
	for (int i = 1; i < TNUMS; ++i){
		pthread_join(pid[i], NULL);
	}
	for (int i = 0; i < j; ++i){
		printf("%d \n", prime[i]);
	}
	return 0;
}
