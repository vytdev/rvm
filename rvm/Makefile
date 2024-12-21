# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99
LDFLAGS= -lm

CFLAGS+=  $(MYCFLAGS)
LDFLAGS+= $(MYLDFLAGS)

OBJ= bcode.o exec.o mach.o main.o util.o vmcall.o
TRG= rvm

#
# Options.
#

ifneq ($(B),)
  CFLAGS+= -DBENCHMARK_
endif

#
# Essential targets.
#

all: release
build: $(TRG)

-include .depend

$(TRG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#
# Utilities.
#

PHONY += depend clean

depend:
	$(CC) -MM $(shell find . -name '*.c' -type f) > .depend

clean:
	rm -f $(TRG) $(OBJ) .depend

#
# Build configs.
#

PHONY += debug release perf

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+=  -flto -O3 -march=native -mtune=native \
	-fomit-frame-pointer -funroll-loops -ftree-vectorize
release: LDFLAGS+= -flto -O3
release: build

perf: CFLAGS+= -DPERF_
perf: release

#
# Print help message about this Makefile.
#

PHONY += help

help:
	@echo 'RVM Makefile'
	@echo
	@echo 'Options:'
	@echo '  B=1         Enable benchmarking.'
	@echo
	@echo 'Customisation:'
	@echo '  MYCFLAGS=...   Add custom compiler flags.'
	@echo '  MYLDFLAGS=...  Add custom linker flags.'
	@echo
	@echo 'Build types:'
	@echo '  debug       Support debugging. No optimisations.'
	@echo '  release     Build for release (default).'
	@echo '  perf        Enable aggressive in-code optimisations (use'
	@echo '              with caution!).'
	@echo
	@echo 'Utilities:'
	@echo '  help        Show this help.'
	@echo '  depend      Generate the .depend file.'
	@echo '  clean       Remove built files.'

# All PHONY rules goes here.
.PHONY: $(PHONY)
