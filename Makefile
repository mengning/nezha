# Makefile for Nezha project

TARGETS = testdbapi  testswserver testswclient testprotocol testlinktable \
          nezha

all:	nezha

# nezha:grid version	
nezha:	configdb.o cmdline.o client.o
	gcc -o $@ $^ -ltokyocabinet
	@printf '#=====================================\n'
	@printf '# nezha:grid version\n'
	@printf '# execute ./nezha\n'
	@printf '#=====================================\n'

test:   dbapi.o testdbapi.o \
		socketwrapper.o testsocketwrapperserver.o testsocketwrapperclient.o \
		protocol.o testprotocol.o \
		linktable.o testlinktable.o
	gcc -o testprotocol protocol.o testprotocol.o
	./testprotocol
	gcc -o testlinktable linktable.o testlinktable.o
	./testlinktable
	gcc -o testdbapi dbapi.o testdbapi.o -ltokyocabinet
	./testdbapi
	gcc -o testswserver socketwrapper.o testsocketwrapperserver.o
	gcc -o testswclient socketwrapper.o testsocketwrapperclient.o
	./testswserver &
	sleep 1
	./testswclient
	killall testswserver



.c.o:
	gcc -c $<

clean:
	rm -rf *.o $(TARGETS) *.bak
