# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99 -MMD -MP
LDFLAGS= -lm

ifneq ($(origin BENCHMARK), undefined)
  CFLAGS+= -DBENCHMARK_
endif

RVM_S= rvm_main.c util.c mach.c
RVM_O= $(RVM_S:.c=.c.o)
RVM_D= $(RVM_S:.c=.c.d)
RVM_T= rvm

all: release
build: $(RVM_T)

$(RVM_T): $(RVM_O)
	@ echo "linking $@..."
	@ $(CC) $(LDFLAGS) -o $@ $^

%.c.o: %.c
	@ echo "$<  ==>  $@"
	@ $(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean debug release perf

clean:
	rm -f $(RVM_T)
	rm -f $(RVM_O)
	rm -f $(RVM_D)

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+= -O3 -march=native -mtune=native -fomit-frame-pointer
release: build

perf: CFLAGS+=  -DPERF_ -flto -funroll-loops -frename-registers
perf: LDFLAGS+= -flto
perf: release
