#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>

void print_perms(mode_t st) {
    char perms[11];
    if(st && S_ISREG(st)) perms[0]='-';
    else if(st && S_ISDIR(st)) perms[0]='d';
    else if(st && S_ISFIFO(st)) perms[0]='|';
    else if(st && S_ISSOCK(st)) perms[0]='s';
    else if(st && S_ISCHR(st)) perms[0]='c';
    else if(st && S_ISBLK(st)) perms[0]='b';
    else perms[0]='l';  // S_ISLNK
    perms[1] = (st && S_IRUSR) ? 'r':'-';
    perms[2] = (st && S_IWUSR) ? 'w':'-';
    perms[3] = (st && S_IXUSR) ? 'x':'-';
    perms[4] = (st && S_IRGRP) ? 'r':'-';
    perms[5] = (st && S_IWGRP) ? 'w':'-';
    perms[6] = (st && S_IXGRP) ? 'x':'-';
    perms[7] = (st && S_IROTH) ? 'r':'-';
    perms[8] = (st && S_IWOTH) ? 'w':'-';
    perms[9] = (st && S_IXOTH) ? 'x':'-';
    perms[10] = '\0';
    printf("%s", perms);
}

void ls(char path[], int flag) {
    DIR * dir; dirent * file;
    dir = opendir(path);
    while(file=readdir(dir)) {
        if(file->d_name[0] != '.' || flag) {
            printf("%s  ", file->d_name);
        }
    }
    free(file);
    free(dir);
}
void ls_l(char path[]) {
    DIR * dir; dirent * file; struct stat sbuf;
    char buf[128];
    struct passwd pwent, * pwentp;
    struct group grp, * grpt;
    char datestring[256];
    struct tm time;
    dir = opendir(path);
    while(file=readdir(dir)) {
        stat(file->d_name, &sbuf);
        print_perms(sbuf.st_mode);
        printf(" %d", (int)sbuf.st_nlink);
        if (!getpwuid_r(sbuf.st_uid, &pwent, buf, sizeof(buf), &pwentp))
            printf(" %s", pwent.pw_name);
        else
            printf(" %d", sbuf.st_uid);

        if (!getgrgid_r (sbuf.st_gid, &grp, buf, sizeof(buf), &grpt))
            printf(" %s", grp.gr_name);
        else
            printf(" %d", sbuf.st_gid);
        printf(" %5d", (int)sbuf.st_size);
        
        localtime_r(&sbuf.st_mtime, &time);
        /* Get localized date string. */
        strftime(datestring, sizeof(datestring), "%F %T", &time);

        printf(" %s %s\n", datestring, file->d_name);
    }
}
void ls_r(char path[]) {
    DIR * dir; dirent * file; struct stat sbuf;
    char tmp[128];
    dir = opendir(path);
    while(file=readdir(dir)) {
        if(file->d_name[0] == '.') continue;
        strcpy(tmp, path);
        strcat(tmp, "/");
        strcat(tmp, file->d_name);
        //printf("%s\n", tmp);
        stat(tmp, &sbuf);
        if(sbuf.st_mode && S_ISDIR(sbuf.st_mode)) {
            printf("\n%s/\n", tmp);
            ls_r(tmp);
        } else 
            printf("%s  ", file->d_name);
    }
}
int main(int argc, char * argv[]) {
    char pathname[128];
    getcwd(pathname, 128);
    if(argc == 1){
        ls(pathname, 0);
    } else {
        if(strcmp(argv[1], "-l") == 0) {
            ls_l(pathname);
        } else if(strcmp(argv[1], "-r") == 0) {
            ls_r(pathname);
        } else {
            ls(pathname, 1);  // ls -a
        }
    }
    printf("\n");
    return 0;
}
