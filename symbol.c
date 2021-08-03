/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : SIC/XE machine [project 2]					 *
 * FIle name    : symbol.c										 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 03 - 31								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

/*==================================
 * SYMTAB
 ==================================*/
void init_symtab() {
	for (int i = 0; i < MAX_SYMTAB; i++) {
		sym_table[i] = malloc(sizeof(symlist));
		sym_table[i]->address = -1;
		sym_table[i]->link = NULL;
		/*sym[i] = malloc(sizeof(symlist));
		sym[i]->address = -1;
		sym[i]->link = NULL;*/
	}

	memset(symlabel, 0, sizeof(symlabel));
	symbol_num = 0;
	/*memset(label, 0, sizeof(label));
	*num = 0;*/
}

void init_ex_symtab() {
	for (int i = 0; i < MAX_SYMTAB; i++) {
		ex_sym_table[i] = malloc(sizeof(symlist));
		ex_sym_table[i]->address = -1;
		ex_sym_table[i]->link = NULL;
	}

	memset(ex_symlabel, 0, sizeof(ex_symlabel));
	ex_symbol_num = 0;
}

int symtab_hash(const char* label) {
	int hash = 301;

	for (int i = 0; i < (int)strlen(label); i++)
		hash = ((hash << 4) + (int)(*label)) % MAX_SYMTAB;

	return hash % MAX_SYMTAB;
}

int push_symtab(const char* label, int locctr) {

	symlist* cur = malloc(sizeof(symlist));
	strcpy(cur->symbol, label);
	cur->address = locctr;

	strcpy(symlabel[symbol_num++], label);

	int idx = symtab_hash(label); //get hash

	if (sym_table[idx]->link == NULL) {
		//if sym_table[idx] is empty
		sym_table[idx]->link = cur;
		cur->link = sym_table[idx];
	}
	else {
		//check if duplicated label
		symlist* tmp = sym_table[idx]->link;
		while (tmp->address != -1) {
			if (strcmp(cur->symbol, tmp->symbol) == 0)
				return -1;
			tmp = tmp->link;
		}

		cur->link = sym_table[idx]->link;
		sym_table[idx]->link = cur;
	}

	return 1;
}

void copy_symtab(const char* label, int locctr) {
	symlist* cur = malloc(sizeof(symlist));
	strcpy(cur->symbol, label);
	cur->address = locctr;

	int idx = symtab_hash(label);

	if (ex_sym_table[idx]->link == NULL) {
		ex_sym_table[idx]->link = cur;
		cur->link = ex_sym_table[idx];
	}
	else {
		cur->link = ex_sym_table[idx]->link;
		ex_sym_table[idx]->link = cur;
	}
}

int find_symtab(const char* symbol) {
	int idx = symtab_hash(symbol);
	symlist* cur = sym_table[idx]->link;

	if ('0' <= symbol[0] && symbol[0] <= '9') {
		return atoi(symbol);
	}

	if (cur) {
		while (cur->address != -1) {
			if (strcmp(cur->symbol, symbol) == 0)
				return cur->address;
			cur = cur->link;
		}
	}
	return -1;
}

int find_ex_symtab(const char* symbol) {
	int idx = symtab_hash(symbol);
	symlist* cur = ex_sym_table[idx]->link;

	if ('0' <= symbol[0] && symbol[0] <= '9')
		return atoi(symbol);

	if (cur) {
		while (cur->address != -1) {
			if (strcmp(cur->symbol, symbol) == 0)
				return cur->address;
			cur = cur->link;
		}
	}
	return -1;
}

void delete_symtab(symlist *sym) {
	for (int i = 0; i < MAX_SYMTAB; i++) {
		symlist* sym = sym_table[i]->link;
		//symlist* ex_sym = ex_sym_table[i]->link;

		//free symtab
		if (sym == NULL) {
			free(sym_table[i]);
		}
		else {
			while (sym->address != -1) {
				symlist* tmp = sym->link;
				free(sym);
				sym = tmp;
			}

			free(sym_table[i]);
		}

		//free ex_symtab
		/*if (ex_sym == NULL) {
			free(ex_sym_table[i]);
		}
		else {
			while (ex_sym->address != -1) {
				symlist* tmp = ex_sym->link;
				free(ex_sym);
				ex_sym = tmp;
			}
			free(ex_sym_table[i]);
		}*/
	}
}

void update_ex_symtab() {
	init_ex_symtab();
	ex_symbol_num = symbol_num;

	for (int i = 0; i < symbol_num; i++) {
		strcpy(ex_symlabel[i], symlabel[i]);
		copy_symtab(symlabel[i], find_symtab(symlabel[i]));
	}
}

/*====================================
 * SYMBOL
 ====================================*/
void symbol() {
	//1. sort EX_SYMLABEL -> alphabet ASC
	for (int i = 0; i < ex_symbol_num - 1; i++) {
		for (int j = 0; j < ex_symbol_num - 1 - i; j++) {
			if (strcmp(ex_symlabel[j], ex_symlabel[j + 1]) > 0) {
				char tmp[10];
				strcpy(tmp, ex_symlabel[j]);
				strcpy(ex_symlabel[j], ex_symlabel[j + 1]);
				strcpy(ex_symlabel[j + 1], tmp);
			}
		}
	}

	//2. get hash of EX_SYMLABEL[i], find in the ex_sym_table, print symbol and address
	for (int i = 0; i < ex_symbol_num; i++) {
		int locctr = find_ex_symtab(ex_symlabel[i]);
		printf("\t%s\t%04X\n", ex_symlabel[i], locctr);
	}

	if (ex_symbol_num == 0)
		printf("Error!!! There is no SYMTAB\n");
}
