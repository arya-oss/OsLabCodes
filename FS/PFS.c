/**
 * Title: Simple File System Implementation using File
 * Author: Rajmani Arya
 * Date: 10-Jun-2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// #define DEBUG

#define DISK_SIZE 10240
#define BLOCK_SIZE 256
#define BLOCK_COUNT 40
#define MAXFILENAME 20
#define INODE_OFFSET 256
#define DATA_OFFSET 4096
#define MAXFILECOUNT 15
#define FILES_PER_DIR 10
#define MAX_DATA_BLOCK 24

/**
 * INode is also known as File Control Block
 */ 

struct SuperBlock {
    char sb_name[MAXFILENAME];
    unsigned int sb_inode_off;
    unsigned int sb_data_off;
    unsigned int sb_file_cnt;
    unsigned int sb_nfreeino;
    unsigned int sb_nfreeblk;
};

struct DirEntry {
    char d_name[MAXFILENAME];
    int d_nfile;
    char files[FILES_PER_DIR][MAXFILENAME];
};

struct INode {
    char i_type;                        // '0' for dir and '1' for regular file
    char i_name[MAXFILENAME];           // File or Directory Name
    int             i_parent;           // Parent INode
    unsigned int    i_size;             // file size
    unsigned long   i_mtime;            // modified time
    unsigned int    i_blks[20];         // pointer to data
    char i_remarks[MAXFILENAME];		// remark for directory
};

struct SuperBlock global_sb;
FILE * global_f;
int active = 0;

int bitmap_data[MAX_DATA_BLOCK];

int MkFs(char * disk_name);
void listDir(char * dirpath);
int OpenDevice(char * disk_name);
int KillDevice(char * disk_name);

int get_inode_from_name(char * name);
int putfile(char * path, char * filename);
int getfile(char * filename);
int removeFile(char * filename);
int putRemark(char * filename, char * remarks);

int get_next_free_inode ();
int get_next_free_data_block();

/**
 * Split input command with delimiter whitespace
 * @param input_cmd Command Line Input (CLI) for parsing args.
 * @param args string array to store arguments
 * @return number of arguments
 */

int parse_args (char * input_cmd, char args[3][32]) {
    int nargs = 0;
    char * pch;
    pch = strtok(input_cmd, " ");
    while (pch != NULL) {
        strcpy(args[nargs], pch);
        pch = strtok(NULL, " ");
        nargs ++;
    }
    return nargs;
}

/**
 * Remove begining and trailing whitespace from input_cmd
 * @param str raw input command str
 */

void strtrim (char * str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace((unsigned char) str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char) str[end]))
        end--;

    // Shift all characters back to the start of the string array.
    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0'; // Null terminate string.
}

/**
 * Format time into human readable format
 * @param tm timestamp long value
 * @param ctm character string to store formatted time
 * @param size max size of character array
 */
void format_time(long tm, char * ctm, unsigned int size) {
	time_t tt = tm;
    strftime(ctm, size, "%I:%M %p\t %B %d", localtime(&tt));
}

int main(int argc, char * argv[])
{
    char input_cmd[128]; char args[3][32];
    do {
        printf("PFS> ");
        fgets(input_cmd, 128, stdin);
        strtrim(input_cmd);
        parse_args(input_cmd, args);
        if (strcmp(args[0], "open") == 0)
        {
            OpenDevice(args[1]);
        }
        else if (strcmp(args[0], "dir") == 0)
        {
            listDir("/");
        }
        else if (strcmp(args[0], "put") == 0)
        {
           if (putfile(args[1], args[2]) == -1) {
                printf("%s\n", "File writing error !");
           }
        }
        else if (strcmp(args[0], "get") == 0)
        {
            getfile(args[1]);
        }
        else if (strcmp(args[0], "rm") == 0)
        {
            removeFile(args[1]);
        }
        else if (strcmp(args[0], "putr") == 0)
        {
            putRemark(args[1], args[2]);
        }
        else if (strcmp(args[0], "kill") == 0)
        {
            KillDevice(args[1]);
        }
        else if (strcmp(args[0], "quit") == 0)
        {
            continue;
        }
        else
        {
            printf("%s\n", "Unknown Command !");
        }
    } while (strcmp(input_cmd, "quit") != 0);

    return 0;
}

int MkFs(char * disk_name) {
    int i;
    FILE * file = fopen(disk_name, "w");
    if (!file) {
        printf("Error !! Opening file %s\n", disk_name);
        return -1;
    }
    char buffer[BLOCK_SIZE];
    memset(buffer, 0, BLOCK_SIZE);
    for (i = 0; i < BLOCK_COUNT; i++) {
        fwrite(buffer, 1, BLOCK_SIZE, file);
    }
    #ifdef DEBUG
    fprintf(stderr, "%s Disk Created ! of %d bytes size\n", disk_name, DISK_SIZE);
    #endif // DEBUG
    fseek(file, 0, SEEK_SET);
    strcpy(global_sb.sb_name, disk_name);
    global_sb.sb_inode_off = INODE_OFFSET;
    global_sb.sb_data_off = DATA_OFFSET;
    global_sb.sb_nfreeino = MAXFILECOUNT - 1;
    global_sb.sb_nfreeblk = BLOCK_COUNT - 1 - MAXFILECOUNT - 1;
    fwrite(&global_sb, sizeof(struct SuperBlock), 1, file);
    #ifdef DEBUG
    fprintf(stderr, "Super Block Created !!");
    #endif // DEBUG
    // Create Root Dir (INode block)
    fseek(file, global_sb.sb_inode_off, SEEK_SET);
    struct INode * root_dir = (struct INode *)malloc(sizeof(struct INode));
    root_dir->i_type = '0'; // Root Directory
    root_dir->i_mtime = time(NULL);
    strcpy(root_dir->i_name, "/");
    root_dir->i_parent = 0;
    root_dir->i_blks[0] = 1;
    for (i=1; i < 20; i++) {
        root_dir->i_blks[i] = 0;
    }
    root_dir->i_size = 0;
    fwrite(root_dir, sizeof(struct INode), 1, file);
    #ifdef DEBUG
    fprintf(stderr, "Root Inode Written!\n");
    #endif // DEBUG
    // Create Data Block Entry (. && ..)
    bitmap_data[0] = 1;
    fseek(file, global_sb.sb_data_off, SEEK_SET);
    struct DirEntry * dir_disk = (struct DirEntry *)malloc(sizeof(struct DirEntry));
    strcpy(dir_disk->d_name,"/");
    dir_disk->d_nfile = 0;
    fwrite(dir_disk, sizeof(struct DirEntry), 1, file);
    #ifdef DEBUG
    fprintf(stderr, "Root DIR entry written on disk block");
    #endif // DEBUG
    fclose(file);
    #ifdef DEBUG
    fprintf(stderr, "Portable FileSystem Created !");
    #endif // DEBUG
    return 1;
}

int OpenDevice(char * disk_name) {
    if (active == 1) return 1;
    MkFs(disk_name);
    global_f = fopen(disk_name, "ra+");
    fseek (global_f, 0L, SEEK_SET);
    fread (&global_sb, 1, sizeof(struct SuperBlock), global_f);
    active = 1;
    int i;
    bitmap_data[0] = 1;
    for (i=1; i < MAX_DATA_BLOCK; i++) {
    	bitmap_data[i] = 0;
	}
    return 1;
}

int KillDevice(char * disk_name) {
    fclose(global_f);
    return remove(disk_name);
}

int get_inode_from_name(char * name) {
    struct INode ino;
    fseek(global_f, global_sb.sb_inode_off, SEEK_SET);
    int i=1;
    while ( i <= MAXFILECOUNT) {
        fread(&ino, 1, sizeof(struct INode), global_f);
        fseek(global_f, global_sb.sb_inode_off + i * BLOCK_SIZE, SEEK_SET);
        //printf("%c %ld %s\n", ino.i_type, ino.i_mtime, ino.i_name);
        if (ino.i_mtime != 0 && strcmp(name, ino.i_name) == 0) {
            return i;
        }
        i++;
    }
    return -1;
}

void listDir(char * dirpath) {
    int parent; char ctm[64]; struct INode ino;
    // parent = get_inode_from_name(dirpath);
    // if (parent == -1) {
    //     printf("%s\n", "Directory does not exit!");
    //     return;
    // }
    parent = 1; // root dir inode
    fseek(global_f, global_sb.sb_inode_off, SEEK_SET);
    int i=1;
    while ( i <= MAXFILECOUNT) {
        fread(&ino, 1, sizeof(struct INode), global_f);
        fseek(global_f, global_sb.sb_inode_off + i * BLOCK_SIZE, SEEK_SET);
        // printf("%d %s\n", ino.i_size, ino.i_name);
        if (ino.i_mtime != 0 && ino.i_parent == parent) {
        	time_t tt = ino.i_mtime;
            strftime(ctm, 64, "%I:%M %p  %B %d", localtime(&tt));
            printf("%s  %d bytes   %s\n", ino.i_name, ino.i_size, ctm);
        }
        i++;
    }
}

int get_next_free_inode () {
    struct INode ino;
    fseek(global_f, global_sb.sb_inode_off + BLOCK_SIZE, SEEK_SET);
    int i=1;

    while ( i <= MAXFILECOUNT) {
        fread(&ino, 1, sizeof(struct INode), global_f);
        fseek(global_f, global_sb.sb_inode_off + (i+1) * BLOCK_SIZE, SEEK_SET);
        if (ino.i_mtime == 0) {
            return i;
        }
        i++;
    }
    return -1;
}

int get_next_free_data_block() {
    int i=1;
    while (i <= MAX_DATA_BLOCK) {
    	if (bitmap_data[i] == 0)
    		return i;
    	i++;
	}
	return -1;
}

int putfile(char * path, char * filename) {
    int parent, data_addr, i,j,k;
    parent = get_inode_from_name(filename);
    // printf("Parent INode %d\n", parent);
    if (parent != -1) {
        printf("File %s exists!\n", filename);
        return -1;
    }
    FILE * win_file = fopen(path, "r");
    fseek(win_file, 0, SEEK_END);
    long fsize = ftell(win_file);
    fseek(win_file, 0, SEEK_SET);
    char * buffer = malloc(fsize + 1);
    fread(buffer, fsize, 1, win_file);
    fclose(win_file);
    buffer[fsize] = '\0';
    // Add a new INode ! Done
    int offset = get_next_free_inode();
	// printf("Free INode %d\n", offset);
    if (offset == -1) {
        printf("%s\n", "No Inode left !!");
        return -1;
    }
    struct INode * new_file = (struct INode *)malloc(sizeof(struct INode));
    new_file->i_type = '1';
    new_file->i_mtime = time(NULL);
    strcpy(new_file->i_name, filename);
    new_file->i_parent = 1; // parent is root dir (path = "/")
    new_file->i_size = fsize;
    int nblks;
    if (fsize % BLOCK_SIZE == 0)
    	nblks = fsize/BLOCK_SIZE;
    else
    	nblks = 1 + fsize/BLOCK_SIZE;
    if (global_sb.sb_nfreeblk < nblks) {
        printf("%s\n", "Large file size cannot be written!!");
        return -1;
    }
    global_sb.sb_nfreeblk += nblks;
    global_sb.sb_nfreeino += 1;
    // printf("Blocks = %d\n", nblks);
    char * buf = malloc(BLOCK_SIZE+1);
    for (i=0; i < nblks; i++) {
        data_addr = get_next_free_data_block();
        // printf("%d\n", data_addr);
        if (data_addr == -1) {
            printf("%s\n", "Free Block return -1");
            return -1;
        }
        k = 0;
        for (j=i*BLOCK_SIZE; j < (i+1)*BLOCK_SIZE && j < fsize; j++) {
            buf[k++] = buffer[j];
        }
        buf[k] = '\0';
        fseek(global_f, global_sb.sb_data_off + data_addr * BLOCK_SIZE, SEEK_SET);
        // printf("W On %ld\n", ftell(global_f));
        if (i == nblks-1) {
        	// printf ("Written %d bytes.\n", fsize);
        	fwrite(buf, 1, fsize % BLOCK_SIZE, global_f);
		} else
        	fwrite(buf, 1, BLOCK_SIZE, global_f);
        bitmap_data[data_addr] = 1;
        new_file->i_blks[i] = data_addr;
    }
    fseek(global_f, global_sb.sb_inode_off + offset * BLOCK_SIZE, SEEK_SET);
    fwrite(new_file, sizeof(struct INode), 1, global_f);
    #ifdef DEBUG
    fprintf(stderr, "New file created and written!\n");
    #endif // DEBUG
    
    // Add entry to Directory Inode
    fseek(global_f, global_sb.sb_data_off, SEEK_SET);
    struct DirEntry root_dir;
    fread(&root_dir, 1, sizeof(struct DirEntry), global_f);
    strcpy(root_dir.files[root_dir.d_nfile], filename);
    root_dir.d_nfile += 1;
    fseek(global_f, global_sb.sb_data_off, SEEK_SET);
    fwrite(&root_dir, sizeof(struct DirEntry), 1, global_f);
    free(buffer);
    free(buf);
    return 1;
}

int getfile(char * filename) {
	int ino = get_inode_from_name(filename)-1; int nblks;
	// printf("INode = %d\n", ino);
	if (ino == -2) {
		printf("%s\n", "File not found !");
		return -1;
	}
	
	int i,j,x,k=0;
	fseek(global_f, global_sb.sb_inode_off + ino * BLOCK_SIZE, SEEK_SET);
	struct INode inode;
	
	FILE * file = fopen(filename, "w");
	fread(&inode, 1, sizeof(struct INode), global_f);
	if (inode.i_size % BLOCK_SIZE == 0)
    	nblks = inode.i_size/BLOCK_SIZE;
    else
    	nblks = 1 + inode.i_size/BLOCK_SIZE;
    	
	char * buf = malloc(BLOCK_SIZE+1);
	// printf("Blocks = %d\n", nblks);
	for (i=0; i < nblks; i++) {
		// printf("Data Block addr = %d\n", inode.i_blks[i]);
		fseek(global_f, global_sb.sb_data_off + inode.i_blks[i] * BLOCK_SIZE, SEEK_SET);
		// printf("W On %ld\n", ftell(global_f));
		if ( i == nblks-1) 
			x = fread(buf, 1, inode.i_size % BLOCK_SIZE, global_f);
		else
			x = fread(buf, 1, BLOCK_SIZE, global_f);
		buf[x] = '\0';
		// printf("%s %d\n", buf, x);
		fwrite(buf, x, 1, file);;
	}
	fclose(file);
	free(buf);
	return 1;
}

int removeFile(char * filename) {
	int ino = get_inode_from_name(filename)-1; int nblks,i;
	// printf("INode = %d\n", ino);
	if (ino == -2) {
		printf("%s\n", "File not found !");
		return -1;
	}
	char * buffer = malloc(BLOCK_SIZE);
	memset(buffer, 0, BLOCK_SIZE);
	fseek(global_f, global_sb.sb_inode_off + ino * BLOCK_SIZE, SEEK_SET);
	struct INode inode;
	fread(&inode, 1, sizeof(struct INode), global_f);
	fseek(global_f, -BLOCK_SIZE, SEEK_CUR);
	fwrite(buffer, 1, BLOCK_SIZE, global_f);
	if (inode.i_size % BLOCK_SIZE == 0)
    	nblks = inode.i_size/BLOCK_SIZE;
    else
    	nblks = 1 + inode.i_size/BLOCK_SIZE;
    global_sb.sb_nfreeblk -= nblks;
    global_sb.sb_nfreeino -= 1;
    for (i=0; i < nblks; i++) {
    	bitmap_data[i] = 0;
	}
	free(buffer);
	return 1;
}

int putRemark(char * filename, char * remarks) {
	int ino = get_inode_from_name(filename)-1; int nblks,i;
	// printf("INode = %d\n", ino);
	if (ino == -2) {
		printf("%s\n", "File not found !");
		return -1;
	}
	fseek(global_f, global_sb.sb_inode_off + ino * BLOCK_SIZE, SEEK_SET);
	struct INode inode;
	fread(&inode, 1, sizeof(struct INode), global_f);
	strcpy(inode.i_remarks, remarks);
	fseek(global_f, global_sb.sb_inode_off + ino * BLOCK_SIZE, SEEK_SET);
	fwrite(&inode, sizeof(struct INode), 1, global_f);
	return 1;
}
