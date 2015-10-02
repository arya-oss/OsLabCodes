/**
 *  Multithreaded Merge Sort
 *  @author Rajmani Arya
 *  @date 19-09-2015
 */
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
#include <algorithm>

int c[100000];

struct Array {
	int * a;
	int low, high;
	Array(int * a, int low, int high) {
		this->a = a;
		this->low = low;
		this->high = high;
	}
};
/**
 * @brief This function merges tow sorted part of array
 * 
 * @param a [pointer to given array]
 * @param low [lower index of given array]
 * @param high [higher index of given array]
 * @param mid [index from second sorted begins]
 */
void merge(int a[], int low, int high, int mid){
    int i=low, j=0, k=mid+1;
    while (i <= mid && k <= high){
        if (a[i] <= a[k])
            c[j++] = a[i++];
        else
            c[j++] = a[k++];
    }
    while (i <= mid)
        c[j++] = a[i++];
    while (k <= high)
        c[j++] = a[k++];
    
    for (i = 0; i < j; i++)
        a[low+i] = c[i];
} 
void * ParallelMergeSort(void * array){
	Array * A = (Array *) array;
    int mid; pthread_t lThread, rThread; Array * lArray, * rArray;
    if( A->low >= A->high) return NULL;
    /* create resource */
    mid = (A->low+A->high)/2;
    lArray = new Array(A->a, A->low, mid);
    rArray = new Array(A->a, mid+1, A->high);
    /* Suppose there is no error in creating thread */
    pthread_create(&lThread, NULL, & ParallelMergeSort, (void *) lArray);
    pthread_create(&rThread, NULL, & ParallelMergeSort, (void *) rArray);
    /* Join both Thread */
    pthread_join(lThread, NULL);
    pthread_join(rThread, NULL);
    merge(A->a, A->low, A->high, mid);
    delete lArray;
    delete rArray;
    return NULL;
}
void SimpleMergeSort(int a[], int low, int high) {
    if(low>=high) return;
    int mid = (low+high)/2;
    SimpleMergeSort(a, low, mid);
    SimpleMergeSort(a, mid+1, high);
    merge(a, low, high, mid);
}
/**
 * Main method to test Merge Sort
 */
int main(){
    clock_t t1, t2;
    const int size=50000;
    int * a = new int[size];
    for (int i = size; i >=1 ; --i) {
        a[size-i] = i;
    }
	Array * A = new Array(a, 0, size-1);
	pthread_t pid;
    t1 = clock();
	pthread_create(&pid, NULL, & ParallelMergeSort, (void *) A);
	pthread_join(pid, NULL);
    //simplemergesort(a, 0, size-1);
    t2 = clock();
    float tm = (((float)t2 - (float)t1)/1000000.0F)*1000;
	printf("Time taken %f secs to Complete Sorting.\n", tm);
	return 0;
}