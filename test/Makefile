CFLAGS=-I ../inc -g

test: test.o
	$(CC) -o test test.o ../libpsk.so
	
clean:
	rm -f *.o
	rm -f test pskrecv

pskrecv: pskrecv.o
	$(CC) -o pskrecv pskrecv.o ../libpsk.so -lpulse-simple -lpulse

run: test
	arecord -f s16_le -r 8000 | ./test
