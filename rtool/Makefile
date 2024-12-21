# Makefile.
#

CC=      gcc
CFLAGS=  -std=gnu99
LDFLAGS= -lm

CFLAGS+=  $(MYCFLAGS)
LDFLAGS+= $(MYLDFLAGS)

OBJ=
TRG= rtool

#
# Essential targets.
#

all: release
build: $(TRG)

CCMD= $(CC) $(CFLAGS) -c -o $@ $<
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
	rm -f $(TRG) $(OBJ) .depend

#
# Build configs.
#

PHONY += debug release

debug: CFLAGS+=  -g -DDEBUG_
debug: LDFLAGS+= -g
debug: build

release: CFLAGS+= -O2
release: build

#
# Print help message about this Makefile.
#

PHONY += help

help:
	@echo 'RTool Makefile'
	@echo
	@echo 'Customisation:'
	@echo '  MYCFLAGS=...   Add custom compiler flags.'
	@echo '  MYLDFLAGS=...  Add custom linker flags.'
	@echo
	@echo 'Build types:'
	@echo '  debug       Support debugging. No optimisations.'
	@echo '  release     Build for release (default).'
	@echo
	@echo 'Utilities:'
	@echo '  help        Show this help.'
	@echo '  depend      Generate the .depend file.'
	@echo '  clean       Remove built files.'

# All PHONY rules goes here.
.PHONY: $(PHONY)
