# Makefile for Nezha project

TARGETS = testdbapi nezha

all:	nezha

# nezha:local command line version	
nezha:	dbapi.o cmdline.o
	gcc -o $@ $^ -ltokyocabinet
	@printf '#=====================================\n'
	@printf '# nezha:local command line version\n'
	@printf '# you can execute ./nezha\n'
	@printf '#=====================================\n'

test:   dbapi.o testdbapi.o
	gcc -o testdbapi dbapi.o testdbapi.o -ltokyocabinet
	./testdbapi

.c.o:
	gcc -c $<

clean:
	rm -rf *.o $(TARGETS)
