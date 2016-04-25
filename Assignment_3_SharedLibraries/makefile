CC = gcc -g -std=c99
CFLAGS = -Wall -Wextra
SOURCE = libmtu.c
OBJECT = -o testlib
LDFLAGS = -shared

all: testlibmtu

libmtu.a:
	$(CC) $(CFLAGS) -c $(SOURCE)
	ar cr libmtu.a libmtu.o

libmtu.so:
	$(CC) $(CFLAGS) -fPIC -c $(SOURCE)
	$(CC) -shared -o libmtu.so libmtu.o

testlibmtu: libmtu.so testlibmtu.c
	rm -f libmtu.a
	$(CC) $(CFLAGS) -o testlibmtu testlibmtu.c -L. -lmtu
	
clean: $(SOURCE)
	rm -f libmtu.a libmtu.so *.o testlibmtu
