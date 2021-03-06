# thread-runner build config options. Set the variables
# as desired.

MAINOBJS = trunner.o

## Set BITS to 64 to install to lib64/
BITS=
CC=clang
LINK=cc
OPTIM=-O0
DEBUG=-g -Wall -Wextra
FLAGS=-std=c99 $(OPTIM) $(DEBUG)
LFLAGS=$(OPTIM) $(DEBUG)

PREFIX=.
LIBDIR=$(PREFIX)/lib$(BITS)/
INCLUDEDIR=$(PREFIX)/include/
