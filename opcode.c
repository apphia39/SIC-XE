/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 1]					 *
 * FIle name    : opcode.c										 *
 * Author       : 20181603 kim minseon							 *			
 * Date         : 2021 - 03 - 17								 *
 *																 *			
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

void init_hashtable(){	
	for(int i=0; i<MAX_TABLE; i++){
		hash_table[i] = malloc(sizeof(oplist));
		hash_table[i]->opcode = -1;
		hash_table[i]->link = NULL;
	}
}

int hash_func(const char *str){
	// @param : str(=instruction = mnemonic)
	int hash = 301;

	for(int i=0; i<(int)strlen(str); i++)
		hash = ((hash<<4) + (int)(*str)) % MAX_TABLE;

	return hash % MAX_TABLE;
}

/* Read and get data from the file, and make hash_table using hash function */
void make_hashtable(){	
	int opcode; 
	char instruction[10]; 
	char format[5];
	int i=0; //index of mnemonic[]

	FILE *fp = fopen("opcode.txt", "r");
	if(fp == NULL){
		printf("File open failed.\n");
		printf("Please restart the program.\n");
	}

	while(fscanf(fp, "%X %s %s", &opcode, instruction, format) != EOF){
		oplist *cur = malloc(sizeof(oplist));
		cur->opcode = opcode;
		strcpy(cur->instruction, instruction);
		strcpy(cur->format, format);

		strcpy(mnemonic[i++], instruction);

		int idx = hash_func(instruction); //get hash

		if(hash_table[idx]->link == NULL){
			// if hash_table[idx] is empty
			hash_table[idx]->link = cur;
			cur->link = hash_table[idx];
		}
		else{
			// if hash_table[idx] is not empty
			cur->link = hash_table[idx]->link;
			hash_table[idx]->link = cur;
		}
		
	}

	fclose(fp);
}

void opcode_mnemonic(const char *instruction){
	int idx = hash_func(instruction);

	oplist *cur = hash_table[idx]->link;

	while(cur->opcode != -1){

		if(strcmp(instruction, cur->instruction) == 0){
			printf("opcode is %02X\n", cur->opcode);
			break;
		}
		cur = cur->link;
	}

}

void opcodelist(){
	for(int i=0; i<MAX_TABLE; i++){
		oplist *cur = hash_table[i]->link;
		
		printf("%d : ", i);

		if(cur == NULL){
			printf("\n");
			continue;
		}

		while(cur->link->opcode != -1){
			printf("[%s, %02X] -> ", cur->instruction, cur->opcode);
			cur = cur->link;
		}
		printf("[%s, %02X]\n", cur->instruction, cur->opcode);
	}
}

void delete_hashtable(){

	for(int i=0; i<MAX_TABLE; i++){
		oplist *cur = hash_table[i]->link;
		
		if(cur == NULL){
			free(hash_table[i]);
			continue;
		}
		
		while(cur->opcode != -1){
			oplist *tmp = cur->link;
			free(cur);
			cur = tmp;
		}

		free(hash_table[i]);
	}
}
