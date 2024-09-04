CC= gcc
LD= gcc
CCFLAGS= -c -O2 -march=native -std=c99 -Wall -Wextra -Werror -pedantic \
    -Wno-gnu-zero-variadic-macro-arguments
LDFLAGS=
INCS=
LIBS=

build: tlc

clean:
	rm -f *.o tlc

install: build
	sudo cp tlc /usr/local/bin

uninstall:
	sudo rm /usr/local/bin/tlc

tlc: tlc.o error.o args.o file.o config.o tokens.o parser.o
	$(LD) -o $@ $^ $(LIBS) $(LDFLAGS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $< $(INCS)

tlc.o: tlc.h args.h file.h config.h tokens.h parser.h
error.o: error.h
args.o: args.h error.h args.hash.c
file.o: file.h error.h
config.o: config.h error.h file.h case.h config.hash.c
tokens.o: tokens.h error.h file.h tokens.hash.c
parser.o: parser.h error.h file.h config.h tokens.h case.h

.PHONY: build clean install uninstall
