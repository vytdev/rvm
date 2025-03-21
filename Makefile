#
# rvm -- A high-performance abstract virtual machine.
# Copyright (C) 2025  Vincent Yanzee J. Tan
#

VERSION=   0

# Tools to use.
CC=        gcc
RM=        rm -f

# Required dynamic libraries.
DYNLIBS=   -lm

# Output names (with './' prefix).
TRG=       ./rvm
T-TRG=     ./test/test

# Flags for all.
CFLAGS=    -std=gnu99
LDFLAGS=
# For the source code.
ICFLAGS=   -DRVMVER=$(VERSION)
ILDFLAGS=
# For the test code.
TCFLAGS=
TLDFLAGS=
# Test parameters.
TESTARGS=

#
# DO NOT CHANGE ANYTHING AFTER THIS LINE.
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#

# If B=1 is set.
ifneq ($(B),)
  ICFLAGS+= -DBENCHMARK_
endif

# CLI customisation.
CFLAGS+=   $(MYCFLAGS)
LDFLAGS+=  $(MYLDFLAGS)
TESTARGS+= $(MYTESTARGS)
# The general flags.
ICFLAGS+=  $(CFLAGS)
ILDFLAGS+= $(LDFLAGS)
TCFLAGS+=  $(CFLAGS)
TLDFLAGS+= $(LDFLAGS)

all: release
build: $(TRG)

include List.mk
-include .depend

#
#
# Build targets.
# ==============
#

# The usable executable.
$(TRG): $(OBJ)
	$(CC) $(ILDFLAGS) -o $@ $^ $(DYNLIBS)

# For testing.
$(T-TRG): $(T-OBJ)
	$(CC) $(TLDFLAGS) -o $@ $^ $(src-obj) $(DYNLIBS)

# Default C rule.
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# For the source code.
src/%.o: src/%.c
	$(CC) $(ICFLAGS) -c -o $@ $<

# For the testing files.
test/%.test.o: test/%.test.c
	$(CC) $(TCFLAGS) -DTESTF_ -c -o $@ $<

#
#
# Utilities and build types.
# ==========================
#

PHONY= depend clean test debug release help

depend:
	$(CC) -MM $(shell find . -name '*.c' -type f) > .depend

clean:
	$(RM) $(TRG) $(T-TRG) $(OBJ) $(T-OBJ) .depend

test: $(T-TRG)
	$(T-TRG) $(TESTARGS)

debug: ICFLAGS+=  -g -DDEBUG_
debug: ILDFLAGS+= -g
debug: build

release: ICFLAGS+=  -flto -O3 -march=native -mtune=native \
         -fomit-frame-pointer -funroll-loops -ftree-vectorize
release: ILDFLAGS+= -flto -O3
release: build

help:
	@echo 'RVM Makefile'
	@echo
	@echo 'Options:'
	@echo '  B=1         Enable benchmarking.'
	@echo
	@echo 'Customisation:'
	@echo '  MYCFLAGS=...   Add custom compiler flags.'
	@echo '  MYLDFLAGS=...  Add custom linker flags.'
	@echo '  MYTESTARGS=... Add args to the test suite.'
	@echo
	@echo 'Build types:'
	@echo '  build       Normal full source build.'
	@echo '  debug       Support debugging. No optimisations.'
	@echo '  release     Build for release (default).'
	@echo
	@echo 'Utilities:'
	@echo '  help        Show this help.'
	@echo '  depend      Generate the .depend file.'
	@echo '  clean       Remove built files.'
	@echo '  test        Do tests (run a build first).'

# All PHONY rules goes here.
.PHONY: $(PHONY)
