/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 1/2/3]				 *
 * FIle name    : main.c										 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 03 - 31								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

void print_error(int code){
	switch(code){
		case -1: printf("Error!!! Invalid command.\n");	break;
		case -2: printf("Error!!! Command number error.\n"); break;
		case -3: printf("Error!!! Command format error.\n"); break;
		case -4: printf("Error!!! Invalid memory.\n"); break;
		case -5: printf("Error!!! Invalid value.\n"); break;
		case -6: printf("Error!!! Invalid mnemonic.\n"); break;
		case -7: printf("Error!!! File error.\n"); break;
		default : printf("Error!!!\n");
	}
}

int main(){
	
	int error_code;
	int token_num;
	int val1, val2, val3;

	init_hashtable(); make_hashtable();
	init_symtab(); init_ex_symtab();
	memset(mem, 0, sizeof(mem)); //initialize memory

	while(1){
		/* initialize */
		error_code = 0; token_num = 0;
		memset(command, 0, sizeof(command));
		for(int i=0; i<MAX_TOKEN; i++)
			memset(token[i], 0, sizeof(token[i]));
		val1 = -1; val2 = -1; val3 = -1;

		/* input command */
		printf("sicsim> ");
		fgets(command, sizeof(command), stdin);
		

		/* tokenize (parse command) */
		token_num = token_command();

		if(token_num < 0){
			print_error(-3);
			continue;
		}

		/* check command */
		error_code = check_command(token_num, &val1, &val2, &val3);

		if(error_code == 10){
			break;
		}
		else if(error_code > 0){
			push_history(token_num);
			run(val1, val2, val3, error_code);
		}
		else{
			print_error(error_code);
		}
	}
	
	delete_history();
	delete_hashtable();
	delete_symtab();
	
	return 0;
}
