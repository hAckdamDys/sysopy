#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void showFileInfo(int);

void printFiles(char* fullPath, long long bytesMax){
	/*
	dla plikow:
	ścieżka bezwzględna pliku
	rozmiar w bajtach
	prawa dostępu do pliku (w formacie używanym przez ls -l, np. r--r--rw- oznacza prawa odczytu dla wszystkich i zapisu dla właściciela)
	datę ostatniej modyfikacji
	*/
	char ffPath[1000];
	//printf("fpath: %s\n",fullPath);	
	DIR* dirp = opendir(fullPath);
	if(dirp==NULL){
		return;
	}
	struct dirent* dEnt = readdir(dirp);
	struct stat fileStat;
	while(dEnt!=NULL){
		//printf("%s \n",dEnt->d_name);
		if(strcmp(dEnt->d_name,".")==0 || strcmp(dEnt->d_name,"..")==0){
			dEnt = readdir(dirp);
			continue;
		}
		strcpy(ffPath,fullPath);
		strcat(ffPath,"/");
		strcat(ffPath,dEnt->d_name);
		//printf("\n\n\n!!ffpath : %s\n",ffPath);
		lstat(ffPath, &fileStat);
		
		if(fileStat.st_mode & S_IFMT & S_IFDIR){
			//printf("\nDIR HERE!\n");
			printFiles(ffPath,bytesMax);
			dEnt = readdir(dirp);
			continue;
		}
		if(!(fileStat.st_mode & S_IFMT & S_IFREG)){//jak nie regular
			dEnt = readdir(dirp);
			continue;
		}
		if(fileStat.st_size>bytesMax){//jak za duzy
			dEnt = readdir(dirp);
			continue;
		}
		printf("%s\n",ffPath);
		
		printf("File size: %li\n",fileStat.st_size);
		printf("File Permissions: \t");
		printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
		printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
		printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
		printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
		printf("\nLast file modification:   %s\n", ctime(&fileStat.st_mtime));

		dEnt = readdir(dirp);
	}
}

int main(int argc,char *argv[]){
	if(argc!=3){
		printf("Podaj 2 argumenty; sciezke i bajty\n");
		return 1;
	}
	char* dirName = argv[1];
	long long bytesMax = atoi(argv[2]);
	printFiles(realpath(dirName, NULL),bytesMax);
	
	return 0;
}