CFLAG := -std=c99 -D_DEFAULT_SOURCE -Wall -g

cpu_test: opcode.o cpu.o cpu_test.o
	$(CC) -Wall -o cpu_test opcode.o cpu.o cpu_test.o

opcode.o: opcode.h opcode.c
cpu.o: cpu.h cpu.c
cpu_test.o: cpu.h cpu_test.c

clean:
	del /Q /F cpu_test.exe *.o