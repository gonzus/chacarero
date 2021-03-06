first: demo

# use C11
CFLAGS += -std=c11

# get all warnings including some extra crispy ones
CFLAGS += -Wall -Wextra

# BUT... lemon has some warnings that we cannot work around
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare

# check memory allocations
CFLAGS += -DMEM_CHECK=1

# use realloc instead of malloc / free
# CFLAGS += -DMEM_USE_REALLOC_INSTEAD_OF_MALLOC_AND_FREE=1

# see more log messages
CFLAGS += -DLOG_LEVEL=0

# turn on debugger support
CFLAGS += -g

# optimize compiled objects
CFLAGS += -O3

LEMON_TEMPLATE = /usr/local/share/lemon/lempar.c

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

demo: demo.o lexicon.o grammar.o lexer.o parser.o mem.o log.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

lexicon.c: lexicon.re
	re2c -W -o $@ $<

grammar.c grammar.h: grammar.lemon $(LEMON_TEMPLATE)
	lemon -W -T$(LEMON_TEMPLATE) $<

demo.o: demo.c parser.h
lexicon.o: lexicon.c grammar.h lexer.h
grammar.o: grammar.c parser.h
lexer.o: lexer.c grammar.h lexer.h
parser.o: parser.c grammar.h lexer.h parser.h
mem.o: mem.c mem.h
log.o: log.c log.h

clean:
	rm -f demo *.o
	rm -f grammar.h grammar.c grammar.out
	rm -f lexicon.c
	rm -f trace.out

all: first

.PHONY: clean all first
