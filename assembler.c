/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 2]					 *
 * FIle name    : assembler.c									 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 04 - 16								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

char directive[8][6] = {"START", "END", "BYTE", "WORD", "RESB", "RESW", "BASE", "NOBASE"};
char registers[9][2] = {"A", "X", "L", "B", "S", "T", "F", "PC", "SW"};

/*=================================
 * PASS1 
 =================================*/
int token_file(const char* str, char* label, char* instruction, char* operand1, char* operand2, int* option, int* format) {
	//option 0:simple, 1:indirect, 2:immediate 
	//format 3: format3, 4: format4

	char tmp_str[strlen(str)];
	strcpy(tmp_str, str);
	char* ptr = strtok(tmp_str, ", ");

	//comment
	if (str[0] == '.') {
		label[0] = '.';
		return 1;
	}

	else {
		//no label
		if (str[0] == ' ') {
			if (ptr[0] == '+') {
				strcpy(instruction, ptr + 1);
				*format = 4; //format4
			}
			else {
				strcpy(instruction, ptr);
				*format = 3; //format3
			}

			if (ptr = strtok(NULL, ", ")) {
				if (ptr[0] == '#') {
					strcpy(operand1, ptr + 1);
					*option = 2; //immediate
				}
				else if (ptr[0] == '@') {
					strcpy(operand1, ptr + 1);
					*option = 1; //indirect
				}
				else {
					strcpy(operand1, ptr);
					*option = 0; //simple
				}
			}
			if (ptr = strtok(NULL, ", ")) {
				strcpy(operand2, ptr);
			}
		}
		//label
		else {
			strcpy(label, ptr);

			if (ptr = strtok(NULL, ", ")) {
				if (ptr[0] == '+') {
					strcpy(instruction, ptr + 1);
					*format = 4; //format4
				}
				else {
					strcpy(instruction, ptr);
					*format = 3; //format3
				}
			}
			if (ptr = strtok(NULL, ", ")) {
				if (ptr[0] == '#') {
					strcpy(operand1, ptr + 1);
					*option = 2; //immediate
				}
				else if (ptr[0] == '@') {
					strcpy(operand1, ptr + 1);
					*option = 1; //indirect
				}
				else {
					strcpy(operand1, ptr);
					*option = 0; //simple
				}
			}
			if (ptr = strtok(NULL, ", ")) {
				strcpy(operand2, ptr);
			}
		}
	}
	return 0;
}

int chk_label_error(const char* label){
	//if label has same name with mnemonic, error
	for(int i=0; i<58; i++){
		if(strcmp(mnemonic[i], label) == 0)
			return -1;
	}
	return 1;
}

int chk_mnemonic_error(const char* instruction) {
	//directive
	for (int i = 0; i < 8; i++) {
		if (strcmp(directive[i], instruction) == 0) {
			obj_table[obj_num].dir = 1;
			return 1;
		}
	}

	//mnemonic opcode
	for (int i = 0; i < 58; i++) {
		if (strcmp(mnemonic[i], instruction) == 0)
			return 1;
	}

	return -3;
}

int is_register(const char* operand) {
	if (operand) {
		for(int i=0; i<9; i++){
			if(strcmp(operand, registers[i]) == 0){
				return 1;
			}
		}
	}
	return 0;
}

//modified 0416
int chk_byte_error(const char *operand){
	//BYTE C' '인 경우
	//BYTE X' '인 경우, ' '사이에는 2글자가 와야하며, 그 값은  16진수이다.
	int len = strlen(operand)-3;
	int error_flag = 0;

	if(operand[0] == 'C'){
		if(len < 0)
			error_flag = -1;
	}

	else if(operand[0] == 'X'){
		if(len != 2) 
			error_flag = -1;
		
		for(int i=2; i<=3; i++){
			if(('A' <= operand[i] && operand[i] <= 'F') || ('0' <= operand[i] && operand[i] <= '9') || 'a' <= operand[i] && operand[i] <= 'f') ;
			else error_flag = -1;
		}
	}

	else
		error_flag = -1;

	return error_flag;
}

int chk_word_error(const char *operand){
	int error_flag = 0;
	int len = strlen(operand);

	for(int i=0; i<len; i++){
		if(operand[0] < '0' || operand[0] > '9'){
			error_flag = -1;
			break;
		}
	}

	return error_flag;
}

int chk_res_error(const char *operand, int option){
	//option 0: resb, option1: resw, option2 : start
	//if operand is not number, error
	//if operand is >= 0x100000, error

	int error_flag = 0;
	int len = strlen(operand);
	int res = 0;
	
	for(int i=0; i<len; i++){
		if(operand[i] < '0' || operand[i] > '9'){
			error_flag = -1;
			break;
		}
	}

	if(option == 0 || option == 2)
		res = atoi(operand);
	else
		res = atoi(operand) * 3;

	if(res < 0 || res >= MAX_LOC)
		error_flag = -1;

	return error_flag;
}

int chk_address(const char* instruction, const char* operand1, const char* operand2, int format) {
	int error_code = 0;

	//if register operation, return 2
	if (is_register(operand1) == 1) {
		//check if operand2 is register
		obj_table[obj_num].reg = 1;
		obj_table[obj_num].format = 2;
		return 2;
	}

	//if BYTE, C: return constant length, X: return 1
	if (strcmp(instruction, "BYTE") == 0) {
		error_code = chk_byte_error(operand1);
		if (error_code == -1)
			return -4;
		else {
			if (operand1[0] == 'C') return strlen(operand1) - 3; //the number of character
			else if (operand1[0] == 'X') return 1;
			else return -1;
		}
	}

	//if WORD, return 3
	if (strcmp(instruction, "WORD") == 0) {
		error_code = chk_word_error(operand1);
		if (error_code == -1) return -4;
		else 
			return 3;
	}

	//if RESB, return value
	if (strcmp(instruction, "RESB") == 0) {
		error_code = chk_res_error(operand1, 0);
		if (error_code == -1) return -4;
		else return atoi(operand1);
	}

	//if RESW, return value*3
	if (strcmp(instruction, "RESW") == 0) {
		error_code = chk_res_error(operand1, 1);
		if (error_code == -1) return -4;
		else 
			return atoi(operand1) * 3;
	}

	//if directives, return 0
	if (strcmp(instruction, "START") == 0 || strcmp(instruction, "BASE") == 0 || strcmp(instruction, "NOBASE") == 0 || strcmp(instruction, "END") == 0) {
		return 0;
	}

	//if format4, return 4
	if (format == 4) return 4;
	
	return 3;
}

int is_indexed(const char* operand) {
	if (operand) {
		if (strcmp(operand, "X") == 0)
			return 1;
	}
	return 0;
}

int chk_locctr_error(int locctr){
	if(0 <= locctr && locctr < MAX_LOC)
		return 1;
	else
		return -1;
}

int pass1(const char *filename) {
	FILE* fp = fopen(filename, "r");
	char str[100]; //read file
	char label[MAX_ASM];
	char instruction[MAX_ASM];
	char operand1[MAX_ASM];
	char operand2[MAX_ASM];
	int option = 0; //0:simple, 1:indirect, 2:immediate
	int format = 3; //1: format2, 2: format2, 3:format3, 4:format4
	int comment = 0; //0:no comment, 1:comment
	int start = 0;
	int address = 0;
	int locctr = 0;
	int i = 0; //.asm file line number
	int endflag = 0; //check if there is END

	if (fp == NULL) {
		return -1;
	}

	while (fgets(str, sizeof(str), fp) != NULL) {
		//initialize
		str[strlen(str) - 1] = '\0';
		label[0] = '\0'; instruction[0] = '\0'; operand1[0] = '\0'; operand2[0] = '\0';
		i++; obj_num++;
		option = 0; format = 3; comment = 0; address = 0;
		obj_table[i].line_num = i*5;

		if(str[0] == '\n' || str[0] == '\0')
			break;

		//token source file, check if the line is comment
		comment = token_file(str, label, instruction, operand1, operand2, &option, &format);

		//if END, set endflag
		if (strcmp(instruction, "END") == 0)
			endflag = 1;

		//if "START", initialize locctr
		if (strcmp(instruction, "START") == 0) {
			locctr = turn_hexa(operand1);
			start = locctr;
		}

		//if comment
		if (comment == 1) {
			obj_table[obj_num].format = 0;
			strcpy(obj_table[obj_num].comment, str);
			continue;
		}

		//if not comment,
		//check if label & mnemonic is valid
		if(chk_label_error(label) < 0){
			printf("Error occurs : line %d\n", i);
			fclose(fp);
			return -8;
		}
		if (chk_mnemonic_error(instruction) < 0) {
			printf("Error occurs : line %d\n", i);
			fclose(fp);
			return -3;
		}

		//get address
		//check the operand of directives
		address = chk_address(instruction, operand1, operand2, format);
		if (address < 0) {
			printf("Error occurs : line %d\n", i);
			fclose(fp);
			return -4;
		}

		//make symtab
		if(strcmp(instruction, "START") != 0){
			if(address > 0 && label[0] != '\0'){
				if(push_symtab(label, locctr) < 0){
					printf("Error occurs : line %d\n", i);
					fclose(fp);
					return -2;
				}
			}
		}

		obj_table[obj_num].locctr = locctr;
		obj_table[obj_num].address = address;
		obj_table[obj_num].format = format;
		obj_table[obj_num].addr_mode = option;
		strcpy(obj_table[obj_num].label, label);
		strcpy(obj_table[obj_num].mnemonic, instruction);
		strcpy(obj_table[obj_num].operand1, operand1);
		strcpy(obj_table[obj_num].operand2, operand2);
		if (is_indexed(operand2) == 1) obj_table[obj_num].indexed = 1;

		locctr += address;

		//check if locctr is valid
		if (chk_locctr_error(locctr) < 0) {
			printf("Error occurs : line %d\n", i);
			fclose(fp);
			return -6;
		}
	}

	if (endflag == 0)
		return -5;

	program_length = locctr - start;
	fclose(fp);

	return 0;
}


/*=================================
 * PASS2
 =================================*/
int get_regnum(const char *reg){
	for (int i = 0; i < 9; i++) {
		if (strcmp(reg, registers[i]) == 0){
			return i;
		}
	}
	
	return -1;
}

int find_relative(int i, int pc, int base){
	int address = find_symtab(obj_table[i].operand1);
	int result = address - pc;

	//if operand is number, address is that number
	if('0' <= obj_table[i].operand1[0] && obj_table[i].operand1[0] <= '9')
		return atoi(obj_table[i].operand1);

	//no operand -> address 0 (ex. RSUB)
	if(strlen(obj_table[i].operand1) == 0)
		return 0;

	//assume if pc relative
	if(-2048 <= result && result <= 2047){
		obj_table[i].rel_mode = 1;
		result = address - pc;
		if(result < 0){
			result = ~result;
			result += 1;
			result = 4096 - result;
		}
		return result;
	}

	result = result + pc;
	result = result - base;

	//assume if base relative
	if(0<= result && result <= 4095){
		obj_table[i].rel_mode = 2;
		return result;
	}
		
	//error
	return MAX_LOC;
}

int make_lstFile(const char *filename){
	FILE *fp = fopen(filename, "w");

	if(fp == NULL){
		return -1;
	}

	for(int i=1; i<obj_num; i++){
		fprintf(fp, "%d\t", obj_table[i].line_num);  //write line_number
		
		if(obj_table[i].format == 0){//comment
			fprintf(fp, "\t%s\n", obj_table[i].comment);
			continue;
		}

		//write locctr
		if(strcmp(obj_table[i].mnemonic, "BASE") == 0 
		|| strcmp(obj_table[i].mnemonic, "END") == 0 
		|| strcmp(obj_table[i].mnemonic, "NOBASE") == 0){
			fprintf(fp, "\t");
		}
		else
			fprintf(fp, "%04X\t", obj_table[i].locctr);

		//write label
		if(strlen(obj_table[i].label) == 0)
			fprintf(fp, "\t");
		else
			fprintf(fp, "%s\t", obj_table[i].label);

		//write mnemonic
		if(obj_table[i].format == 4)
			fprintf(fp, "+");
		fprintf(fp, "%s\t", obj_table[i].mnemonic);

		//write operand
		if(strlen(obj_table[i].operand1) == 0)
			fprintf(fp, "\t\t");
		else{
			if(obj_table[i].addr_mode == 1)
				fprintf(fp, "@");
			else if(obj_table[i].addr_mode == 2)
				fprintf(fp, "#");
			fprintf(fp, "%s", obj_table[i].operand1);

			if(strlen(obj_table[i].operand2) == 0)
				fprintf(fp, "\t\t");
			else{
				fprintf(fp, ", %s\t", obj_table[i].operand2);

				//to make object codes readable 
				if(strlen(obj_table[i].operand1) + strlen(obj_table[i].operand2) < 4)
					fprintf(fp, "\t");
			}
		}

		//write object code
		if(strcmp(obj_table[i].mnemonic, "BASE") == 0
		|| strcmp(obj_table[i].mnemonic, "NOBASE") == 0
		|| strcmp(obj_table[i].mnemonic, "RESW") == 0
		|| strcmp(obj_table[i].mnemonic, "RESB") == 0
		|| strcmp(obj_table[i].mnemonic, "END") == 0
		|| strcmp(obj_table[i].mnemonic, "START") == 0){
			fprintf(fp, "\n");
		}
		else{
			fprintf(fp, "%s\n", obj_table[i].objcode);
		}
	}

	fclose(fp);
	return 1;
}

int make_objFile(const char *filename){
	char text_record[60];
	int record_idx = 0;
	int start = 0;
	int plus = 0;
	int exec_addr = -1;

	FILE *fp = fopen(filename, "w");
	if(fp == NULL){
		return -1;
	}

	//Header record
	fprintf(fp, "H%-6s", obj_table[1].label);
	fprintf(fp, "%06X%06X\n", obj_table[1].locctr, program_length);
	start = obj_table[1].locctr;

	//Text record
	for(int i=2; i<obj_num-1; i++){
		plus = 0;

		//if comment, skip
		if(obj_table[i].format == 0
		|| strcmp(obj_table[i].mnemonic, "BASE") == 0
		|| strcmp(obj_table[i].mnemonic, "NOBASE") == 0
		|| strcmp(obj_table[i].mnemonic, "START") == 0
		|| strcmp(obj_table[i].mnemonic, "END") == 0)
			continue;


		plus = strlen(obj_table[i].objcode);
	
		if(strcmp(obj_table[i].mnemonic, "RESW") == 0
		|| strcmp(obj_table[i].mnemonic, "RESB") == 0){
			if(record_idx == 0) continue;
			else{
				fprintf(fp, "T%06X%02X%s\n", start, record_idx/2, text_record);
				memset(text_record, '\0', sizeof(text_record));
				record_idx = 0;
				start = -1;
				continue;
			}
		}

		if(record_idx + plus >= 60){
			fprintf(fp, "T%06X%02X%s\n", start, record_idx/2, text_record);
			memset(text_record, '\0', sizeof(text_record));
			record_idx = 0;
			start = obj_table[i].locctr;
			i--;
			continue;
		}

		if(start == -1)
			start = obj_table[i].locctr;
		
		if(exec_addr == -1)
			exec_addr = obj_table[i].locctr;

		strcpy(text_record+record_idx, obj_table[i].objcode);
		record_idx += strlen(obj_table[i].objcode);
	}

	if(record_idx > 0)
		fprintf(fp, "T%06X%02X%s\n", start, record_idx/2, text_record);

	record_idx = 0;

	//Modification record
	for(int i=1; i<obj_num-1; i++){
		if(obj_table[i].format == 4 && strlen(obj_table[i].operand1) > 0){
			if('0' <= obj_table[i].operand1[0] && obj_table[i].operand1[0] <= '9'){
				//if operand is constant
				continue;
			}
			else
				fprintf(fp, "M%06X05\n", record_idx/2 + 1);
		}

		record_idx += strlen(obj_table[i].objcode);
	}

	//End record
	fprintf(fp, "E%06X\n", exec_addr);

	fclose(fp);
	return 1;
}

void get_opcode(){
	for(int i=1; i<obj_num; i++){
		int idx = hash_func(obj_table[i].mnemonic);
		oplist *cur = hash_table[idx]->link;

		if(cur){
			while(cur->opcode != -1){
				if(strcmp(cur->instruction, obj_table[i].mnemonic) == 0)
					obj_table[i].opcode = cur->opcode;
				cur = cur->link;
			}
		}
	}
}


void comb(int mul, int o, char *objcode, int idx){
	int base16 = 1;
	int i;
	int result;

	for(i=1; i<mul; i++)
		base16 *= 16;

	for(i=0; i<mul; i++){
		result = o/base16;

		if(0 <= result && result < 10)
			objcode[idx++] = '0' + result;
		else if(10 <= result && result < 16)
			objcode[idx++] = 'A' + result - 10;

		o %= base16;
		base16 /= 16;
	}
}

int make_objCode(){
	int pc = 0;
	int base = 0;
	int objcode1, objcode2, objcode3;

	for(int i=1; i<obj_num - 1; i++){
		//initialize
		objcode1 = 0; objcode2 = 0; objcode3 = 0;
		pc = obj_table[i+1].locctr;
		if(strcmp(obj_table[i].mnemonic, "BASE") == 0)
			base = find_symtab(obj_table[i].operand1);

		//make object code
		/* ========================================================================
		 * 1. objcode1 : get first 8 bits of object code = opcode + ni 
						- if addr_mode : opcode & 0(+3), 1(+2), 2(+1) 
		 * 2. objcode2 : get xbpe bits
						- if format4: 1, else if format3, choose relative
						- if indexed mode, +8
		 * 3. objcode3 : get address
						- if format4: direct, else if format3, address-relative
		 ==========================================================================*/
		objcode1 = obj_table[i].opcode + (3 - obj_table[i].addr_mode);

		if(obj_table[i].format == 4){
			objcode2 = 1;
			if(obj_table[i].indexed == 1) objcode2 += 8;
			objcode3 = find_symtab(obj_table[i].operand1);
			obj_table[i].rel_mode = 0; //format4 : relative mode=0

			comb(2, objcode1, obj_table[i].objcode, 0);
			comb(1, objcode2, obj_table[i].objcode, 2);
			comb(5, objcode3, obj_table[i].objcode, 3);
		}
		//if comment or directives, no object code
		else if(obj_table[i].format == 0 || obj_table[i].dir == 1){
			objcode1 = 0;
			objcode2 = 0;
			objcode3 = 0;
			memset(obj_table[i].objcode, '\0', sizeof(obj_table[i].objcode));

			//if BYTE
			if(strcmp(obj_table[i].mnemonic, "BYTE") == 0){
				if(obj_table[i].operand1[0] == 'C'){
					int byte_len = strlen(obj_table[i].operand1) - 3;

					//modified 0416
					//단어 길이만큼 아스키코드로 변
					int obj_idx = 0;
					for(int k=1; k<=byte_len; k++){
						if(byte_len >= k){
							objcode1 = obj_table[i].operand1[k+1];
							comb(2, objcode1, obj_table[i].objcode, obj_idx);
							obj_idx += 2;
						}
					}
				}
				else{
					objcode1 = 0;
					for(int j=2; j<4; j++){
						if('A' <= obj_table[i].operand1[j] && obj_table[i].operand1[j] <= 'F')
							objcode1 += obj_table[i].operand1[j] - 'A' + 10;

						else if('a' <= obj_table[i].operand1[j] && obj_table[i].operand1[j] <= 'f')
							objcode1 += obj_table[i].operand1[j] - 'a' + 10;

						else
							objcode1 += obj_table[i].operand1[j] - '0';

						if(j==2)
							objcode1 *= 16;

						comb(2, objcode1, obj_table[i].objcode, 0);
					}	
				}
			}
			//if WORD
			else if(strcmp(obj_table[i].mnemonic, "WORD") == 0){
					objcode1 = atoi(obj_table[i].operand1);
					comb(6, objcode1, obj_table[i].objcode, 0);
			}
		}

		//if register operation
		else if(obj_table[i].reg == 1){
			objcode1 = obj_table[i].opcode;
			if (strlen(obj_table[i].operand1) > 0) {
				objcode2 = get_regnum(obj_table[i].operand1);
				if (objcode2 < 0) {
					printf("Error occurs : line %d\n", obj_table[i].line_num / 5);
					return -4;
				}
			}
			else objcode2 = 0;
			if (strlen(obj_table[i].operand2) > 0) {
				objcode3 = get_regnum(obj_table[i].operand2);
				if (objcode3 < 0) {
					printf("Error occurs : line %d\n", obj_table[i].line_num / 5);
					return -4;
				}
			}
			else objcode3 = 0;

			comb(2, objcode1, obj_table[i].objcode, 0);
			comb(1, objcode2, obj_table[i].objcode, 2);
			comb(1, objcode3, obj_table[i].objcode, 3);
		}
		else{
			objcode3 = find_relative(i, pc, base);
			if (objcode3 == MAX_LOC) {
				printf("Error occurs : line %d\n", obj_table[i].line_num / 5);
				return -6;
			}

			if (obj_table[i].rel_mode == 1) objcode2 = 2;
			else if (obj_table[i].rel_mode == 2) objcode2 = 4;

			if (obj_table[i].indexed == 1) objcode2 += 8;

			comb(2, objcode1, obj_table[i].objcode, 0);
			comb(1, objcode2, obj_table[i].objcode, 2);
			comb(3, objcode3, obj_table[i].objcode, 3);
		}
	}

	return 0;
}

int chk_operand_error(){
	int flag = 0;

	for(int i=0; i<obj_num; i++){
		if(obj_table[i].format == 0
		|| strcmp(obj_table[i].mnemonic, "START") == 0
		|| strcmp(obj_table[i].mnemonic, "END") == 0
		|| strcmp(obj_table[i].mnemonic, "BYTE") == 0
		|| strcmp(obj_table[i].mnemonic, "WORD") == 0
		|| obj_table[i].reg == 1)
			continue;
		else{
			//if operand is number
			if('0' <= obj_table[i].operand1[0] && obj_table[i].operand1[0] <= '9'){
				if(chk_locctr_error(atoi(obj_table[i].operand1)) < 0){
					printf("Error occurs : line %d\n", obj_table[i].line_num/5);
					return -4;
				}
			}
			else if(strcmp(obj_table[i].mnemonic, "RSUB") == 0){
				if(strlen(obj_table[i].operand1) > 0){
					printf("Error occurs : line %d\n", obj_table[i].line_num / 5);
					return -4;
				}
				continue;
			}
			else{
				flag = 0;
				//check if operand is in symlabel
				for(int j=0; j<symbol_num; j++){
					if(strcmp(symlabel[j], obj_table[i].operand1) == 0){
						flag = 1;
						break;
					}
				}
				if(flag == 0){
					printf("Error occurs : line %d\n", obj_table[i].line_num/5);
					return -4;
				}
			}
		}
	}
	return 1;
}

int pass2(char *lst_file, char *obj_file){
	int error_code = 0;

	//check operand error
	error_code = chk_operand_error();
	if(error_code < 0)
		return error_code;

	//get opcode
	get_opcode();
	
	//make object code
	error_code = make_objCode();
	if(error_code < 0)
		return error_code;

	//make .lst file
	error_code = make_lstFile(lst_file);
	if(error_code < 0) 
		return error_code;

	//make .obj file		
	error_code = make_objFile(obj_file);
	if(error_code < 0)
		return error_code;

	return 0;
}

/*=================================
 * Assemble
 =================================*/
int check_file_format(const char *filename) {
	char tmp_filename[MAX_FILENAME];
	strcpy(tmp_filename, filename);

	char* format = strtok(tmp_filename, ".");

	if (format = strtok(NULL, ".")) {
		if (strcmp(format, "asm") == 0) {
			return 1;
		}
	}

	return -1;
}

void make_filename(int option, char* filename, int len) {
	//option 0: lst file
	//option 1: obj file
	filename[len++] = '.';

	if (option == 0) {
		filename[len++] = 'l';
		filename[len++] = 's';
		filename[len++] = 't';
	}
	else if (option == 1) {
		filename[len++] = 'o';
		filename[len++] = 'b';
		filename[len++] = 'j';
	}
}
void init_objtab(){
	memset(obj_table, 0, MAX_LEN);
	obj_num = 0;
}

void asm_error(int code){
	erase_history();
	switch(code){
		case -1: printf("Assemble Error!!! File open error\n"); break;
		case -2: printf("Assemble Error!!! Duplicated symbol\n"); break;
		case -3: printf("Assemble Error!!! Invalid mnemonic\n"); break;
		case -4: printf("Assemble Error!!! Invalid operand\n"); break;
		case -5: printf("Assemble Error!!! No 'END'\n"); break;
		case -6: printf("Assemble Error!!! Invalid address\n"); break;
		case -7: printf("Assemble Error!!! File format error\n"); break;
		case -8: printf("Assemble Error!!! Invalid label\n"); break;
		default: printf("Assemble Error!!!\n");
	}
}

/* ASSEMBLE */
void assemble(const char *filename) {
	int error_code = 0;
	char tmp_filename[MAX_FILENAME];
	strcpy(tmp_filename, filename);

	char* asm_file = strtok(tmp_filename, "."); //[source filename]
	char lst_file[MAX_FILENAME]; //[source filename].lst
	char obj_file[MAX_FILENAME]; //[source filename].obj
	int len = strlen(asm_file);

	//check if source file is .asm file
	if(check_file_format(filename) == -1){
		asm_error(-7);
		return;
	}

	//1. make filename
	strcpy(lst_file, asm_file);
	strcpy(obj_file, asm_file);
	make_filename(0, lst_file, len);
	make_filename(1, obj_file, len); 

	//2. initialize
	init_symtab(); //symbol.c 
	init_objtab(); 
	program_length = 0;


	//4. pass1,  pass2
	error_code = pass1(filename); 
	if (error_code != 0) {
		asm_error(error_code);
		remove(lst_file);
		remove(obj_file);
		return;
	}
	error_code = pass2(lst_file, obj_file);
	if (error_code != 0) {
		asm_error(error_code); 
		remove(lst_file);
		remove(obj_file);
		return;
	}

	update_ex_symtab(); //symbol.c
	printf("[%s], [%s]\n", lst_file, obj_file);
}
