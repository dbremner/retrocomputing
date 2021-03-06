#pragma once
/*
 *	Reserved word list
 */
RESERVED_WORD reserved_words[] = {

		/* Statements */
	{"DECLARE",	DECLARE},
	{"DO",		DO},
	{"END",		END},
	{"IF",		IF},
	{"THEN",		THEN},
	{"ELSE",		ELSE},
	{"GOTO",		GOTO},
	{"GO",		GO},
	{"CALL",		CALL},
	{"RETURN",	RETURN},
	{"DISABLE",	DISABLE},
	{"ENABLE",	ENABLE},
	{"OUTPUT",	OUTPUT},
	{"OUTWORD",	OUTWORD},
	{"OUTHWORD",	OUTHWORD},

		/* Operators */
	{"AND",		AND},
	{"OR",		OR},
	{"XOR",		XOR},
	{"NOT",		NOT},
	{"MOD",		MOD},
	{"PLUS",		PLUS},
	{"MINUS",	MINUS},

		/* DO options */
	{"CASE",		CASE},
	{"WHILE",	WHILE},
	{"TO",		TO},
	{"BY",		BY},

		/* DECLARE types */
	{"BYTE",		BYTE},
	{"WORD",		WORD},
	{"DWORD",	DWORD},
	{"INTEGER",	INTEGER},
	{"REAL",		REAL},
	{"SELECTOR",	SELECTOR},
	{"ADDRESS",	ADDRESS},
	{"STRUCTURE",	STRUCTURE},
	{"LABEL",	LABEL},
	{"POINTER",	POINTER},
	{"BASED",	BASED},
	{"LITERALLY",	LITERALLY},

		/* DECLARE options */
	{"DATA",		DATA},
	{"EXTERNAL",	EXTERNAL},
	{"INITIAL",	INITIAL},
	{"PUBLIC",	PUBLIC},
	{"AT",		AT},

		/* Misc reserved words */
	{"PROCEDURE",	PROCEDURE},
	{"REENTRANT",	REENTRANT},
	{"INTERRUPT",	INTERRUPT},

		/* End of list */
	{"",		END_OF_FILE}
};


/*
 *	Operator list
 */
RESERVED_OPERATOR reserved_operators[] = {
	{"+",	"+",	PLUS},
	{"-",	"-",	MINUS},
	{"*",	"*",	TIMES},
	{"/",	"/",	DIVIDE},
	{"<>",	"!=",	NOT_EQUAL},
	{"<=",	"<=",	LESS_EQUAL},
	{">=",	">=",	GREATER_EQUAL},
	{"<",	"<",	LESS},
	{">",	">",	GREATER},
	{"=",	"=",	EQUAL},
	{":=",	"=",	EQUATE},
	{"@",	"&",	AT_OP},
	{"",	"",	END_OF_FILE}
};

/*
 *	Control directives list
 */
RESERVED_WORD control_directives[] = {
#ifdef	USE_ALL_CONTROLS
	{"CODE",		C_CODE},
	{"CO",		C_CODE},
	{"NOCODE",	C_NOCODE},
	{"NOCO",		C_NOCODE},
	{"COND",		C_COND},
	{"NOCOND",	C_NOCOND},
	{"DEBUG",	C_DEBUG},
	{"DB",		C_DEBUG},
	{"NODEBUG",	C_NODEBUG},
	{"NODB",		C_NODEBUG},
	{"EJECT",	C_EJECT},
	{"EJ",		C_EJECT},
#endif
	{"IF",		C_IF},
	{"ELSEIF",	C_ELSEIF},
	{"ELSE",		C_ELSE},
	{"ENDIF",	C_ENDIF},
	{"INCLUDE",	C_INCLUDE},
	{"IC",		C_INCLUDE},
#ifdef	USE_ALL_CONTROLS
	{"INTERFACE",	C_INTERFACE},
	{"ITF",		C_INTERFACE},
	{"LEFTMARGIN",	C_LEFTMARGIN},
	{"LM",		C_LEFTMARGIN},
	{"LIST",		C_LIST},
	{"LI",		C_LIST},
	{"NOLIST",	C_NOLIST},
	{"NOLI",		C_NOLIST},
	{"OBJECT",	C_OBJECT},
	{"OJ",		C_OBJECT},
	{"NOOBJECT",	C_NOOBJECT},
	{"NOOJ",		C_NOOBJECT},
	{"OPTIMIZE",	C_OPTIMIZE},
	{"OT",		C_OPTIMIZE},
	{"OVERFLOW",	C_OVERFLOW},
	{"OV",		C_OVERFLOW},
	{"NOOVERFLOW",	C_NOOVERFLOW},
	{"NOOV",		C_NOOVERFLOW},
	{"PAGELENGTH",	C_PAGELENGTH},
	{"PL",		C_PAGELENGTH},
	{"PAGEWIDTH",	C_PAGEWIDTH},
	{"PW",		C_PAGEWIDTH},
	{"PAGING",	C_PAGING},
	{"PI",		C_PAGING},
	{"NOPAGING",	C_NOPAGING},
	{"NOPI",		C_NOPAGING},
	{"PRINT",	C_PRINT},
	{"PR",		C_PRINT},
	{"NOPRINT",	C_NOPRINT},
	{"NOPR",		C_NOPRINT},
	{"RAM",		C_RAM},
	{"ROM",		C_ROM},
	{"SAVE",		C_SAVE},
	{"SA",		C_SAVE},
	{"RESTORE",	C_RESTORE},
	{"RS",		C_RESTORE},
#endif
	{"SET",		C_SET},
	{"RESET",	C_RESET},
#ifdef	USE_ALL_CONTROLS
	{"SMALL",	C_SMALL},
	{"SM",		C_SMALL},
	{"COMPACT",	C_COMPACT},
	{"CP",		C_COMPACT},
	{"MEDIUM",	C_MEDIUM},
	{"MD",		C_MEDIUM},
	{"LARGE",	C_LARGE},
	{"LA",		C_LARGE},
	{"SUBTITLE",	C_SUBTITLE},
	{"ST",		C_SUBTITLE},
	{"SYMBOLS",	C_SYMBOLS},
	{"SB",		C_SYMBOLS},
	{"NOSYMBOLS",	C_NOSYMBOLS},
	{"NOSB",		C_NOSYMBOLS},
	{"TITLE",	C_TITLE},
	{"TT",		C_TITLE},
	{"TYPE",		C_TYPE},
	{"TY",		C_TYPE},
	{"NOTYPE",	C_NOTYPE},
	{"NOTY",		C_NOTYPE},
	{"XREF",		C_XREF},
	{"XR",		C_XREF},
	{"NOXREF",	C_NOXREF},
	{"NOXR",		C_NOXREF},
	{"INTVECTOR",	C_INTVECTOR},
	{"IV",		C_INTVECTOR},
	{"NOINTVECTOR",	C_NOINTVECTOR},
	{"NOIV",		C_NOINTVECTOR},
	{"MOD86",	C_MOD86},
	{"MOD186",	C_MOD186},
	{"WORD16",	C_WORD16},
	{"W16",		C_WORD16},
	{"WORD32",	C_WORD32},
	{"W32",		C_WORD32},
#endif
		/* End of list */
	{"",		END_OF_FILE}
};

