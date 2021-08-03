/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 1][project 2]			 *
 * FIle name    : shell.c										 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 04 - 07								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"
#include <dirent.h>
#include <sys/stat.h>

void help(){
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
	printf("progaddr [address]\n");
	printf("bp [address]\n");
	printf("bp clear\n");
	printf("loader [filename1] [filename2] [filename3]\n");
	printf("run\n");
}

void dir(){
	DIR *dp;
	struct dirent *dent;
	struct stat buf;

	if((dp = opendir(".")) == NULL){
		printf("dir: file open error\n");
		printf("Please restart the program.\n");
	}

	else{
		while(dent = readdir(dp)){
			lstat(dent->d_name, &buf);
			printf("%s", dent->d_name);
		
			if(S_ISDIR(buf.st_mode))
				printf("/\n");
			else if(buf.st_mode & S_IXUSR)
				printf("*\n");
			else 
				printf("\n");
		}

		closedir(dp);
	}
}

void history(list *head){
	list *tmp = head;
	int i = 1;

	while(tmp != NULL){
		if(tmp->cmd[0] != '\0'){
			printf("%d\t%s", i++, tmp->cmd);
		}
		tmp = tmp->link;
	}
}

void type(char *filename){
	FILE *fp = fopen(filename, "r");
	char str[MAX_LEN];

	//이 부분 command.c의 fopen error로 묶을 수 있나?
	if(fp == NULL){
		printf("Error!!! File open error\n");
		return ;
	}
		
	while(fgets(str, sizeof(str), fp) != NULL){
		printf("%s", str);
	}
		
	fclose(fp);
}
