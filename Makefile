all:main

CC=gcc
CFLAGS=-g -std=c99 -Wall -Wno-unused -I/usr/local/include/leveldb/
LDFALGS=-L/usr/local/lib/ -lleveldb

main:main.o csas.o\
	inmemory-engine.o\
	leveldb-engine.o\
	sqlite-engine.o\
	example-engine.o
	$(CC) $(LDFALGS) -o $@ $^

main.o:main.c

csas.o:csas.c

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
	rm main main.o csas.o\
		inmemory-engine.o\
		leveldb-engine.o\
		sqlite-engine.o\
		example-engine.o
