# Makefile.
#

CC=      gcc
RM=      rm -f

CFLAGS=  -std=gnu99
LDFLAGS= -lm

CFLAGS+=  $(ICFLAGS) $(MYCFLAGS)
LDFLAGS+= $(ILDFLAGS) $(MYLDFLAGS)

OBJ= main.o $(obj-core)
TRG= rvm

#
# Options.
#

ifneq ($(B),)
  ICFLAGS+= -DBENCHMARK_
endif

#
# Essential targets.
#

all: release
build: $(TRG)

include List.mk
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
	$(RM) $(TRG) $(OBJ) .depend

#
# Build configs.
#

PHONY += debug release perf

debug: ICFLAGS+=  -g -DDEBUG_
debug: ILDFLAGS+= -g
debug: build

release: ICFLAGS+=  -flto -O3 -march=native -mtune=native \
	-fomit-frame-pointer -funroll-loops -ftree-vectorize
release: ILDFLAGS+= -flto -O3
release: build

perf: ICFLAGS+= -DPERF_
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
