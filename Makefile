# Makefile for Nezha project

TARGETS = testdbapi  testswserver testswclient testprotocol \
          nezha nezhaserver nezhaclient

all:	nezha nezhaserver nezhaclient

# nezhaserver:database service
nezhaserver:dbapi.o socketwrapper.o protocol.o dbserver.o \
            event.o msgq.o cmdline.o
	gcc -o $@ $^ -ltokyocabinet

# nezhaclient:database remote command line
nezhaclient:socketwrapper.o protocol.o remotedbapi.o clouddbapi.o cmdline.o client.o
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
		protocol.o testprotocol.o
	gcc -o testdbapi dbapi.o testdbapi.o -ltokyocabinet
	./testdbapi
	gcc -o testswserver socketwrapper.o testsocketwrapperserver.o
	gcc -o testswclient socketwrapper.o testsocketwrapperclient.o
	./testswserver &
	sleep 1
	./testswclient
	killall testswserver
	gcc -o testprotocol protocol.o testprotocol.o
	./testprotocol

.c.o:
	gcc -c $<

clean:
	rm -rf *.o $(TARGETS) *.bak
