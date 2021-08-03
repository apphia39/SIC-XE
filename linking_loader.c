/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 1][project2][project3] *
 * FIle name    : 20181603.h									 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 04 - 30								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

int program_start_addr = 0; //PROGADDR
int break_point[MAX_LEN];	//store break points
int bp_flag[MAX_LEN];
int bp_num = 0;				//the number of break points
int csaddr = -1;			//CSADDR
int total_program_length = 0; //the length of total program
int reg[10];				//register for running program
int run_start = -1;
int run_end = -1;

void print_linking_loader_error(int error_code){
	printf("Linking Loader Error!!! ");

	switch(error_code){
		case -1 : printf("File open error\n"); break;
		case -2 : printf("Duplicated external symbol\n"); break;
		case -3 : printf("Undefined external symbol\n"); break;
		default : printf("\n");
	}
}

void init_reg(){
	//A, X, L, B, S, T, F, ?, PC, SW
	for(int i=0; i<10; i++)
		reg[i] = 0;

	reg[2] = total_program_length;
	reg[8] = program_start_addr;
}

void init_estab(){
	for(int i=0; i<MAX_ESTAB; i++){
		es_table[i] = malloc(sizeof(estab));
		es_table[i]->address = -1;
		es_table[i]->control_section = 0;
		es_table[i]->length = 0;
		es_table[i]->link = NULL;
	}

	memset(external_symbol, 0, sizeof(external_symbol));
	external_symbol_num = 0;
}

int estab_hash(const char* name){
	int hash = 301;

	for(int i=0; i<(int)strlen(name); i++){
		hash = ((hash<<4) + (int)(*name)) % MAX_ESTAB;
	}

	return hash % MAX_ESTAB;
}

int push_estab(const char* name, int address, int cs, int length){
	estab *cur = malloc(sizeof(estab));
	strcpy(cur->symbol_name, name);
	cur->address = address;

	strcpy(external_symbol[external_symbol_num++], name);

	if(cs == 1){
		cur->control_section = 1;
		cur->length = length;
	}

	int idx = estab_hash(name);

	if(es_table[idx]->link == NULL){
		es_table[idx]->link = cur;
		cur->link = es_table[idx];
	}
	else{
		estab *tmp = es_table[idx]->link;
		while(tmp->address != -1){
			if(strcmp(cur->symbol_name, tmp->symbol_name) == 0)
				return -1;
			tmp = tmp->link;
		}

		cur->link = es_table[idx]->link;
		es_table[idx]->link = cur;
	}

	return 1;
}

int find_estab(const char *name){
	int idx = estab_hash(name);
	estab* cur = es_table[idx]->link;

	if(cur){
		while(cur->address != -1){
			if(strcmp(cur->symbol_name, name) == 0)
				return cur->address;
			cur = cur->link;
		}
	}
	return -1;
}


int loader_pass1(char file[MAX_FILE_NUM][MAX_FILENAME]){
	int file_num = 0; // the number of object files to load
	int csaddr = -1; // control section address
	int cslth; // control section length
	char str[100]; // each line of file
	char symbol_name[30]; // external symbol name 
	int symbol_address; // address of external symbol
	int flag; // end flag of while loop

	//get the number of files to read
	for(int i=0; i<MAX_FILE_NUM; i++){
		if(strlen(file[i]) > 0)
			++file_num;
	}

	for(int i=0; i<file_num; i++){
		char *ptr;
		FILE *fp = fopen(file[i], "r");

		//check file open error
		if(fp == NULL) return -1;

		//initialize
		flag = 0;
		cslth = 0;

		//set csaddr of first control section
		if(csaddr == -1)
			csaddr = program_start_addr;

		//read obj file
		while(fgets(str, sizeof(str), fp) != NULL){
			//E record : get CSADDR of next control section
			if(str[0] == 'E'){
				csaddr += cslth;
				break;
			}
			//H record : put control section name and CSADDR into EXTAB & set CSLTH
			else if(str[0] == 'H'){
				ptr = strtok(str+1, " \n");
				strcpy(symbol_name, ptr);
				ptr = strtok(NULL, " \n");
				cslth = turn_hexa(ptr+6);

				if(push_estab(symbol_name, csaddr, 1, cslth) < 0){
					return -2;
				}
			}
			//D record : put external symbols into ESTAB
			else if(str[0] == 'D'){
				ptr = strtok(str+1, " \n");
				while(ptr != NULL){
					strcpy(symbol_name, ptr);
					ptr = strtok(NULL, " \n");

					if(strlen(ptr) > 6){ //000030ENDA
						char tmp[6];
						for(int k=0; k<6; k++)
							tmp[k] = ptr[k];
						symbol_address = turn_hexa(tmp);
						ptr = ptr+6;
					}
					else{ //000042
						symbol_address = turn_hexa(ptr);
						flag = 1;
					}

					if(push_estab(symbol_name, csaddr+symbol_address, 0, 0) < 0){
						return -2;
					}

					if(flag == 1) break;
				}
			}
		}
		fclose(fp);
	}

	return 1;
}

int char_to_hexa(char character){
	if('0' <= character && character <= '9')
		return character-'0';
	else
		return character-'A'+10;
}

int loader_pass2(char file[MAX_FILE_NUM][MAX_FILENAME]){
	int file_num = 0;
	int csaddr = -1;
	int cslth;
	int execaddr = -1;
	char str[100];
	char refer_num[10][10];
	char tmp_6[6];
	int tmp;

	//get the number of files to read
	for(int i=0; i<MAX_FILE_NUM; i++){
		if(strlen(file[i]) > 0)
			++file_num;
	}

	for(int i=0; i<file_num; i++){
		char *ptr;
		FILE *fp = fopen(file[i], "r");

		//check file open error
		if(fp == NULL) return -1;

		//initialize
		memset(refer_num, 0, sizeof(refer_num));
		cslth = 0;

		//set csaddr to progaddr, execaddr to progaddr
		if(csaddr == -1)
			csaddr = program_start_addr;
		if(execaddr == -1)
			execaddr = program_start_addr;

		//read obj file
		while(fgets(str, sizeof(str), fp) != NULL){
			//E record : get first instruction address, transfer control
			if(str[0] == 'E'){
				ptr = strtok(str+1, " \n");
				if(ptr)
					execaddr = turn_hexa(ptr);
				csaddr += cslth;
				break;
			}

			//H record : get cslth, control section name
			else if(str[0] == 'H'){
				ptr = strtok(str+1, " \n");
				strcpy(refer_num[1], ptr);
				ptr = strtok(NULL, " \n");
				cslth = turn_hexa(ptr+6);
			}

			//R record : reference number
			else if(str[0] == 'R'){
				ptr = strtok(str+1, " \n");
				while(ptr != NULL){
					tmp = char_to_hexa(ptr[0]) * 10 + char_to_hexa(ptr[1]);
					strcpy(refer_num[tmp], ptr+2);
					ptr = strtok(NULL, " \n");
				}
			}

			//T record : load to memory(address + csaddr)
			else if(str[0] == 'T'){
				//get the start address of text record
				memset(tmp_6, 0, sizeof(tmp_6));
				strncpy(tmp_6, str+1, 6);
				int record_start_addr = turn_hexa(tmp_6);

				//get the length of text record
				int record_length = char_to_hexa(str[7]) * 16 + char_to_hexa(str[8]);

				//get memory address
				record_start_addr += csaddr;

				//read text record
				int str_idx = 9;
				for(int k=0; k<record_length; k++){
					tmp = char_to_hexa(str[str_idx++]) * 16 + char_to_hexa(str[str_idx++]);
					mem[record_start_addr++] = tmp;
				}
			}

			//M record : find in ESTAB, relocation & linking
			else if(str[0] == 'M'){
				char objcode[6];
				int result = 0;
				int symbol_addr;
				int mem_addr;
				memset(objcode, 0, sizeof(objcode));
				memset(tmp_6, 0, sizeof(tmp_6));

				strncpy(tmp_6, str+1, 6);
				mem_addr = turn_hexa(tmp_6) + csaddr;

				//1. combination
				result = mem[mem_addr]*256*256 + mem[mem_addr+1]*256 + mem[mem_addr+2];
				if(result >= 0x800000){
					result = 16777216 - result;
					result = -result;
				}

				//2. sum or subtract
				int refer_idx = char_to_hexa(str[10]) * 10 + char_to_hexa(str[11]);
				symbol_addr = find_estab(refer_num[refer_idx]);

				if(symbol_addr < 0)
					return -3;

				if(str[9] == '+')
					result += symbol_addr;
				else
					result -= symbol_addr;
				
				if(result < 0){
					result = -result;
					result = 16777216 - result; //16^6 = 16,777,216
				}
	
				//3. divide
				comb(6, result, objcode, 0);
				mem[mem_addr] = char_to_hexa(objcode[0]) * 16 + char_to_hexa(objcode[1]);
				mem[mem_addr+1] = char_to_hexa(objcode[2]) * 16 + char_to_hexa(objcode[3]);
				mem[mem_addr+2] = char_to_hexa(objcode[4]) * 16 + char_to_hexa(objcode[5]);
			}
		}

		fclose(fp);
	}

	return 1;
}

void delete_estab(){
	for(int i=0; i<MAX_ESTAB; i++){
		estab *cur = es_table[i]->link;

		if(cur == NULL){
			free(es_table[i]);
			continue;
		}

		while(cur->address != -1){
			estab *tmp = cur->link;
			free(cur);
			cur = tmp;
		}

		free(es_table[i]);
	}
}

void loader(char *filename1, char *filename2, char *filename3){
	char file[MAX_FILE_NUM][MAX_FILENAME];
	int error_code = 0;

	//initialize
	strcpy(file[0], filename1);
	strcpy(file[1], filename2);
	strcpy(file[2], filename3);
	csaddr = -1; 
	total_program_length = 0;

	//initialize estab
	init_estab();

	error_code = loader_pass1(file);
	if(error_code < 0){
		print_linking_loader_error(error_code);
		return;
	}

	error_code = loader_pass2(file);
	if(error_code < 0){
		print_linking_loader_error(error_code);
		return;
	}

	//if no error, initialize registers and print load map(ESTAB)
	printf("control symbol address length\n");
	printf("section name\n");
	printf("------------------------------------\n");
	for(int i=0; i<external_symbol_num; i++){
		int idx = estab_hash(external_symbol[i]);
		estab* cur = es_table[idx]->link;

		if(cur){
			while(cur->address != -1){
				if(strcmp(cur->symbol_name, external_symbol[i]) == 0){
					if(cur->control_section == 1){
						printf("%-7s %7s %04X\t%04X\n", cur->symbol_name, "", cur->address, cur->length);
						total_program_length += cur->length;
					}
					else{
						printf("%-7s %-7s %04X\n", "", cur->symbol_name, cur->address);
					}
					break;
				}
				cur = cur->link;
			}
		}
	}
	printf("------------------------------------\n");
	printf("%10s total length %04X\n", "", total_program_length);

	init_reg();
	delete_estab();
}

void progaddr(int address){
	program_start_addr = address;
}

void bp(int address){
	//bp : print all breakpoints
	if(address == -1){
		printf("\tbreakpoint\n");
		printf("\t----------\n");
		for(int i=0; i<bp_num; i++){
			printf("\t%X\n", break_point[i]);
		}
	}
	//bp [address] : store breakpoint
	else{
		bp_flag[bp_num] = 0;
		break_point[bp_num++] = address;
		printf("\t[ok] create breakpoint %X\n", address);
	}
}

void bp_clear(){
	//bp clear : remove all breakpoints
	memset(break_point, 0, sizeof(break_point));
	bp_num = 0;
	printf("\t[ok] clear all breakpoints\n");
}

void print_register(){
	printf("A : %06X   X : %06X\n", reg[0], reg[1]);
	printf("L : %06X  PC : %06X\n", reg[2], reg[8]);
	printf("B : %06X   S : %06X\n", reg[3], reg[4]);
	printf("T : %06X\n", reg[5]);
}

int get_format(int mem_value, int ni, int e){
	int opcode;

	//format 2
	if(mem_value == 0xB4 || mem_value == 0xA0 || mem_value == 0xB8)
		return 2;

	opcode = mem_value - ni;

	//format 4
	if(e == 1) return 4;

	//format 3
	else return 3;
}

void get_register(int idx, int *r1, int *r2){
	*r1 = mem[idx+1] / 16;
	*r2 = mem[idx+1] % 16;
}

void disassemble(int idx, int opcode, int target_addr, int value){
	int tmp;
	int r1, r2;

	switch(opcode){
		case 0xB4 : //CLEAR r1
			get_register(idx, &r1, &r2);
			reg[r1] = 0;
			break;
		case 0xA0 : //COMPR r1, r2
			get_register(idx, &r1, &r2);
			if(reg[r1] < reg[r2]) reg[9] = -1;
			else if(reg[r1] == reg[r2]) reg[9] = 0;
			else reg[9] = 1;
			break;
		case 0xB8 : //TIXR r1
			get_register(idx, &r1, &r2);
			reg[1]++;
			if(reg[1] < reg[r1]) reg[9] = -1;
			else if(reg[1] == reg[r1]) reg[9] = 0;
			else reg[9] = 1;
			break;
		case 0x28 : //COMP m
			if(reg[0] < value) reg[9] = -1;
			else if(reg[0] == value) reg[9] = 0;
			else reg[9] = 1;
			break;
		case 0x3C : //J m
			reg[8] = target_addr;
			break;
		case 0x30 : //JEQ m
			if(reg[9] == 0) reg[8] = target_addr;
			break;
		case 0x38 : //JLT m
			if(reg[9] == -1) reg[8] = target_addr;
			break;
		case 0x48 : //JSUB m
			reg[2] = reg[8];
			reg[8] = target_addr;
			break;
		case 0x00 : //LDA m 
			reg[0] = value;
			break;
		case 0x68 : //LDB m
			reg[3] = value;
			break;
		case 0x50 : //LDCH m 
			for(int i=0; i<reg[1]; i++) value = value / 256;
			reg[0] = reg[0] - reg[0]%256;
			reg[0] += value % 256;
			break;
		case 0x74 : //LDT m
			reg[5] = value;
			break;
		case 0xD8 : //RD m
			reg[9] = 0;
			break;
		case 0x4C : //RSUB m
			reg[8] = reg[2];
			break;
		case 0x0C : //STA m
			tmp = reg[0];
			mem[target_addr+2] = tmp % 256; tmp /= 256;
			mem[target_addr+1] = tmp % 256; tmp /= 256;
			mem[target_addr] = tmp % 256;
			break;
		case 0x54 : //STCH m 
			tmp = reg[0];
			mem[target_addr+2] = tmp % 256;
			mem[target_addr+1] = 0;
			mem[target_addr] = 0;
			break;
		case 0x14 : //STL m
			tmp = reg[2];
			mem[target_addr+2] = tmp % 256; tmp /= 256;
			mem[target_addr+1] = tmp % 256; tmp /= 256;
			mem[target_addr] = tmp % 256;
			break;
		case 0x10 : //STX m
			tmp = reg[1];
			mem[target_addr+2] = tmp % 256; tmp /= 256;
			mem[target_addr+1] = tmp % 256; tmp /= 256;
			mem[target_addr] = tmp % 256;
			break;
		case 0xE0 : //TD m
			reg[9] = -1;
			break;
		case 0xDC : //WD m
			break;
	}
}

int execute(int idx){
	int format;
	int opcode;
	int ni, xbpe;
	int x, b, p, e;
	int target_addr;
	int value;

	//initialize
	ni = mem[idx] % 4;
	xbpe = mem[idx+1] / 16;
	x = xbpe / 8; xbpe = xbpe - x*8;
	b = xbpe / 4; xbpe = xbpe - b*4;
	p = xbpe / 2; 
	e = xbpe % 2;
	target_addr = 0; value = 0;

	//get format and opcode
	format = get_format(mem[idx], ni, e);
	opcode = mem[idx] - ni;

	//get pc regiseter
	reg[8] = idx + format;

	//if register operation
	if(format == 2){
		disassemble(idx, opcode+ni, target_addr, value);
	}
	else{
		//get Target Address
		if(format == 3){ //format3
			if(ni == 0){ //SIC instruction
				target_addr = (mem[idx+1] - x*8) * 256 + mem[idx+2];
				value = mem[target_addr]*256*256 + mem[target_addr+1]*256 + mem[target_addr+2];
			}
			else{
				target_addr = (mem[idx+1] % 16) * 256 + mem[idx+2];

				//if target address is negative number
				if(target_addr >= 0x800) 
					target_addr -= 0x1000;

				if(p == 1)//pc relative
					target_addr += reg[8];
				else if(b == 1) //base relative
					target_addr += reg[3];

				if(x == 1) //indexed mode
					target_addr += reg[1];

				if(ni == 1){ //immediate addressing mode
					value = target_addr;
				}
				else if(ni == 2){ //indirect addressing mode
					value = mem[target_addr]*256*256 + mem[target_addr+1]*256 + mem[target_addr+2];
					value = mem[value]*256*256 + mem[value+1]*256 + mem[value+2];
				}
				else{ //simple addressing mode
					value = mem[target_addr]*256*256 + mem[target_addr+1]*256 + mem[target_addr+2];
				}
			}
		}
		else{ //format4
			target_addr = (mem[idx+1] % 16) * 256 * 256 + mem[idx+2] * 256 + mem[idx+3];

			if(x == 1) //indexed mode
				target_addr += reg[1];


			if(ni == 1){// immediate addressing mode
				value = target_addr;
			}
			else if(ni == 2){ //indirect addressing mode
				value = mem[target_addr]*256*256 + mem[target_addr+1]*256 + mem[target_addr+2];
				value = mem[value]*256*256 + mem[value+1]*256 + mem[value+2];
			}
			else{ //simple addressing mode
				value = mem[target_addr]*256*256 + mem[target_addr+1]*256 + mem[target_addr+2];

			}
		}

		disassemble(idx, opcode, target_addr, value);
	}
	return reg[8];
}

void run_program(){
	//get start, end
	if(run_start == -1)
		run_start = program_start_addr;
	else
		run_start = reg[8];

	run_end = program_start_addr + total_program_length;

	for(int idx = run_start; ; ){
		//check if idx is break point
		for(int bp_idx = 0; bp_idx < bp_num; bp_idx++){
			if(idx == break_point[bp_idx] && bp_flag[bp_idx] == 0){
				bp_flag[bp_idx] = 1;
				print_register();
				printf("%11s Stop at checkpoint[%X]\n", "", idx);
				return;
			}
		}

		//check the end of memory
		if(idx == 0x2A){
			reg[8] = program_start_addr + total_program_length;
			break;
		}

		idx = execute(idx);
	}
	
	//end program
	print_register();
	printf("%11s End Program\n", "");
	for(int bp_idx = 0; bp_idx < bp_num; bp_idx++)
		bp_flag[bp_idx] = 0;
	run_start = -1;
}
