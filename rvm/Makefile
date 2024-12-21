# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99
LDFLAGS= -lm

CFLAGS+=  $(MYCFLAGS)
LDFLAGS+= $(MYLDFLAGS)

ifneq ($(origin BENCHMARK), undefined)
  CFLAGS+= -DBENCHMARK_
endif

OBJ= bcode.o exec.o mach.o main.o util.o vmcall.o
TRG= rvm

all: release
build: $(TRG)

-include .depend

$(TRG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: depend clean debug release perf

depend:
	$(CC) -MM *.c > .depend

clean:
	rm -f $(TRG) $(OBJ)

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+=  -flto -O3 -march=native -mtune=native \
	-fomit-frame-pointer -funroll-loops -ftree-vectorize
release: LDFLAGS+= -flto -O3
release: build

perf: CFLAGS+= -DPERF_
perf: release
