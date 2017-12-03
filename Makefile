all:server

CC=gcc
CFLAGS=-g -std=c99 -Wall -Wno-unused -I/usr/local/include/leveldb/ -I/usr/local/include/evhtp/
LDFALGS=-L/usr/lib -lpthread -L/usr/local/lib -lleveldb -levent -levhtp

server:server.o\
    csas.o\
	inmemory-engine.o\
	leveldb-engine.o\
	sqlite-engine.o\
	example-engine.o
	$(CC) -o $@ $^ $(LDFALGS)

server.o:server.c
	$(CC) -o $@ -c $^ $(CFLAGS)

csas.o:csas.c
	$(CC) -o $@ -c $^ $(CFLAGS)

inmemory-engine.o:inmemory-engine.c
	$(CC) $(CFLAGS) -o $@ -c $^

leveldb-engine.o:leveldb-engine.c
	$(CC) $(CFLAGS) -o $@ -c $^

sqlite-engine.o:sqlite-engine.c
	$(CC) $(CFLAGS) -o $@ -c $^

example-engine.o:example-engine.c
	$(CC) -o $@ -c $^


.PHONY:clean

clean:
	rm server server.o\
        csas.o\
		inmemory-engine.o\
		leveldb-engine.o\
		sqlite-engine.o\
		example-engine.o
