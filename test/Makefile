CFLAGS=-I ../inc -g

all: test pskrecv psktrans

test: test.o
	$(CC) -o $@ $< ../libpsk.so

pskrecv: pskrecv.o
	$(CC) -o $@ $< ../libpsk.so -lpulse -lpulse-simple

psktrans: psktrans.o
	$(CC) -o $@ $< ../libpsk.so -lpulse -lpulse-simple

clean:
	rm -f *.o
	rm -f test pskrecv psktrans

run: test
	arecord -f s16_le -r 8000 | ./test
