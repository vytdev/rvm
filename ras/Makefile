# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99
LDFLAGS=

CFLAGS+=  $(MYCFLAGS)
LDFLAGS+= $(MYLDFLAGS)

OBJ= main.o lexer.o util.o rvm_util.o
TRG= ras

all: release
build: $(TRG)

-include .depend

$(TRG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: depend clean debug release

depend:
	$(CC) -MM *.c > .depend

clean:
	rm -f $(TRG) $(OBJ)

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+= -O2 -march=native -mtune=native
release: build

# Other dependencies goes here.

COMP= $(CC) $(CFLAGS) -c -o $@ $<

rvm_util.o: ../rvm/util.c ; $(COMP)
