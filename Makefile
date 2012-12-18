# Makefile for Nezha project

TARGETS = testdbapi nezha testswserver testswclient

all:	nezha

# nezha:local command line version	
nezha:	dbapi.o cmdline.o
	gcc -o $@ $^ -ltokyocabinet
	@printf '#=====================================\n'
	@printf '# nezha:local command line version\n'
	@printf '# you can execute ./nezha\n'
	@printf '#=====================================\n'

test:   dbapi.o testdbapi.o socketwrapper.o testsocketwrapperserver.o testsocketwrapperclient.o
	gcc -o testdbapi dbapi.o testdbapi.o -ltokyocabinet
	./testdbapi
	gcc -o testswserver socketwrapper.o testsocketwrapperserver.o
	gcc -o testswclient socketwrapper.o testsocketwrapperclient.o
	./testswserver &
	./testswclient
	killall testswserver

.c.o:
	gcc -c $<

clean:
	rm -rf *.o $(TARGETS) *.bak
