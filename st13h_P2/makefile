CC = cc -g -Wall -pedantic -o

all: assembler simulate

simulate: simulator.c 
	$(CC) simulate simulator.c

assembler: assembler.c
	$(CC) assembler assembler.c

clean:	
	rm assembler simulate