.\" 	$Id: comments.mm,v 1.2 1992/05/28 09:34:52 eric Exp $
.\" 
.SA 1
.TL
Implementor's Comments on the IEEE PILOT Standard
.AF "Thyrsus Enterprises"
.AU "Eric S. Raymond" ESR "\&\&22 South Warren Ave, Malvern PA 19355"
.MT 4
.nr Pt 1
.H 1 "Introduction"
.P
A language standard ought to be a self-contained and unambiguous
description of a language, sufficiently complete to enable a competent
language implementor to write a translator that deals appropriately
with all code that uses only constructs defined in the standard.
.P
The best test of the completeness and precision of a language standard
is a pragmatic one.  Can an expert language implementor with no
experience of the language produce a quality implementation without
having to either make arbitrary choices or refer to nebulous `existing
practice'?  If not, then the standard is under-specified or ambiguous.
The combination of innocence and experience required of the
implementor to make such a test effective is hard to find, but it
happens with respect to PILOT that I fill it exactly.
.P
I first considered writing a PILOT interpreter less than a month ago,
while talking with Dick Karpinski at Hackers 7.0 in October 1991.
Though I am expert with YACC and LEX and the author of several
compilers and special-purpose control languages, I had barely even
heard of PILOT, and have never programmed on any existing implementation.
.P
I have now completed an implementation of a PILOT interpreter meeting
the letter of IEEE Standard 1154-1991 [1].  I have also written, to
test it, a large PILOT program which tutors users on the PILOT
language.  Though I undertook the job basically as an interesting
three or four-day hack, the process has raised some issues of which (I
feel) the PILOT standards committee and community ought to take formal
notice.
.P
In brief, the Standard is seriously under-specified, poorly organized,
ambiguous in some spots, buggy in others, and in serious need of
revision.  Specifics follow.
.H 1 "Lexical Specification Problems"
.P
There is a whole cluster of problems arising from poorly specified
lexical rules that could have been avoided with a very little extra
language.
.P
The <char> production is misleading.  It should read "...any ASCII
character other than a newline...".  As written, it looks like strings
never terminate.  The fact that statements are terminated by newline
is never made explicit; even if it were, the application to string
termination is hard to see.
.P
Under the <statement> production, it says "Whitespace between elements
is ignored", but in some cases near strings it is unclear what an
"element" is.  To take two important cases:
.VL 5
.LI *
In the <text> production, it is not obvious whether text
is interpreted as a token stream or as characters 
including whitespace.
.LI *
It is also not obvious whether whitespace after a ':' is
elided from a trailing text element.
.LE
.P
Practice in existing implementations is to treat <text> as a stream of
characters.  This creates a serious lexical problem with regard to the
boundaries of embedded identifiers.  For example, the text

.DS
	"sample $var sample"
.DE

might be interpreted as any of

.DS
	"sample " $v "ar sample"
	"sample " $va "r sample"
	"sample " $var " sample"
.DE

Common sense would favor the last (whitespace-bounded) parse, but the
Standard does not even express or even imply a rule on the matter.  
The problem is even nastier with postfix-$ string identifiers.  The text

.DS
	"sample var$ sample"
.DE

may be interpreted as any of

.DS
	"sample va" r$ " sample"
	"sample v" ar$ " sample"
	"sample " var$ " sample"
.DE

and again the Standard gives no guidance.  It would not be responsive
to argue that the whitespace-bounded (third) interpretation is
`obviously' correct; because there is another rule which compilcates
the picture.
.P
The <limited-string> rule limits identifiers to 10 characters.  How,
then, do we parse "sample $abcdefghijk sample"?  Is this

.DS
	"sample " $abcdefghij "k sample"
.DE

or does it raise a syntax error?  The Standard does not specify.
.P
In TYPE, MATCH, REMARK, FILE, GRAPHIC, YES and NO, the lexical rules do not
permit us to distinguish between a missing and a null text argument.
The BNF is thus unnecessarily complicated; it should specify <text>
rather than {<text>} in all cases, and note that <text> may be empty.
.H 1 BNF problems
.P
There are also miscellaneous technical problems arising from
misleading BNF usage and the sketchiness of the annotations.
.P
Syntactic elements in the BNF are not consistently surrounded by <>.
This irregularity makes it harder to read.
.P
Handling of case is inconsistent.  2.2 states that case is
insignificant, but the <condition> production confuses the issue by
giving keyword alternatives in both cases.
.P
The productions for the Y and N commands do more to conceal than
reveal what's really going on.  Better to have simply specified that
an elided keyword before Y or N is treated as `type'.
.P
The BNF production for the <condition> part is bizarre and misleading,
especially the part reading

.DS
	if condition has <rel_exp>
		then if %satisfied
			then %satisfied		<< %relation
.DE

I am informed that this is intended to suggest `short circuit'
evaluation of conditions like

.DS
	t y (#foo > 3) : stuff
.DE

so that if %answer contains "stuff" the condition (#foo > 3) need not
be evaluated.  However, since IEEE PILOT has no side effects in
expressions, this should not be a consideration.  In any case, the
annotation

.DS
	if condition has 'y' or 'Y'
		then %satisfied		<< %matched
	else if condition has 'n' or 'N'
		then %satisfied		<< not %matched
	else if condition has <rel_exp>
		then %satisfied		<< %relation
.DE

expresses the same semantics more clearly (note the presence of the
second else).
.H 1 "Semantic underspecification problems with Std 1154-1991"
.P
The semantics of the YES and NO statements are inadequately specified.
The cryptic references "same as ty" and "same as tn" seem intended to
convey that Y and N act like T statements with preset Y and N
conditions, but this needs to be made clearer --- and not just as a
last sentence in 4.6, which is about a different feature!
.P
In the ACCEPT production, it looks semantically as though the
<identifier> must be a string ID, because it says the identifier is
assigned %answer which is type string.  However, I am told it is
traditional to do an implicit conversion to a number when the
identifier is of numeric type, ignoring embedded commas.  The Standard
does not mention this.
.P
Also under ACCEPT, it is not clear whether or not the value of %text
afterwards includes the trailing newline.
.P
The Standard fails to specify whether an attempt to access a variable
not yet assigned is an error.  Assuming it is not, it fails to specify
the initial value of variables.
.P
The description of `match' fails to specify whether '*' prefers the
longest or the shartest possible match; also whether "*" matches the
null string.  That is, in matching an argument of "b*a" against an
%answer of "xbaaacx" it is not clear whether %matched should be "b",
"ba" or "baa".
.P
The description of `match' fails to specify whether whitespace is
literal or fluid in a match pattern (that is, whether by default all
runs of whitespace are treated as equivalent for matching purposes).
.P
The description of `match' fails to specify whether, in a pattern with
multiple or-barred alternatives, %right and %left are excavated from
the entire pattern or just the alternative in which the match was found.
.P
Some existing program text implies that whitespace occuring in match
patterns after any of the or-bar characters is ignored.  The Standard
does not address this.
.P
There is no unary minus; hence, no negative numeric constants.  But
just such a constant is referenced in the explanation of the <mulop>
production!
.P
The Standard is vague on the subject of what a `valid label is; in
particular it does not specify whether or not labels may be
forward-referenced.
.P
The Standard fails to mention a major feature of existing practice,
one used by every program in [2]; colon continuation.  Dick Karpinski
informs me that the new line continuation syntax proposed in 4.6 is
intended to replace this feature.  However, this would clearly be a
major mistake.  Even if it did not break nearly the entire corpus of
existing programs (including the language designer's own tutorial
examples!), the new syntax invites bizarre errors if any leading part
of a wrapped line resembles a keyword.
.P
Finally, and most seriously, there is the complete lack of
any semantic description of the FILE or GRAPHICS commands.  That two
of the keywords described as `core language elements' have completely
unspecified actions is nothing less than disgraceful.  If no semantics
could be agreed on, they should not have been included in the required
core set.
.H 1 "Other problems with the Standard"
.P
The Language Extensions section fights existing practice too hard.
The egregious problems with 4.6 have already been noted.  Why the WAIT
name is specified for timed delay when practice and [2] seem to favor
PAUSE is not clear.  Nor does the suggestion of `\' as a modulo
operator in 4.10 recommend itself over the % used in [2].  It's
already loaded as the string escape character.
.P
The abjuration that a keyword-expansion program `should be provided'
is duplicated on page 8 and 14.  This bespeaks sloppy editing.
Moreover it is not clear what it means nor why it's important.  The
passive voice nimbly evades the question of who is to do the providing.
.P
No personal names are given in the history section, which is odd
considering the amount of space devoted to it.  This gives the whole a
dry, beaureaucratic tone and suppresses at least one important fact.
Surely it is of some interest to readers that the inventor of the
language now chairs the group that produced the Standard?
.P
What is the historical remark in 4.5 doing in a section supposedly
about language extensions?
.P
The presence of two loose pages of corrections is not reassuring.
.H 1 "Recommendations for Change"
.P
The Standard's unhappy combination of serious underspecification and
numerous small technical problems adds up to an embarrassing whole,
creating the impression of a sloppy and amateurish job done by a group
of people too close to the language to notice that they've left out
key details.  I would not want my name associated with the document as
it now stands, and submit that it requires immediate and serious revision.
.P
The three most conspicuous problems with the Standard as it now exists
are (1) the ambiguities in the lexical rules, and (2) the gaping void
where semantic specification of FILE and GRAPHIC ought to be.  Also,
(3) the omission of colon continuation creates major difficulties for
almost all existing code, and the 4.6 proposal to replace it is an
unmitigated disaster which should be scrapped.
.P
The specific changes I most urgently recommend are, accordingly:
.P
.VL 5
.LI 1.
Addition of a `Lexical Rules' section to the Standard.  It need not be
terribly complicated; but it should say things like "whitespace is
ignored except in <text> arguments, e.g. following the colon in a
statement with text arguments or following the equals character in an
assignment to a string identifier" and "case is ignored everywhere in
the language, but preserved on output of text data".
.LI 2.
FILE and GRAPHICS should either be specified or dropped from the core
statements list.
.LI 3.
Colon continuation should be supported.  The proposed 4.6 replacement
should be dropped.
.LE
.P
There is much more that ought to be done, including the following:
.P
.VL 5
.LI 4.
The BNF needs to be rewritten more carefully. The command productions
ough to be interlineated with paragraph-length descriptions of what
the command is expected to do.
.LI 5.
The postfix-$ form for string identifiers allowed in 3.1 (correction
sheet, page 13) should be dropped.  It creates too many lexical
ambiguities.
.LI 6.
Is it really necessary to have \fIthree\fI different equivalent
alternation operators for string matching?  A reasonable design would
pick one and toss the other two.
.LE
.P
In testing the interpreter against the programs from [2], I learned
quite a bit about existing practice and idioms.  The language of the
Standard should not break as many of these as it does.
I believe serious consideration ought to be given to:
.VL 5
.LI 7.
Admitting CH, CA, and PAUSE to the Standard, with the proviso that
CH can be a no-op and CA raise an error when PILOT is run on a display
that does not have the relevant features.
.LI 8.
Permitting the leading * to be elided from JUMP and USE label arguments.
.LI 9.
Allowing a conforming implementation to ignore leading whitespace (after the
colon) in a text part.
.LE
.HU Bibliography
.VL 5
.LI [1]
.I IEEE Standard for Programmed Inquiry, Learning Or Teaching (PILOT)
ISBN 1-55937-151-X, dated August 22 1991.
.LI [2]
.I A User's Guide To Pilot
by John A. Starkweather, Ph.D. Prentice-Hall, Inc., Englewood Cliffs,
NJ 1985.
.LE
