#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char * argv[]) {
    if(argc == 1){
        printf("Usage : ./a.out filename1 filename2 ...\n");
        exit(1);
    }
    char path[128], tmp[128]; int n, i, j;
    getcwd(path, 128);
    for(i=1; i<argc; i++) {
        strcpy(tmp, path);
        strcat(tmp, "/");
        strcat(tmp, argv[i]);
        int fd = open(tmp, O_RDONLY);
        if(fd == -1) {
            printf("File Doesn't exist %s\n", argv[i]);
        } else {
            while(n = read(fd, tmp, 127)){
                if(n != 127){
                    for(j=0; j<n; j++)
                        printf("%c", tmp[j]);
                }
                else
                    printf("%s", tmp);
            }
            close(fd);
            printf("\n\n");
        }
    }
    return 0;
}
