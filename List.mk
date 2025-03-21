src-obj=  $(src-files:.c=.o)
src-files= \
    src/bcode.c \
    src/exec.c \
    src/mach.c \
    src/util.c \
    src/vmcall.c

test-obj= $(test-files:.c=.o)
test-files= \
    test/util.test.c

OBJ=   src/main.o \
       $(src-obj)

T-OBJ= test/runner/main.o \
       test/runner/core.o \
       test/runner/util.o \
       test/register.o    \
       $(test-obj)
