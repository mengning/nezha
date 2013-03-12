# Makefile for Nezha project

TARGETS = testdbapi  testswserver testswclient testprotocol testlinktable \
          nezha libconfigdb.* 
OBJS    = event.o msgq.o engine.o nodes.o socketwrapper.o dbapi.o protocol.o \
        remotedbapi.o configdb.o 

all:	configdb install nezha

# nezha:grid version	
nezha:	cmdline.o client.o
	gcc -o $@ $^ -L. -lconfigdb -ltokyocabinet
	@printf '#=====================================\n'
	@printf '# nezha:grid version\n'
	@printf '# execute ./nezha\n'
	@printf '#=====================================\n'

configdb: $(OBJS)
	ar -r libconfigdb.a $(OBJS)
	ar -s libconfigdb.a
	ranlib libconfigdb.a
	gcc -g -shared -o libconfigdb.so $^ -ltokyocabinet -lc

install:
	cp configdb.h /usr/include
	cp libconfigdb.* /usr/lib

uninstall:
	rm -rf  /usr/include/configdb.h
	rm -rf  /usr/lib/libconfigdb.*

test:   socketwrapper.o testsocketwrapperserver.o testsocketwrapperclient.o 
	gcc -o testswserver socketwrapper.o testsocketwrapperserver.o
	gcc -o testswclient socketwrapper.o testsocketwrapperclient.o
	./testswserver &
	sleep 1
	./testswclient &
	./testswclient &
	./testswclient 
#	killall testswserver



.c.o:
	gcc -fPIC -g -c $<

clean:
	rm -rf *.o $(TARGETS) *.bak *.hdb
