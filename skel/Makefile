CC=gcc
# CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
HASH=hashtable_impl

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(HASH).o
	$(CC) $^ -o $@

main.o: main.c utils.h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(HASH).o: $(HASH).c $(HASH).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
