# Makefile for Nezha project

all:	nezha

nezha:	dbapi.o cmdline.o main.o
	gcc -o nezha dbapi.o cmdline.o main.o -ltokyocabinet

.c.o:
	gcc -c $<

clean:
	rm *.o nezha
