all:server

CC=gcc
CFLAGS=-g -Wall -std=c99 -Wno-unused -I./include/ 
LDFALGS=-L/usr/local/lib/ -lleveldb -levent\
		-L./lib/ -levhtp\
        -lpthread\

server:server.o\
    csas.o\
	inmemory-engine.o\
	leveldb-engine.o
	$(CC) -o $@ $^ $(LDFALGS)

server.o:server.c
	$(CC) -o $@ -c $^ $(CFLAGS)

csas.o:csas.c
	$(CC) -o $@ -c $^ $(CFLAGS)

inmemory-engine.o:HashMap-engine/inmemory-engine.c
	$(CC) $(CFLAGS) -o $@ -c $^

leveldb-engine.o:NoSQL-engine/leveldb-engine.c
	$(CC) $(CFLAGS) -o $@ -c $^


.PHONY:clean

clean:
	rm server server.o\
        csas.o\
		inmemory-engine.o\
		leveldb-engine.o
