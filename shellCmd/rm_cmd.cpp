#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<string.h>

#define INVALID_ARGS 3
#define NO_ARGS 1

using namespace std;

/* Main function starts here */
int main(int argc, char * argv[]) {
	int res; struct stat st;
	if(argc == 1)
		exit(NO_ARGS);
	if(strcmp(argv[1],"-i") == 0) {
		if(argc == 2)
			exit(INVALID_ARGS);
		else {
			if(stat(argv[2], &st) != -1){
				cout << "Confirm deletion ? (1/0) ";
				cin >> res;
				if(res)
					unlink(argv[2]);
			}
		}
	} else {
		for(int i=1; i<argc; i++){
			res = unlink(argv[i]);
			if(res != 0) cout << "Error ! on " << argv[i] << "\n";
		}
	}
	
	cout << "\n";
	return 0;
}
