all: run


run: calc.o asm.o
	gcc -g -Wall -o run calc.o asm.o

calc.o: calc.c
	gcc -g -Wall -c -o calc.o calc.c


asm.o: asm.s
	nasm -g -f elf64 -w+all -o asm.o asm.s


#tells make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o run
