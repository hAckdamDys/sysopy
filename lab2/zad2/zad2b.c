#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


long int bytesMax=0;
static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	if(tflag != FTW_F){//jak nie regular
		return 0;
	}
	if(sb->st_size>bytesMax){//jak za duzy
		return 0;
	}
	printf("%s\n",realpath(fpath, NULL));
	printf("File size: %li\n",sb->st_size);
	printf("File Permissions: \t");
	printf( (S_ISDIR(sb->st_mode)) ? "d" : "-");
	printf( (sb->st_mode & S_IRUSR) ? "r" : "-");
	printf( (sb->st_mode & S_IWUSR) ? "w" : "-");
	printf( (sb->st_mode & S_IXUSR) ? "x" : "-");
	printf( (sb->st_mode & S_IRGRP) ? "r" : "-");
	printf( (sb->st_mode & S_IWGRP) ? "w" : "-");
	printf( (sb->st_mode & S_IXGRP) ? "x" : "-");
	printf( (sb->st_mode & S_IROTH) ? "r" : "-");
	printf( (sb->st_mode & S_IWOTH) ? "w" : "-");
	printf( (sb->st_mode & S_IXOTH) ? "x" : "-");
	printf("\nLast file modification:   %s\n", ctime(&sb->st_mtime));

    return 0;           /* To tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
	if(argc!=3){
		printf("Podaj 2 argumenty; sciezke i bajty\n");
		return 1;
	}
	char* dirName = argv[1];
	bytesMax = atoi(argv[2]);
	int flags = 0;
	flags |= FTW_DEPTH;
	flags |= FTW_PHYS;
	if (nftw(dirName, display_info, 20, flags)== -1) {//20 max deskryptorow, -1 to fail
        perror("nftw");
        return 1;
    }
    return 0;
}
