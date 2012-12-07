# Makefile for Nezha project

all:	nezha

nezha:	dbapi.o main.o
	gcc -o $@ $^ -ltokyocabinet

.c.o:
	gcc -c $<

clean:
	rm *.o nezha
