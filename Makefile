# RVM Makefile.
#

CC=        gcc
RM=        rm -rf

STD=       gnu99
CFLAGS=    -Wall -Wextra
CFLAGS+=   $(MYCFLAGS) -std=$(STD)

SRC=       rvm.c
OBJ=       $(SRC:.c=.o)

ifneq ($(P),)
  CFLAGS+= -pedantic -ansi
endif

default: release

build: $(OBJ)

release: build
release: CFLAGS+= -O3 -march=native -mtune=native \
         -fomit-frame-pointer -funroll-loops -ftree-vectorize

debug:   build
debug:   CFLAGS+= -g -DRVM_DBG_

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test/test.o: STD= c99
test/test: test/test.o $(OBJ)
	$(CC) -o $@ $^
test: release test/test

clean:
	$(RM) $(OBJ) test/test test/test.o

help:
	@echo "RVM Makefile"
	@echo "  build       Build with default flags."
	@echo "  release     Build for release."
	@echo "  debug       Build for debugging."
	@echo "  test        Compile testing loader."
	@echo "  clean       Remove output files."
	@echo "  help        Show this help."
	@echo "Customisation:"
	@echo "  MYCFLAGS=...   Custom compiler flags."
	@echo "  CC=...         C compiler to use. (default: $(CC))"
	@echo "  RM=...         Command to remove files. (default: $(RM))"
	@echo "  STD=...        Specify the standard. (default: $(STD))"
	@echo "  P=1            Force standard conformance."

.PHONY: default build release debug test clean help
