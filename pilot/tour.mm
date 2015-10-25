.\" 	$Id: tour.mm,v 1.3 1996/10/07 22:38:26 esr Exp $
.\" 
.SA 1
.TL "A Brief Tour of the IEEE-PILOT Internals"
.AF "Thyrsus Enterprises"
.AU "Eric S. Raymond" ESR "\&\&22 South Warren Ave, Malvern PA 19355"
.MT 4
.nr Pt 1
.H 1 "Introduction"
.P
The reference implementation of IEEE PILOT is both an interpreter for
the language and a portable compiler for it, using techniques that may
be of interest to implementors of other small languages.  Other unusual
techniques were required to cope with PILOT's idiosyncratic lexical
rules and field structure.  This paper, accordingly, describes the
internals of IEEE PILOT.  It is intended both as a hacker's guide to the
PILOT code and as an exposition of methods which may find more general
applicability.  It concludes with a discussion of porting issues.
.P
.H 1 "Outline of the Implementation"
.P
PILOT parsing for both the interpreter and the compiler is driven by a
conventional yacc(1)-generated grammar and a lex(1)-generated lexical
analyzer.  The grammar is relatively simple, the lexer fairly complex
(the reverse of the usual situation in more modern languages).  Source
code may be found in pilot.y and pilot.l, respectively.  Because there
are incompatible YACCs floating around, the distribution includes the
generated C code from these files.
.P
The lexer and grammar build parse trees which are passed to one of two
back ends.  One back end is an interpreter which evals the
tree nodes; another back end is a code generator which emits
C code and compiles it.
.P
An important point to note is that the parse-tree traverse logic is
shared between the compiler and interpreter; the interpretation vs.
code-emission differences are actually embedded in the functions which
cope with the leaf nodes (and in the pre- and post-amble to the parse).
.P
.H 1 "Unusual aspects of the lexing and parsing process"
.P
The archaic field-oriented, partially case-blind syntax of PILOT
requires the lexer to maintain a couple of different kinds of state.
The major distinction that needs to be tracked is between text before
a colon (the keyword/expression/conditional part of PILOT statements)
and the part after (which may be a token-structured expression or a
string part, possibly with embedded variable references).
.P
We redefine lex(1)'s input() and output() macros to smash case where
appropriate (that is, everywhere but string parts).  This also allows
us to trigger some instrumentation and call a start-of-line hook just
after each input line read.
.P
The grammar, by contrast, is almost trivial.  Expression syntax is
simple, and there are only two other variable-length constructs; the
text part of statements like T: or M: (which is interpreted as a list
of literal strings interspersed with variable references; see the
`text' production in pilot.y), and the label list in the
(non-IEEE-standard) JM: construct.
.P
PILOT code is interpreted/compiled on a line-by-line basis.  The
grammar's `prog' production copes with the line structure, discarding
errors with a skip to the next line start.
.P
.H 1 "Interpretation versus compilation"
.P
The interpretation back end uses the parse tree as control structure
and calls functions in the libpilot.a library (which is linked to the
interpreter) to do the actual work.
.P
The only really tricky part of interpretation is the handling of
labels.  Label values are actually seek addresses in the source text;
when a forward reference is encountered, the interpreter needs to seek
forward to resolve it (this is why we can't execute PILOT programs off
standard input).
.P
The compilation back end also uses the parse tree as control structure
generates C code equivalent to the PILOT program into a temp file;
this code contains calls to the pilot library functions, and the
compile command used to generate the output binary links the pilot
library to it.
.P
In the compiler, label addresses are accumulated with a first pass
over the source text.  Each PILOT label is turned into a C label;
JUMPs become goto statements.  USE/END pairs are a little trickier;
the USE is a jump plus return-stack push of a unique USE number.  Each
end becomes a C switch in which generated USE labels are dispatched
to based on the USE number currently at the top of the return stack.
.P
Thus, what you're compiling away is essentially control structure and
the expression syntax; but since that's most of the language,
compilation is pretty effective.  Indeed, the only really nontrivial
PILOT library primitive is the MATCH operation.
.P
.H 1 "Portability considerations"
.P
This PILOT is written in ANSI C, YACC, and LEX.  It will make and
run correctly on (at least) any System V UNIX release 3.2 or later, or
SunOS.
 .PP
You will need stock Unix lex and yacc or Berkeley blex and byacc if
you want to hack the grammar.  Bison and Flex will \fInot\fR do, they are
too incompatible and lack needed features.
.P
The most serious problem you're likely to run into is different names
of header files on a target system (in particular with string.h).
Also, the code requires getopt(3); PD versions of it are ubiquitous.
If your C environment is \fIreally\fR archaic and crufty there may not be
a /usr/include/limits.h (the code only uses PATH_MAX) and you might
have to map strchr() to index() with a macro.
.P
The second most serious problem is the call to the C compiler to bash
generated C code into executable bits (this is in gencode.c:execfile().
This should work on any UNIX system, provided you have your pilot
directory variable PILOTDIR set up properly and put a copy of pilot.h
there.  If you're porting this to MS-DOS or whatever, string together
your own compilation command.
.P
If you define the symbol TERMCAP, the PILOT interpreter will use termcap(3)
calls to implement CH/CA/CL/CE.  Otherwise it will assume you're using a
VT100/ANSI compatible output device.  Same goes for compiled code generated
with the PILOT libraries.
