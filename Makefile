main.out : main.o command.o shell.o memory.o opcode.o assembler.o symbol.o linking_loader.o
	gcc -W -o main.out main.o command.o shell.o memory.o opcode.o  assembler.o symbol.o linking_loader.o

main.o : main.c main.h
	gcc -W -c main.c

command.o : command.c
	gcc -W -c command.c

shell.o : shell.c
	gcc -W -c shell.c

memory.o : memory.c
	gcc -W -c memory.c

opcode.o : opcode.c
	gcc -W -c opcode.c

assembler.o : assembler.c
	gcc -W -c assembler.c

symbol.o : symbol.c
	gcc -W -c symbol.c

linking_loader.o : linking_loader.c
	gcc -W -c linking_loader.c

clean : 
	rm *.o main.out
