
/*  A Bison parser, made from pilot.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	TYPE	258
#define	ACCEPT	259
#define	MATCH	260
#define	JUMP	261
#define	USE	262
#define	COMPUTE	263
#define	GRAPHIC	264
#define	KFILE	265
#define	REMARK	266
#define	END	267
#define	YES	268
#define	NO	269
#define	JUMPMATCH	270
#define	TYPEH	271
#define	PROBLEM	272
#define	PAUSE	273
#define	LINK	274
#define	CLEARHOME	275
#define	CURSADDR	276
#define	CLEARLINE	277
#define	CLEAREND	278
#define	SYSTEM	279
#define	NUMBER	280
#define	CHAR	281
#define	SPECIAL	282
#define	STRING	283
#define	ALLOC	284
#define	NUMERIC_IDENT	285
#define	STRING_IDENT	286
#define	LABEL	287
#define	NEWLINE	10
#define	COLON	288
#define	LPAREN	289
#define	RPAREN	290
#define	PLUS	291
#define	MINUS	292
#define	MULTIPLY	293
#define	DIVIDE	294
#define	MODULO	295
#define	EQUAL	296
#define	LESS	297
#define	GREATER	298
#define	BANG	299
#define	ORBAR	300
#define	COMMA	301
#define	NEQUAL	302
#define	NOTGRT	303
#define	NOTLESS	304
#define	UMINUS	305
#define	ANDAND	306
#define	LEXERR	307

#line 25 "pilot.y"

/*
 *	$Id: pilot.y,v 1.8 1996/10/07 22:38:25 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: pilot.y,v 1.8 1996/10/07 22:38:25 esr Exp $";
#endif /* lint */

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "pilot.h"

extern int yylineno, cmd;
extern FILE *yyin;

#define NOMEM	"out of memory\n"
#define BADATOM	"internal error, bad type in atom\n"

#define NONSTD	"this operation is not supported in IEEE 1154-1991\n"
#define NONSTANDARD	if (pedantic) yyerror(NONSTD); else

#line 83 "pilot.y"
typedef union
{
    int		number;
    char	*string;
    struct rav	*var;
    struct edon	*node;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		150
#define	YYFLAG		-32768
#define	YYNTBASE	68

#define YYTRANSLATE(x) ((unsigned)(x) <= 307 ? yytranslate[x] : 79)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    33,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    57,     2,     2,     2,    49,     2,     2,    39,
    37,    45,    41,    61,    43,     2,    47,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    35,     2,    53,
    51,    55,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    59,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    34,    36,    38,
    40,    42,    44,    46,    48,    50,    52,    54,    56,    58,
    60,    62,    63,    64,    65,    66,    67
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     5,     9,    12,    14,    16,    19,    24,    29,
    34,    38,    43,    48,    53,    58,    63,    68,    75,    82,
    87,    92,    97,   102,   106,   111,   116,   121,   126,   131,
   135,   142,   146,   150,   155,   157,   161,   162,   164,   169,
   173,   175,   177,   179,   183,   187,   191,   196,   201,   206,
   208,   212,   216,   218,   221,   225,   229,   233,   237,   239,
   241,   242,   245,   248,   251,   254,   257
};

static const short yyrhs[] = {    -1,
    68,    69,    33,     0,    68,     1,    33,     0,    68,    33,
     0,    32,     0,    70,     0,    32,    70,     0,     3,    72,
    35,    78,     0,     4,    72,    35,    31,     0,     4,    72,
    35,    30,     0,     4,    72,    35,     0,     5,    72,    35,
    78,     0,     6,    72,    35,    32,     0,     6,    72,    35,
    30,     0,     6,    72,    35,    27,     0,     7,    72,    35,
    32,     0,     7,    72,    35,    30,     0,     8,    72,    35,
    31,    51,    78,     0,     8,    72,    35,    30,    51,    75,
     0,     9,    72,    35,    78,     0,    10,    72,    35,    78,
     0,    11,    72,    35,    78,     0,    12,    72,    35,    75,
     0,    12,    72,    35,     0,    15,    72,    35,    71,     0,
    16,    72,    35,    78,     0,    17,    72,    35,    78,     0,
    18,    72,    35,    75,     0,    19,    72,    35,    78,     0,
    20,    72,    35,     0,    21,    72,    35,    25,    61,    25,
     0,    22,    72,    35,     0,    23,    72,    35,     0,    24,
    72,    35,    78,     0,    32,     0,    71,    61,    32,     0,
     0,    73,     0,    73,    39,    74,    37,     0,    39,    74,
    37,     0,    13,     0,    14,     0,    30,     0,    75,    53,
    75,     0,    75,    55,    75,     0,    75,    51,    75,     0,
    75,    53,    55,    75,     0,    75,    53,    51,    75,     0,
    75,    55,    51,    75,     0,    76,     0,    76,    41,    76,
     0,    76,    43,    76,     0,    77,     0,    43,    77,     0,
    77,    45,    77,     0,    77,    47,    77,     0,    77,    49,
    77,     0,    39,    75,    37,     0,    30,     0,    25,     0,
     0,    78,    28,     0,    78,    29,     0,    78,    25,     0,
    78,    26,     0,    78,    31,     0,    78,    30,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    93,    94,    96,    98,   104,   105,   106,   120,   123,   125,
   127,   130,   133,   135,   137,   139,   141,   144,   146,   149,
   151,   154,   157,   159,   162,   164,   166,   168,   170,   172,
   174,   176,   178,   180,   185,   187,   192,   193,   194,   195,
   198,   199,   202,   204,   205,   206,   207,   208,   209,   212,
   213,   214,   217,   218,   220,   221,   222,   226,   227,   228,
   231,   233,   235,   237,   239,   241,   243
};

static const char * const yytname[] = {   "$","error","$undefined.","TYPE","ACCEPT",
"MATCH","JUMP","USE","COMPUTE","GRAPHIC","KFILE","REMARK","END","YES","NO","JUMPMATCH",
"TYPEH","PROBLEM","PAUSE","LINK","CLEARHOME","CURSADDR","CLEARLINE","CLEAREND",
"SYSTEM","NUMBER","CHAR","SPECIAL","STRING","ALLOC","NUMERIC_IDENT","STRING_IDENT",
"LABEL","NEWLINE","COLON","':'","LPAREN","')'","RPAREN","'('","PLUS","'+'","MINUS",
"'-'","MULTIPLY","'*'","DIVIDE","'/'","MODULO","'%'","EQUAL","'='","LESS","'<'",
"GREATER","'>'","BANG","'!'","ORBAR","'|'","COMMA","','","NEQUAL","NOTGRT","NOTLESS",
"UMINUS","ANDAND","LEXERR","prog","statement","exec","lablist","condition","yescond",
"rel_exp","expression","term","factor","text",""
};
#endif

static const short yyr1[] = {     0,
    68,    68,    68,    68,    69,    69,    69,    70,    70,    70,
    70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
    70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
    70,    70,    70,    70,    71,    71,    72,    72,    72,    72,
    73,    73,    74,    74,    74,    74,    74,    74,    74,    75,
    75,    75,    76,    76,    76,    76,    76,    77,    77,    77,
    78,    78,    78,    78,    78,    78,    78
};

static const short yyr2[] = {     0,
     0,     3,     3,     2,     1,     1,     2,     4,     4,     4,
     3,     4,     4,     4,     4,     4,     4,     6,     6,     4,
     4,     4,     4,     3,     4,     4,     4,     4,     4,     3,
     6,     3,     3,     4,     1,     3,     0,     1,     4,     3,
     1,     1,     1,     3,     3,     3,     4,     4,     4,     1,
     3,     3,     1,     2,     3,     3,     3,     3,     1,     1,
     0,     2,     2,     2,     2,     2,     2
};

static const short yydefact[] = {     1,
     0,     0,    37,    37,    37,    37,    37,    37,    37,    37,
    37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
    37,    37,     5,     4,     0,     6,     3,    41,    42,     0,
     0,    38,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     7,     2,    60,    59,     0,     0,     0,     0,    50,
    53,    61,     0,    11,    61,     0,     0,     0,    61,    61,
    61,    24,     0,    61,    61,     0,    61,    30,     0,    32,
    33,    61,    59,     0,    54,    40,     0,     0,     0,     0,
     0,     0,     0,     0,     8,     0,    10,     9,    12,    15,
    14,    13,    17,    16,     0,     0,    20,    21,    22,    23,
    35,    25,    26,    27,    28,    29,     0,    34,    58,    46,
     0,     0,    44,     0,    45,    51,    52,    55,    56,    57,
    64,    65,    62,    63,    67,    66,    39,     0,    61,     0,
     0,    48,    47,    49,    19,    18,    36,    31,     0,     0
};

static const short yydefgoto[] = {     1,
    25,    26,   112,    31,    32,    58,    59,    60,    61,    95
};

static const short yypact[] = {-32768,
    40,   -29,    14,    14,    14,    14,    14,    14,    14,    14,
    14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
    14,    14,    97,-32768,   -21,-32768,-32768,-32768,-32768,    46,
   -20,    -9,     0,     7,    19,    32,    42,    56,    63,   108,
   115,   117,   119,   120,   121,   122,   123,   124,   125,   126,
   127,-32768,-32768,-32768,    33,    54,   -16,   114,    35,    -7,
    47,-32768,    46,    -5,-32768,    -8,    48,    80,-32768,-32768,
-32768,    54,    49,-32768,-32768,    54,-32768,-32768,   128,-32768,
-32768,-32768,-32768,   129,-32768,-32768,    54,    44,   -22,    54,
    54,   -16,   -16,   -16,   116,   130,-32768,-32768,   116,-32768,
-32768,-32768,-32768,-32768,   112,   113,   116,   116,   116,-32768,
-32768,   104,   116,   116,-32768,   116,   107,   116,-32768,-32768,
    54,    54,-32768,    54,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    54,-32768,   137,
   145,-32768,-32768,-32768,-32768,   116,-32768,-32768,   171,-32768
};

static const short yypgoto[] = {-32768,
-32768,   149,-32768,   118,-32768,   110,   -56,    58,   -55,   -64
};


#define	YYLAST		173


static const short yytable[] = {    84,
    99,    85,    54,    27,   107,   108,   109,    83,    54,   113,
   114,    53,   116,    83,    62,   110,    56,   118,   100,   115,
    57,   101,    56,   102,    97,    98,    28,    29,   124,    63,
   120,   123,   125,    90,    64,    91,   128,   129,   130,   149,
     2,    65,     3,     4,     5,     6,     7,     8,     9,    10,
    11,    12,    30,    66,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,   142,   143,    67,   144,    54,   -43,
    54,    23,    24,    83,   146,    55,    68,   103,    54,   104,
   111,   145,    56,    83,    56,    87,    57,    88,    57,    89,
    69,    92,    56,    93,   121,    94,    57,    70,   122,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,   105,
   106,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
   131,   132,    71,   133,   134,   135,   136,   126,   127,    72,
    86,    73,   117,    74,    75,    76,    77,    78,    79,    80,
    81,    82,   138,   139,   140,   119,   137,   141,   147,   148,
   150,    52,    96
};

static const short yycheck[] = {    56,
    65,    57,    25,    33,    69,    70,    71,    30,    25,    74,
    75,    33,    77,    30,    35,    72,    39,    82,    27,    76,
    43,    30,    39,    32,    30,    31,    13,    14,    51,    39,
    87,    88,    89,    41,    35,    43,    92,    93,    94,     0,
     1,    35,     3,     4,     5,     6,     7,     8,     9,    10,
    11,    12,    39,    35,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,   121,   122,    35,   124,    25,    37,
    25,    32,    33,    30,   139,    30,    35,    30,    25,    32,
    32,   138,    39,    30,    39,    51,    43,    53,    43,    55,
    35,    45,    39,    47,    51,    49,    43,    35,    55,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    30,
    31,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    25,    26,    35,    28,    29,    30,    31,    90,    91,    35,
    37,    35,    25,    35,    35,    35,    35,    35,    35,    35,
    35,    35,    51,    51,    61,    37,    37,    61,    32,    25,
     0,    23,    63
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(YYPARSE_PARAM)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 95 "pilot.y"
{eolhook(); new = nodes;;
    break;}
case 3:
#line 97 "pilot.y"
{yyerrok;;
    break;}
case 5:
#line 104 "pilot.y"
{gen_label(yyvsp[0].var);;
    break;}
case 7:
#line 106 "pilot.y"
{gen_label(yyvsp[-1].var);;
    break;}
case 8:
#line 121 "pilot.y"
{gen_type(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 9:
#line 124 "pilot.y"
{gen_accept(yyvsp[-2].node, yyvsp[0].var, STRING_IDENT);;
    break;}
case 10:
#line 126 "pilot.y"
{gen_accept(yyvsp[-2].node, yyvsp[0].var, NUMERIC_IDENT);;
    break;}
case 11:
#line 128 "pilot.y"
{gen_accept(yyvsp[-1].node, (variable *)NULL, 0);;
    break;}
case 12:
#line 131 "pilot.y"
{gen_match(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 13:
#line 134 "pilot.y"
{gen_jump(yyvsp[-2].node, yyvsp[0].var, LABEL);;
    break;}
case 14:
#line 136 "pilot.y"
{NONSTANDARD {yyvsp[0].var->type = LABEL; gen_jump(yyvsp[-2].node, yyvsp[0].var, LABEL);};
    break;}
case 15:
#line 138 "pilot.y"
{gen_jump(yyvsp[-2].node, (variable *)NULL, yyvsp[0].number);;
    break;}
case 16:
#line 140 "pilot.y"
{gen_use(yyvsp[-2].node, yyvsp[0].var);;
    break;}
case 17:
#line 142 "pilot.y"
{NONSTANDARD {yyvsp[0].var->type = LABEL; gen_use(yyvsp[-2].node, yyvsp[0].var);};
    break;}
case 18:
#line 145 "pilot.y"
{gen_compute(yyvsp[-4].node, yyvsp[-2].var, (node *)NULL, textparts, tp - textparts);;
    break;}
case 19:
#line 147 "pilot.y"
{gen_compute(yyvsp[-4].node, yyvsp[-2].var, yyvsp[0].node, (part *)NULL, 0);;
    break;}
case 20:
#line 150 "pilot.y"
{gen_graphic(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 21:
#line 152 "pilot.y"
{gen_file(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 23:
#line 158 "pilot.y"
{NONSTANDARD gen_end(yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 24:
#line 160 "pilot.y"
{gen_end(yyvsp[-1].node, 0);;
    break;}
case 25:
#line 163 "pilot.y"
{NONSTANDARD gen_jumpmatch(yyvsp[-2].node, njumplabs, jumplabs);;
    break;}
case 26:
#line 165 "pilot.y"
{NONSTANDARD gen_typeh(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 27:
#line 167 "pilot.y"
{NONSTANDARD gen_problem(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 28:
#line 169 "pilot.y"
{NONSTANDARD gen_pause(yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 29:
#line 171 "pilot.y"
{NONSTANDARD gen_link(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 30:
#line 173 "pilot.y"
{NONSTANDARD gen_clearhome(yyvsp[-1].node);;
    break;}
case 31:
#line 175 "pilot.y"
{NONSTANDARD gen_cursaddr(yyvsp[-4].node, yyvsp[-2].number, yyvsp[0].number);;
    break;}
case 32:
#line 177 "pilot.y"
{NONSTANDARD gen_clearline(yyvsp[-1].node);;
    break;}
case 33:
#line 179 "pilot.y"
{NONSTANDARD gen_clearend(yyvsp[-1].node);;
    break;}
case 34:
#line 181 "pilot.y"
{NONSTANDARD gen_system(yyvsp[-2].node, textparts, tp - textparts);;
    break;}
case 35:
#line 186 "pilot.y"
{njumplabs = 0; jumplabs[njumplabs++] = yyvsp[0].var;;
    break;}
case 36:
#line 188 "pilot.y"
{jumplabs[njumplabs++] = yyvsp[0].var;;
    break;}
case 37:
#line 192 "pilot.y"
{yyval.node = (node *)NULL;;
    break;}
case 38:
#line 193 "pilot.y"
{yyval.node = yyvsp[0].node;;
    break;}
case 39:
#line 194 "pilot.y"
{yyval.node = cons(ANDAND, yyvsp[-3].node, yyvsp[-1].node);;
    break;}
case 40:
#line 195 "pilot.y"
{yyval.node = yyvsp[-1].node;;
    break;}
case 41:
#line 198 "pilot.y"
{yyval.node = atom(YES, 0, (variable *)NULL);;
    break;}
case 42:
#line 199 "pilot.y"
{yyval.node = atom(NO, 0, (variable *)NULL);;
    break;}
case 43:
#line 203 "pilot.y"
{yyval.node = atom(NUMERIC_IDENT, 0, yyvsp[0].var);;
    break;}
case 44:
#line 204 "pilot.y"
{yyval.node = cons(LESS, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 45:
#line 205 "pilot.y"
{yyval.node = cons(GREATER, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 46:
#line 206 "pilot.y"
{yyval.node = cons(EQUAL, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 47:
#line 207 "pilot.y"
{yyval.node = cons(NEQUAL, yyvsp[-3].node, yyvsp[0].node);;
    break;}
case 48:
#line 208 "pilot.y"
{yyval.node = cons(NOTGRT, yyvsp[-3].node, yyvsp[0].node);;
    break;}
case 49:
#line 209 "pilot.y"
{yyval.node = cons(NOTLESS, yyvsp[-3].node, yyvsp[0].node);;
    break;}
case 50:
#line 212 "pilot.y"
{yyval.node = yyvsp[0].node;;
    break;}
case 51:
#line 213 "pilot.y"
{yyval.node = cons(PLUS, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 52:
#line 214 "pilot.y"
{yyval.node = cons(MINUS, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 53:
#line 217 "pilot.y"
{yyval.node = yyvsp[0].node;;
    break;}
case 54:
#line 219 "pilot.y"
{NONSTANDARD cons(UMINUS, 0, yyvsp[0].node);;
    break;}
case 55:
#line 220 "pilot.y"
{yyval.node = cons(MULTIPLY, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 56:
#line 221 "pilot.y"
{yyval.node = cons(DIVIDE, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 57:
#line 223 "pilot.y"
{NONSTANDARD yyval.node = cons(MODULO, yyvsp[-2].node, yyvsp[0].node);;
    break;}
case 58:
#line 226 "pilot.y"
{yyval.node = yyvsp[-1].node;;
    break;}
case 59:
#line 227 "pilot.y"
{yyval.node = atom(NUMERIC_IDENT, 0, yyvsp[0].var);;
    break;}
case 60:
#line 228 "pilot.y"
{yyval.node = atom(NUMBER, yyvsp[0].number, (variable *)NULL);;
    break;}
case 61:
#line 232 "pilot.y"
{tp = textparts;;
    break;}
case 62:
#line 234 "pilot.y"
{tp->type = STRING; tp->part.string = yyvsp[0].string; tp++;;
    break;}
case 63:
#line 236 "pilot.y"
{tp->type = ALLOC; tp->part.string = yyvsp[0].string; tp++;;
    break;}
case 64:
#line 238 "pilot.y"
{tp->type = NUMBER; tp->part.number = yyvsp[0].number; tp++;;
    break;}
case 65:
#line 240 "pilot.y"
{tp->type = CHAR; tp->part.number = yyvsp[0].number; tp++;;
    break;}
case 66:
#line 242 "pilot.y"
{tp->type = STRING_IDENT; tp->part.var = yyvsp[0].var; tp++;;
    break;}
case 67:
#line 244 "pilot.y"
{tp->type = NUMERIC_IDENT; tp->part.var = yyvsp[0].var; tp++;;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 247 "pilot.y"

#include "gencode.h"
/*
 * Hold the strings and substitutable IDs in a text part.  The reasoning
 * behind the dimension is that there can't hardly be more of these than
 * there are characters on a line.
 */
static part textparts[MAXSTR], *tp;

/*
 * Cons nodes for expression structures.  Similar logic here.
 */
static node nodes[MAXSTR], *new = nodes;

/*
 * Label storage for JUMPMATCH handling
 */
static int njumplabs;
static variable *jumplabs[MAXJUMPS];

node *atom(type, value, var)
/* make an expression node from a part value */
int	type;	/* type of new node */
int	value;	/* part to be encapulated */
variable	*var;
{
    /* get a node */
    if (new < nodes + MAXSTR)
	new++;
    else
	yyerror(NOMEM);

    switch(new->type = type)
    {
    case NUMERIC_IDENT:
	new->value.var = var;
	break;

    case NUMBER:
	new->value.number = value;
	break;

    case YES:	/* eval() or the compiler will munch these */
    case NO:
	break;

    default:
	yyerror(BADATOM);
	break;
    }

    return(new);
}

node *cons(op, left, right)
/* make a cons for a binary operation */
int	op;		/* opcode */
node	*left, *right;	/* child nodes */
{
    /* get a node */
    if (new < nodes + MAXSTR)
	new++;
    else
	yyerror(NOMEM);

    new->type = op;
    new->left = left;
    new->right = right;

    return(new);
}

/* pilot.y ends here */
