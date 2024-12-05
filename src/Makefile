# Makefile.
#

CC=      gcc
CFLAGS=  -std=c99 -MMD -MP
LDFLAGS= -lm

RVM_S= rvm_main.c util.c mach.c
RVM_O= $(RVM_S:.c=.c.o)
RVM_D= $(RVM_S:.c=.c.d)
RVM_T= rvm

all: release
build: $(RVM_T)

$(RVM_T): $(RVM_O)
	$(CC) $(LDFLAGS) -o $@ $^

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean debug release

clean:
	rm -f $(RVM_T)
	rm -f $(RVM_O)
	rm -f $(RVM_D)

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+= -O2
release: build
