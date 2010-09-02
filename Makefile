# Implementace interpretu imperativniho jazyka IFJ07
#
# Autori:
#  xfilom00 -- Filo Martin
#  xkalab00 -- Kalab Jan
#  xkobli00 -- Koblizek Ondrej
#  xnovyo01 -- Novy Ondrej

PROG=ifj07
CC=gcc
CFLAGS=-O2 -Wall -std=c99 -pedantic -g -D NDEBUG
CCVERS=$(shell $(CC) -dumpversion)
ifeq ($(word 1, $(sort 4.1.3 $(CCVERS))),4.1.3)
    CFLAGS+=-fgnu89-inline
endif
LDLIBS=-lm

.PHONY: all

all: $(PROG)

$(PROG): $(patsubst %.c,%.o,$(wildcard *.c))
	$(CC) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f *.o $(PROG)
