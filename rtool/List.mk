# List.mk
#
# This file contains a list of what should
# be compiled.

#
# General modules.
#
OBJ+= util.o

OBJ+= rvm_util.o
rvm_util.o: ../rvm/util.c ; $(CCMD)

#
# Assembler.
#
OBJ+= asm/main.o \
      asm/lexer.o
