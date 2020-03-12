first: demo

# all warnings except unused variables, lemon has some
CFLAGS += -Wall -Wno-unused-variable
CFLAGS += -DGMEM_CHECK

# turn on debugger support
CFLAGS += -g

# optimize compiled objects
CFLAGS += -O3

LEMON_TEMPLATE = /usr/local/share/lemon/lempar.c

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

demo: demo.o lexicon.o grammar.o lexer.o parser.o gmem.o
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
gmem.o: gmem.c gmem.h

clean:
	rm -f demo *.o
	rm -f grammar.h grammar.c grammar.out
	rm -f lexicon.c
	rm -f trace.out

all: first

.PHONY: clean all first
