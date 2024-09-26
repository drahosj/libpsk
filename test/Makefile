CFLAGS=-I ../inc

test: test.o
	$(CC) -o test test.o ../libpsk.so
	
clean:
	rm -f *.o
	rm -f test

run: test
	arecord -f s16_le -r 8000 | ./test
