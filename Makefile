CFLAG := -std=c99 -D_DEFAULT_SOURCE -Wall -g

cpu_test: src/cpu.o src/cpu_test.o
	gcc $(CFLAGS) -Werror -o $@ $^

cpu: cpu.o
	gcc $(CFLAGS) -o $@ $^

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o *.o cpu_test