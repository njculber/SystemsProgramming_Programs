CC = gcc -g -std=c99
CFLAGS = -Wall -Wextra
SOURCE1 = client.c
SOURCE2 = server.c
OBJECT1 = -o client
OBJECT2 = -o server
sOBJECT1 = client
sOBJECT2 = server

all: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE1) $(OBJECT1)
	$(CC) $(CFLAGS) $(SOURCE2) $(OBJECT2)

clean:
	rm -f $(sOBJECT1)
	rm -f $(sOBJECT2)
