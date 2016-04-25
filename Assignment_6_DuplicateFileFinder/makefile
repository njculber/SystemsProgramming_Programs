CC = gcc -g -std=c99 -lcrypto -lssl
CFLAGS = -Wall -Wextra
SOURCE = finddup.c md5sum.c
OBJECT1 = -o finddup
OBJECT = finddup

all: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(OBJECT1)

clean:
	rm -f $(OBJECT)
