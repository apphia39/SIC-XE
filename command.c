/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 1][project 2][project3]*
 * FIle name    : command.c										 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 04 - 30								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

list *head = NULL;
list *tail = NULL;

int check_comma(int start, int end, int token_num){
	int comma = 0;

	for(int i=start; i<end; i++){
		if(command[i] == ',')
			++comma;
	}

	if(token_num < 2 && comma == 0)
		return 1;
	if(token_num >= 2 && comma == 1)
		return 1;
		
	return -1;
}

int token_command(){
	int token_idx = 0;	//index of token
	int idx = 0;		// index of token[]

	// check_comma arrange
	int start = 0;
	int end = 0;

	for(int i=0; i<(int)strlen(command); i++){
		if(command[i] != ' ' && command[i] != ',' && command[i] != '\n' && command[i] != '\t'){
			
			/* command "loader" do not need comma check */
			if(strcmp(token[0], "loader") != 0){ 
				/* comma error check */
				if(idx == 0){
					end = i;
				
					if(check_comma(start, end, token_idx) < 0)
						return -3; // command format error
				}
			}

			token[token_idx][idx++] = command[i];
		}
		else{
			if(idx > 0){
				start = i;

				token[token_idx][idx] = '\0';
				token_idx++;

				idx=0;
			}
		}
	}

	return token_idx;
}

int check_format(char str[MAX_LEN], int *val, int option, int token_num){
	// option 0 : address, option 1 : value

	// check if the str[] is hexadecimal
	for(int i=0; i<(int)strlen(str); i++){
		if(('A' <= str[i] && str[i] <= 'F') || ('0' <= str[i] && str[i] <= '9') || ('a' <= str[i] && str[i] <= 'f'))
			continue;
		else
			return -3;
	}
	
	// if str[] is not null, change str to hexadecimal
	if(strlen(str) > 0){
		*val = turn_hexa(str);
	
		// if type is address, check address boundary
		// else if type is value, check value boundary
		if(option == 0){
			if(*val < 0 || *val >= MAX_ADDR) return -4;
		}
		else if(option == 1){
			if(*val < 0 || *val >= MAX_VAL) return -5;
		}
	}

	/* project2 */
	toUpper(token_num); 

	return 1;
}

int check_start_end(int start, int end){
	if(start >= 0 && end >= 0){
		if(start > end)
			return -1;
	}
	return 1;
}

int check_mnemonic(char str[MAX_LEN]){
	for(int i=0; i< 60; i++){
		if(strcmp(str, mnemonic[i]) == 0)
			return 1;
	}

	return -1;
}

int check_fopen(char *filename){
	FILE *fp = fopen(filename, "r");
	if(fp == NULL)
		return -1;
	return 1;
}

int check_command(int token_num, int *val1, int *val2, int *val3){
		int check_val1=0; 
		int check_val2=0; 
		int check_val3=0;
	
		if(strcmp(token[0], "q") == 0 || strcmp(token[0], "quit") == 0){
			if(token_num == 1) return 10;
			else return -2;
		}
		
		if(strcmp(token[0], "h") == 0 || strcmp(token[0], "help") == 0){
			if(token_num == 1) return 1;
			else return -2;
		}

		if(strcmp(token[0], "d") == 0 || strcmp(token[0], "dir") == 0){
			if(token_num == 1) return 2;
			else return -2;
		}

		if(strcmp(token[0], "hi") == 0 || strcmp(token[0], "history") == 0){
			if(token_num == 1) return 3;
			else return -2;
		}

		if(strcmp(token[0], "du") == 0 || strcmp(token[0], "dump") == 0){
			//check the boundary of address
			if(1 <= token_num && token_num <= 3){
				check_val1 = check_format(token[1], val1, 0, token_num);
				check_val2 = check_format(token[2], val2, 0, token_num);

				if(check_val1 < 0)
					return check_val1;
				if(check_val2 < 0)
					return check_val2;
				if(check_start_end(*val1, *val2) < 0)
					return -4;
				return 4;
			}
			else return -2;
		}
		
		if(strcmp(token[0], "e") == 0 || strcmp(token[0], "edit") == 0){
			//check the boundary of address and value
			if(token_num == 3){
				check_val1 = check_format(token[1], val1, 0, token_num);
				check_val2 = check_format(token[2], val2, 1, token_num);

				if(check_val1 < 0)
					return check_val1;
				if(check_val2 < 0)
					return check_val2;
				return 5;
			}
			else return -2;
		}

		if(strcmp(token[0], "f") == 0 || strcmp(token[0], "fill") == 0){
			//check the boundary of address and value
			if(token_num == 4){
				check_val1 = check_format(token[1], val1, 0, token_num);
				check_val2 = check_format(token[2], val2, 0, token_num);
				check_val3 = check_format(token[3], val3, 1, token_num);

				if(check_val1 < 0)
					return check_val1;
				if(check_val2 < 0)
					return check_val2;
				if(check_start_end(*val1, *val2) < 0)
					return -4;
				if(check_val3 < 0)
					return check_val3;
				return 6;
			}
			else return -2;
		}

		if(strcmp(token[0], "reset") == 0){
			if(token_num == 1) return 7;
			else return -2;
		}

		if(strcmp(token[0], "opcode") == 0){
			//check if mnemonic is correct
			if(token_num == 2){
				if(check_mnemonic(token[1]) < 0)
					return -6;
				return 8;
			}
			else return -2;
		}

		if(strcmp(token[0], "opcodelist") == 0){
			if(token_num == 1) return 9;
			else return -2;
		}

		if(strcmp(token[0], "type") == 0){
			if(token_num == 2){
				if(check_fopen(token[1]) < 0)
					return -7;
				return 11;		
			}
			else return -2;
		}

		if(strcmp(token[0], "assemble") == 0){
			if(token_num == 2){
				if(check_fopen(token[1]) < 0)
					return -7;
				return 12;	
			}
			else return -2;
		}
		
		if(strcmp(token[0], "symbol") == 0){
			if(token_num == 1) return 13;
			else return -2;
		}

		if(strcmp(token[0], "progaddr") == 0){
			if(token_num == 2){
				check_val1 = check_format(token[1], val1, 0, token_num);

				if(check_val1 < 0)
					return check_val1;
				return 14;
			}
			return -2;
		}

		if(strcmp(token[0], "loader") == 0){
			if(2 <= token_num && token_num <= 4){
				for(int i=1; i<=3; i++){
					//if the user input file[i], check if that file[i] exists in current directory
					if(strlen(token[i]) > 0){
						if(check_fopen(token[i]) < 0)
							return -7;
					}
				}
				return 15;
			}
			return -2;
		}

		if(strcmp(token[0], "bp") == 0){
			if(strcmp(token[1], "clear") == 0){
				if(token_num == 2) return 17;
				return -2;
			}
			else{
				if(1 <= token_num && token_num <= 2){
					check_val1 = check_format(token[1], val1, 0, token_num);

					if(check_val1 < 0)
						return check_val1;
					return 16;
				}
				return -2;
			}
		}

		if(strcmp(token[0], "run") == 0){
			if(token_num == 1) return 18;
			return -2;
		}
		
		return -1;
}

void toUpper(int token_num){

	for(int i=1; i<token_num; i++){
		for(int j=0; j<(int)strlen(token[i]); j++){
			if('a' <= token[i][j] && token[i][j] <= 'f')
				token[i][j] = token[i][j] - 'a' + 'A';
		}
	}
}
		
void push_history(int token_num){

	list *L = (list*)malloc(sizeof(list));
	memset(L->cmd, 0, sizeof(L->cmd));

	int idx = 0;

	for(int i=0; i<token_num; i++){
		for(int j=0; j<(int)strlen(token[i]); j++){
			L->cmd[idx++] = token[i][j];
		}

		if(token_num == 1)
			L->cmd[idx] = '\n';

		else{
			if(i == 0)
				L->cmd[idx++] = ' ';
			else if(i ==  token_num - 1){
				L->cmd[idx] = '\n';
			}
			else{
				L->cmd[idx++] = ',';
				L->cmd[idx++] = ' ';
			}
		}
	}

	L->link = NULL;

	if(head == NULL){
		head = L;
		tail = L;
	}
	else{
		tail->link = L;
		tail = L;
	}
}

void erase_history(){
	tail->cmd[0] = '\0';
}

void delete_history(){
	while(head != NULL){
		list *tmp = head->link;
		free(head);
		head = tmp;
	}
}

int turn_hexa(char *str){
	int len = strlen(str);
	int result = 0;
	int base = 1;

	for(int i=len-1; i>=0; i--){
		int value;

		if('0' <= str[i] && str[i] <= '9')
			value = str[i] - '0';
		else if('A' <= str[i] && str[i] <= 'F')
			value = str[i] - 'A' + 10;
		else if('a' <= str[i] && str[i] <= 'f')
			value = str[i] - 'a' + 10;

		result += value * base;
		base *= 16;
	}

	return result;
}

void run(int val1, int val2, int val3, int code){
	switch(code){
		case 1: help(); break;
		case 2: dir(); break;
		case 3: history(head); break;
		case 4: dump(val1, val2); break;
		case 5: edit(val1, val2); break;
		case 6: fill(val1, val2, val3); break;
		case 7: reset(); break;
		case 8: opcode_mnemonic(token[1]); break;
		case 9: opcodelist(); break; 
		case 11: type(token[1]); break;
		case 12: assemble(token[1]); break;
		case 13: symbol(); break;
		case 14: progaddr(val1); break;
		case 15: loader(token[1], token[2], token[3]); break;
		case 16: bp(val1); break;
		case 17: bp_clear(); break;
		case 18: run_program(); break;
	}
}
