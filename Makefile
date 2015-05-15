
YACC=bison
YFLAGS=
CC=gcc
CFLAGS=-D_GNU_SOURCE -g
LEX=flex
LFLAGS=

SRCS=classdef.c phoneme.c syllable.c util.c statement.c random.c evaluate_classdef.c werd.c
OBJS=$(SRCS:%.c=%.o)
LINTS=$(SRCS:%.c=%.ln)

all: werd

werd: depend werd_parser.tab.c werd_parser.yy.c werd_parser.tab.h werd_parser.yy.h $(OBJS)
	$(CC) $(CFLAGS) werd_parser.yy.c werd_parser.tab.c $(SRCS) -o $@

clean:
	rm -f *.tab.c *.tab.h *.yy.c *.exe .depend *.o *.stackdump *.yy.h *.ln

lint: $(LINTS)

test: all
	./werd < data/w-english

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend

%.tab.c %.tab.h: %.y
	$(YACC) $(YFLAGS) -d $<

%.yy.c %.yy.h: %.l
	$(LEX) $(LFLAGS) --header-file=$*.yy.h -o $@ $<

%.ln: %.c
	-splint -weak +posixlib +gnuextensions $< > $@

-include .depend
