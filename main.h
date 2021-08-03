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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000
#define MAX_TOKEN 10
#define MAX_ADDR 0x100000
#define MAX_VAL 0x100
#define MAX_TABLE 20
#define MAX_FILENAME 30
#define MAX_SYMTAB 50
#define MAX_ASM 30
#define MAX_LOC 0x10000
#define MAX_FILE_NUM 3
#define MAX_ESTAB 30

char token[MAX_TOKEN][MAX_LEN];
char command[MAX_LEN];

typedef struct _list{ //linked list for history
	char cmd[MAX_LEN];
	struct _list *link;
} list;

void print_error(int);

/* shell.c */
void help();
void dir();
void history(list*);
void type(char*);

/* command.c */
int token_command();
int check_comma(int, int, int);

int check_command(int, int*, int*, int*);
int check_format(char*, int*, int, int);
int check_start_end(int, int);
int check_mnemonic(char*);
int check_fopen(char*);
int turn_hexa(char*);

void toUpper(int);
void push_history(int);
void erase_history();

void run(int, int, int, int);

void delete_history();

/* memory.c */
int mem[MAX_ADDR];

void dump(int, int);
void edit(int, int);
void fill(int, int, int);
void reset();

/* opcode.c */
typedef struct _oplist{	//linked list for opcode
	int opcode;				//value
	char instruction[10];	//key
	char format[5];
	struct _oplist *link;
} oplist;

oplist *hash_table[MAX_TABLE];
char mnemonic[58][10];

void init_hashtable();
void make_hashtable();
int hash_func(const char*);
void opcode_mnemonic(const char*);
void opcodelist();
void delete_hashtable();

/* symbol.c */
typedef struct _symlist{
	char symbol[MAX_ASM];
	int address;
	struct _symlist *link;
} symlist;

symlist *sym_table[MAX_SYMTAB];
symlist *ex_sym_table[MAX_SYMTAB];
char symlabel[MAX_SYMTAB][MAX_ASM];
char ex_symlabel[MAX_SYMTAB][MAX_ASM];
int symbol_num;
int ex_symbol_num;

void symbol();
void init_symtab();
void init_ex_symtab();
int push_symtab(const char*, int);
void copy_symtab(const char*, int);
int find_symtab(const char*);
int find_ex_symtab(const char*);
int symtab_hash(const char*);
void delete_symtab();
void update_ex_symtab();


/* assembler.c */
typedef struct _asmlist{
	int line_num;
	int locctr;
	int address;
	int reg; //if register operation, set 1
	int dir; //if mnemonic is directive, set 1
	char label[MAX_ASM]; //.asm file
	char comment[100]; //if format=0, store comment
	int format; //0:comment, 3:format3, 4:format4
	int addr_mode; //0:simple, 1:indirect, 2:immediate
	int rel_mode; //0:direct, 1:pc relative, 2:base relative
	int indexed; //if indexed mode, set 1
	char mnemonic[MAX_ASM]; //.asm file
	int opcode; //value in opcode hash_table
	char operand1[MAX_ASM]; //.asm file
	char operand2[MAX_ASM];
	char objcode[30];
} asmlist;

asmlist obj_table[MAX_LEN];
int obj_num;
int program_length;

void assemble();
int check_file_format(const char*);
void make_filename(int, char*, int);
void init_objtab();
void asm_error(int);

int pass1(const char*);
int token_file(const char*, char*, char*, char*, char*, int*, int*);
int chk_label_error(const char*);
int chk_mnemonic_error(const char*);
int is_register(const char*);
int chk_byte_error(const char*);
int chk_word_error(const char*);
int chk_res_error(const char*, int);
int chk_address(const char*, const char*, const char*, int);
int is_indexed(const char*);
int chk_locctr_error(int);

int pass2(char*, char*);
int chk_operand_error();
void get_opcode();
int make_objCode();
int get_regnum(const char*);
int find_relative(int, int, int);
void comb(int, int, char*, int);
int make_lstFile(const char*);
int make_objFile(const char*);


/* linking_loader.c */
typedef struct _estab{
	int control_section; //0 : symbol, 1 : control section
	char symbol_name[30];
	int address;
	int length; //if control section, put length
	struct _estab *link;
}estab;

estab *es_table[MAX_ESTAB];
char external_symbol[MAX_ESTAB][30];
int external_symbol_num;

void print_liking_loader_error(int);
void init_reg();
void init_estab();
void delete_estab();
int estab_hash(const char*);
int push_estab(const char*, int, int, int);
int find_estab(const char*);
int char_to_hexa(char);
int loader_pass1(char file[MAX_FILE_NUM][MAX_FILENAME]);
int loader_pass2(char file[MAX_FILE_NUM][MAX_FILENAME]);
void loader(char*, char*, char*);
void progaddr(int);
void bp(int);
void bp_clear();
void print_register();
int get_format(int, int, int);
void get_register(int, int*, int*);
void disassemble(int, int, int, int);
int execute(int);
void run_program();
