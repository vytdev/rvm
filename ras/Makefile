# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99 -MMD -MP
LDFLAGS=

SRC= main.c
OBJ= $(SRC:.c=.c.o)
DEP= $(SRC:.c=.c.d)
TRG= ras

all: release
build: $(TRG)

-include $(DEP)

$(TRG): $(OBJ)
	@ echo "linking $@..."
	@ $(CC) $(LDFLAGS) -o $@ $^

%.c.o: %.c
	@ echo "$<  ==>  $@"
	@ $(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean debug release

clean:
	rm -f $(TRG)
	rm -f $(OBJ)
	rm -f $(DEP)

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+= -O2 -march=native -mtune=native
release: build
