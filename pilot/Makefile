#
# Makefile for IEEE PILOT interpreter/compiler
#
#	$Id: Makefile,v 1.11 1996/10/07 22:38:20 esr Exp $
#

# Configuration
#
# Set up your compiler with CC.  It must be ANSI-compliant.
#
# You will need stock Unix lex and yacc or Berkeley blex and byacc.
# Bison and Flex will *not* do, they are too incompatible.
#
# If you have termcap, you can try -DTERMCAP for terminal-independent I/O
# (otherwise the code will assume VT100 compatibility).  Note; this fails
# to work with some Berkeley systems; the symptom is a core-dump on startup.
#
# PILOTDIR is the directory PILOT will look for its compilation skeleton in.
#
CC       = gcc -ansi
SYSTYPE	 = -DTERMCAP
PILOTDIR = /home2/esr/WWW/pilot-1.8/

YACC = yacc
LEX = lex

# Optimization flags
OPTFLAGS = -g
LDFLAGS  = -s
YFLAGS   = -vt

CFLAGS  = $(SYSTYPE) $(OPTFLAGS) -DPILOTDIR=\"$(PILOTDIR)\"

SOURCES = pilot.[ylch] gencode.[ch] rpilot.c nonstd.c match.c plib.c numconv.c stdconv.l
DOCS    = READ.ME NEWS pilot.1 stdconv.1 comments.mm tour.mm
SAMPLES = story.p tea.p speaknum.p goldilocks.p tutor.p 
PILOT   = $(DOCS) Makefile $(SOURCES) $(SAMPLES) pilot.lsm

all: pilot stdconv

pilot: grammar.o lexer.o gencode.o nonstd.o libpilot.a
	$(CC) $(CFLAGS) grammar.o lexer.o gencode.o nonstd.o $(LDFLAGS) libpilot.a $(LEXLIB) -ltermcap -o pilot

libpilot.a: rpilot.o plib.o match.o numconv.o
	ar cr libpilot.a rpilot.o plib.o match.o numconv.o
	-ranlib libpilot.a

grammar.o: grammar.c pilot.h tokens.h gencode.h
lexer.o: lexer.c pilot.h tokens.h gencode.h
gencode.o: gencode.c pilot.h tokens.h gencode.h
nonstd.o: nonstd.c pilot.h tokens.h gencode.h
rpilot.o: rpilot.c pilot.h gencode.h
plib.o: plib.c pilot.h
match.o: match.c pilot.h
numconv.o: numconv.c

lexer.c: pilot.l
	$(LEX) $(LFLAGS) pilot.l
	mv lex.yy.c lexer.c

grammar.c tokens.h y.output: pilot.y
	$(YACC) $(YFLAGS) -d pilot.y
	mv y.tab.c grammar.c
	mv y.tab.h tokens.h

pilot.man: pilot.1
	nroff -man -Tlp pilot.1 >pilot.man

stdconv.man: stdconv.1
	nroff -man -Tlp stdconv.1 >stdconv.man

comments.lp: comments.mm
	nroff -mm -Tlp comments.mm >comments.lp

tour.lp: tour.mm
	nroff -mm -Tlp tour.mm >tour.lp

stdconv: stdconv.l
	$(LEX) $(LFLAGS) stdconv.l
	mv lex.yy.c stdconv.c
	$(CC) stdconv.c -o stdconv

GRAMMAR = grammar.c lexer.c tokens.h y.output
GENERATED = comments.lp tour.lp pilot.man stdconv.man stdconv.c

clean:
	rm -f libpilot.a *.o pilot lexer stdconv y.tab.c yacc.* y.output *~ \#* *.shar *.tar *.tar.gz TAGS

uclean: clean
	rm -f $(GENERATED) $(GRAMMAR)

# Zipped tar for FTP distribution
pilot.tar: $(PILOT)
	tar -cvf pilot.tar $(PILOT) $(GRAMMAR)
pilot.tar.gz: pilot.tar
	gzip pilot.tar

# The distribution for UNIX systems
pilot.shar:
	shar $(PILOT) >pilot.shar

# Cope with mailer limits on message size
mailshars:
	shar $(DOCS) tutor.p >pilot1.shar
	shar Makefile $(SOURCES) $(GRAMMAR) $(SAMPLES) >pilot2.shar

# The distribution for non-UNIX systems (no yacc(1), lex(1) or nroff(1)).
luser.shar:
	shar $(PILOT) $(GENERATED) $(GRAMMAR) >luser.shar

TAGS: $(SOURCES)
	etags $(SOURCES)
