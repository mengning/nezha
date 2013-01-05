# Makefile for Nezha project

TARGETS = testdbapi  testswserver testswclient testprotocol testlinktable \
          nezha nezhaserver nezhaclient

all:	nezha nezhaserver nezhaclient

# nezhaserver:database service
nezhaserver:dbapi.o socketwrapper.o protocol.o engine.o server.o \
            event.o msgq.o cmdline.o nodes.o linktable.o
	gcc -o $@ $^ -ltokyocabinet

# nezhaclient:database remote command line
nezhaclient:socketwrapper.o protocol.o remotedbapi.o linktable.o nodes.o clouddbapi.o cmdline.o client.o
	gcc -o $@ $^
	@printf '#=====================================\n'
	@printf '# nezha:C/S version\n'
	@printf '# execute ./nezhaserver & ./nezhaclient\n'
	@printf '#=====================================\n'

# nezha:local command line version	
nezha:	dbapi.o cmdline.o client.o
	gcc -o $@ $^ -ltokyocabinet
	@printf '#=====================================\n'
	@printf '# nezha:local command line version\n'
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
