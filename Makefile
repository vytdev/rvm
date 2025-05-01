# RVM Makefile.
#

CC=        gcc
RM=        rm -rf

STD=       gnu99
CFLAGS=    -Wall -Wextra -pedantic
CFLAGS+=   $(MYCFLAGS) -std=$(STD)

SRC=       rvm.c
OBJ=       $(SRC:.c=.o)

default: release

build: $(OBJ)

release: build
release: CFLAGS+= -O3 -march=native -mtune=native \
         -fomit-frame-pointer -funroll-loops -ftree-vectorize

debug:   build
debug:   CFLAGS+= -g -DRVM_DBG_

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ)

help:
	@echo "RVM Makefile"
	@echo "  build       Build with default flags."
	@echo "  release     Build for release."
	@echo "  debug       Build for debugging."
	@echo "  clean       Remove output files."
	@echo "  help        Show this help."
	@echo "Customisation:"
	@echo "  MYCFLAGS=...   Custom compiler flags."
	@echo "  CC=...         C compiler to use. (default: $(CC))"
	@echo "  RM=...         Command to remove files. (default: $(RM))"
	@echo "  STD=...        Specify the standard. (default: $(STD))"

.PHONY: default build release debug clean help
