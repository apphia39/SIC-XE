/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 1]					 *
 * FIle name    : memory.c										 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 03 - 15								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

//int mem[MAX_ADDR] = {0, }; //memory 1MB
int last_addr = -1;

void dump(int start, int end){
	if(start == -1){
		start = last_addr + 1;
		if(start == MAX_ADDR) start = 0;
	}
	if(end == -1){
		if(MAX_ADDR-1 < start + 159)
			end = MAX_ADDR-1;
		else
			end = start + 159;
	}

	for(int i = start/16; i <= end/16; i++){
		printf("%05X ", i*16);

		for(int j = i*16; j < (i+1)*16; j++){
			if(start <= j && j<= end)
				printf("%02X ", mem[j]);
			else
				printf("   ");
		}
		printf("; ");
		
		for(int j = i*16; j < (i+1) * 16; j++){
			if(start <= j && j <= end && 0x20 <= mem[j] && mem[j] <= 0x7E)
				printf("%c", (char)mem[j]);
			else
				printf(".");
		}
		printf("\n");
	}

	last_addr = end;
}

void edit(int address, int value){
	mem[address] = value;
}

void fill(int start, int end, int value){
	for(int i=start; i<=end; i++){
		mem[i] = value;
	}
}

void reset(){
	memset(mem, 0, sizeof(mem));
}
