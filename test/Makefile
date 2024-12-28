CC=      gcc
RM=      rm -f

CFLAGS=  -std=gnu99
LDFLAGS=

OBJ= $(obj-tests) $(obj-runner) register_tests.o $(patsubst %.o,../src/%.o,$(obj-core))
TRG= test_exec

#
# Test obj defs.
#

obj-tests= \
    util.test.o

obj-runner= \
    runner/core.o \
    runner/main.o \
    runner/util.o

include ../src/List.mk

#
# Target rules.
#

PHONY+= run-test build-test

all: run-test

run-test: $(TRG)
	./$(TRG)

build-test: $(TRG)

$(TRG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^



runner/%.o: runner/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

register_tests.o: register_tests.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.test.o: %.test.c
	$(CC) $(CFLAGS) -DTESTF_ -c -o $@ $<

../src/%.o: ../src/%.c
	$(MAKE) -C../src -j8 release

#
# Utilities.
#

PHONY+= clean

clean:
	$(RM) $(obj-tests) $(obj-runner) register_tests.o $(TRG)

.PHONY: $(PHONY)
