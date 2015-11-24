#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


/*
 author - Aneesh Makala
 m - no of resources
 n -  no of processes
*/

#define m 3
#define n 5


int * AVAILABLE;
int ** ALLOCATION;
int ** NEED;
int * WORK ;
int ** MAX;


int copyFromTo(int *AVAILABLE, int *WORK){
	int i;
	for(i = 0; i<m; i++){
		WORK[i] = AVAILABLE[i];
	}
}

int find(int *finish, int * WORK, int **NEED){
	int i;
	for(i =0; i <n; i++){
		if(!finish[i]){
			int j;
			int smaller = 1;
			for (j = 0; j< m;j++){
				 if(NEED[i][j] > WORK[j]){
					smaller = 0;
					break;
				}
			}
			if(smaller){
				return i ;
			}
		}
	}
	return -1;
}
				

int allFinished(int *finish){
	int i;
	for(i =0; i<n;i++){
		if(!finish[i])
			return 0;
	}
	return 1;
}


int safety(){
	int i , j;
	copyFromTo(AVAILABLE,WORK);
	int *finish = (int *) malloc(sizeof(int ) * n);
	
	while((i = find(finish, WORK, NEED )) != -1){
		for(j = 0;j<m;j++){
			WORK[j] += ALLOCATION[i][j];
		}
		finish[i] = 1;
		printf("%d just completed\n", i);
		if( allFinished(finish))
			break;
	
	}

	if(!allFinished(finish)){
		printf("unsafe");
		return 0;
	}
	else{
		printf("safe");
		return 1;
	}

}

int request(int process_no, int *REQUEST){
	int i,j;
	int error = 0;
	for(i = 0; i <m;i++){
		if(REQUEST[i]>NEED[process_no][i]|| REQUEST[i]>AVAILABLE[i]){
			error = 1;
		}
	}
	if (error){
		printf(" error");
		return 0;
	}
	for(i = 0;i<m;i++){
		AVAILABLE[i] -= REQUEST[i];
		ALLOCATION[process_no][i] += REQUEST[i];
		NEED[process_no][i] -= REQUEST[i];
	}

	if(!safety()){
		
		for(i = 0;i<m;i++){
			AVAILABLE[i] += REQUEST[i];
			ALLOCATION[process_no][i] -= REQUEST[i];
			NEED[process_no][i] += REQUEST[i];
		}
	}		
}


int main(){
	int i, j;
	AVAILABLE = (int *) malloc(sizeof(int) * m);
	ALLOCATION = (int**) malloc(sizeof(int*)*n);
	WORK = (int*) malloc(sizeof(int)*m);
	MAX = (int**) malloc(sizeof(int*)*n);

	NEED = (int**) malloc(sizeof(int*)*n);
	for(i = 0; i<n; i++){
		ALLOCATION[i] = (int*) malloc(sizeof(int) *m);	
		MAX[i] = (int*) malloc(sizeof(int) *m);
		NEED[i] = (int*) malloc(sizeof(int) *m);
		//WORK[i] = (int*) malloc(sizeof(int) *m);	
	}

	for(i= 0; i <m; i++){
		scanf("%d", &AVAILABLE[i]);
	}

	for(i= 0; i <m; i++){
		printf("%d", AVAILABLE[i]);
	}

	for (i = 0;i<n;i++){
		printf("for process %d, enter allocation\n", i);
		for(j = 0; j<m;j++){
			scanf("%d",&ALLOCATION[i][j]);
		}
	
		printf("for process %d, enter max\n", i );
		for(j = 0; j<m;j++){
			scanf("%d",&MAX[i][j]);
		}
		for(j = 0;j<m; j++){
			NEED[i][j] = MAX[i][j] -ALLOCATION[i][j];
		}
	}

	for (i = 0;i<n;i++){
	//	printf("for process %d, enter allocation\n");
		for(j = 0; j<m;j++){
			printf("%d, ",ALLOCATION[i][j]);
		}
		printf("\t");
	//	printf("for process %d, enter max\n");
		for(j = 0; j<m;j++){
			printf("%d, ",MAX[i][j]);
		}
		printf("\t");
		for(j = 0;j<m; j++){
			//NEED[i][j] = MAX[i][j] -ALLOCATION[i][j];
			printf("%d, ",NEED[i][j]);
		}
		printf("\n");
	}
	safety();
	int arr[3] = {0,2,0};
	request(0,arr);
	return 0;
}