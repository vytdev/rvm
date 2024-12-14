# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99 -MMD -MP
LDFLAGS= -lm

ifneq ($(origin BENCHMARK), undefined)
  CFLAGS+= -DBENCHMARK_
endif

SRC= main.c util.c mach.c exec.c vmcall.c
OBJ= $(SRC:.c=.c.o)
DEP= $(SRC:.c=.c.d)
TRG= rvm

all: release
build: $(TRG)

-include $(DEP)

$(TRG): $(OBJ)
	@ echo "linking $@..."
	@ $(CC) $(LDFLAGS) -o $@ $^

%.c.o: %.c
	@ echo "$<  ==>  $@"
	@ $(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean debug release perf

clean:
	rm -f $(TRG)
	rm -f $(OBJ)
	rm -f $(DEP)

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+= -O3 -march=native -mtune=native -fomit-frame-pointer
release: build

perf: CFLAGS+=  -DPERF_ -flto -funroll-loops -ftree-vectorize
perf: LDFLAGS+= -flto -O3
perf: release
