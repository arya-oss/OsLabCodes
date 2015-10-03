#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>

#define INVALID_ARGS 3
#define NO_ARGS 1

using namespace std;

void fullOpt(char path[]) {

}
void showRec(char path[]) {
	dirent ** files; struct stat statbuf;
	cout << "\n" << path << ":\n";
	int n = scandir(path, &files, 0, alphasort);
	char tmp[128], pth[128];
	if(n < 0)
		cout << "Error Ocuured !\n";
	else {
		for(int i=n-1; i>=0; i--) {
			strcpy(pth, path);
			strcat(pth, "/");
			strcat(pth, files[i]->d_name);
			
			if(files[i]->d_name[0] != '.' && stat(pth, &statbuf) != -1) {
   				if (!S_ISDIR(statbuf.st_mode))
   					cout << files[i]->d_name << "  ";
   				else
   					showRec(pth);
   			}
   		}
   	}
	
}
void showFiles(bool all, char path[]){
	dirent ** files;
	int n = scandir(path, &files, 0,alphasort);
	if(n < 0)
		cout << "Error Ocuured !\n";
	else {
		for(int i=0; i<n; i++)
			if(files[i]->d_name[0] != '.' || all)
				cout << files[i]->d_name << "  ";
	}
}
/* Main function starts here */
int main(int argc, char * argv[]) {
	
	char path[128];
	getcwd(path, 128);
	
	if(argc == 2){
		if(strcmp(argv[1], "-a") == 0){
			showFiles(true, path);	
		}
		else if(strcmp(argv[1], "-l") == 0)
			fullOpt(path);
		else if(strcmp(argv[1], "-R") == 0)
			showRec(path);
		else
			showFiles(false, argv[1]);
	} else {
		showFiles(false, path);
	}
	cout << "\n";
	return 0;
}
