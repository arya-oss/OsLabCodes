#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char * argv[]) {
    DIR *dp;
    struct dirent *dirp;
    FILE *fp;
    if((dp=opendir("/proc/"))==NULL){
        printf("Error occurred\n"); 
        exit(1);
    }
    char path[128];
    printf("   PID  STATE  PPID    PGRP  SESSION  COMM\n");
    while((dirp=readdir(dp))!=NULL){
        strcpy(path,"/proc/");
        char c[6];
        strcpy(c,dirp->d_name);
        if(isdigit(c[0])) {
            //pid_t pid=atoi(c);
            strcat(path,c);
            strcat(path,"/stat");
            if((fp=fopen(path,"r"))!=NULL){
                int pid,ppid,session,pgrp; char state; char comm[20];
                fscanf(fp,"%d %s %c %d %d %d",&pid,comm,&state,&ppid,&pgrp,&session);
                printf("%5d %4c %6d %8d %6d %s\n",pid,state,ppid,pgrp,session,comm);
                fclose(fp);
            }
        }
    } 
    closedir(dp);
    return 0;
}
  