#include "as.h"
#include "lexyacc.h"
#include "asytab.h"
#ifdef _STANDALONE_
#include "preproc.h"
#endif

#define BSIZE           8192
#define YYCTYPE         char
#define YYCURSOR        cursor
#define YYLIMIT         limit
#define YYMARKER        marker
#define YYFILL( n )     {fill();}

extern YYSTYPE asyylval;
#ifndef _STANDALONE_
extern char *AsmInStr;          // The input string.
#endif

#ifdef _STANDALONE_
int CurrLineno = 1;             // This pair is used by the parser and the
char *CurrFilename = NULL;      // outside world.

int asyylineno = 1;             // The lexer uses this pair for their own error
char *asyyfname = NULL;         // reporting. (globl also because parser needs
/* NOTE: -- make sure asyylineno is used correctly: update it when we get '\n' and
            when we get #line directive.
         -- CurrLineno should be set in the parser
*/
#endif

#ifdef _STANDALONE_
static int  newlineno;
static char *bot, *pos, *top;
#endif
static char *cursor, *limit, *marker, *tok, *eofPtr, *_yytext;
static int  maxyytextlen = 1, yytextlen;
static char *dirOpStr = NULL;   // used to keep directive string data
static char *cStr = NULL;       // used to keep scanned C-style string data

#ifdef _STANDALONE_
static size_t ppRead( char *buffer, size_t numchar ) {
//****************************************************

    size_t  count = 0;
    int     c = 0;

    while( c != EOF && count < numchar ) {
        buffer[count++] = c = PP_Char();
    }
    if( c == EOF ) count--;
  #ifdef AS_DEBUG_DUMP
    if( _IsOption( DUMP_LEXER_BUFFER ) ) {

        int count2;

        printf( "Read in lexer buffer (%d characters):\n", count );
        for( count2 = 0; count2 < count; ++count2 ) putchar( buffer[count2] );
        printf( "**END**\n" );
    }
  #endif
    return( count );
}

static void ppFlush( void ) {
//***************************

    while( PP_Char() != EOF );
}
#endif

static char *yytext( void ) {
//***************************
// Call this the first time you want the scanned string.
// Afterwards, you can use _yytext to get the same string.

    if ( (yytextlen = cursor - tok) > maxyytextlen - 1 ) {
        MemFree( _yytext );
        maxyytextlen = yytextlen + 1;
        _yytext = (char *) MemAlloc( maxyytextlen );
    }
    memcpy( _yytext, tok, yytextlen );
    _yytext[yytextlen] = 0;
    return( _yytext );
}

#ifdef _STANDALONE_
static void yylexError( int res_id ) {
//************************************
// If error reporting (yyerror style) is needed within this lexer module,
// this is the right one to call. Other ones such as yyerror might get
// the line number wrong.

    int     saveLineno;
    char    *saveFname, *tmpstr;

    saveLineno = CurrLineno;
    saveFname = CurrFilename;
    CurrLineno = asyylineno;
    CurrFilename = asyyfname;
    AsMsgGet( res_id, AsResBuffer );
    tmpstr = AsStrdup( AsResBuffer );
    asyyerror( tmpstr );
    MemFree( tmpstr );
    CurrLineno = saveLineno;
    CurrFilename = saveFname;
}
#endif

#ifdef _STANDALONE_
static void ppError( char *str ) {
//********************************
// Similar to the above one except that it's just for #error directive.

    int     saveLineno;
    char    *saveFname;

    saveLineno = CurrLineno;
    saveFname = CurrFilename;
    CurrLineno = asyylineno;
    CurrFilename = asyyfname;
    Error( GET_STRING, str );
    CurrLineno = saveLineno;
    CurrFilename = saveFname;
}
#endif

static char *stripBackQuotes( char *str ) {
//*****************************************
//  "`foo`" => "foo"

    str[strlen( str ) - 1] = '\0';
    return( str + 1 );
}

extern void AsLexerFini( void ) {
//*******************************
// Cleanup and reset states for next file (if any)

#ifdef _STANDALONE_
    MemFree( bot );
    CurrLineno = 1;
    asyylineno = 1;
    ppFlush();
    bot = NULL;
    pos = NULL;
    top = NULL;
    MemFree( CurrFilename );
    CurrFilename = NULL;
    MemFree( asyyfname );
    asyyfname = NULL;
#endif
    cursor = NULL;
    limit = NULL;
    marker = NULL;
    tok = NULL;
    eofPtr = NULL;
    maxyytextlen = 1;
    MemFree( _yytext );
    _yytext = NULL;
    MemFree( dirOpStr );
    dirOpStr = NULL;
    MemFree( cStr );
    cStr = NULL;
}

#ifdef _STANDALONE_
static void dropDblBackSlashes( char *fname ) {
//*********************************************

    size_t  len;

    len = strlen( fname );
    while( *fname ) {
        if( fname[0] == '\\' && fname[1] == '\\' )  {
            memmove( fname, fname + 1, len );
        }
        len--;
        fname++;
    }
}
#endif

static void fill( void ) {
//************************
#ifdef _STANDALONE_

    unsigned int cnt;

    if(!eofPtr) {
        cnt = tok - bot;
        if(cnt) { /* move partial token to the bottom */
            memcpy(bot, tok, limit - tok);
            tok = bot;
            marker -= cnt;
            cursor -= cnt;
            pos -= cnt;
            limit -= cnt;
        }
        if((top - limit) < BSIZE) { // buffer needs to be expanded
            char *buf;

            buf = (char *) MemAlloc( limit - bot + BSIZE );   // alloc new piece
            memcpy(buf, tok, limit - tok);          // copy leftover
            tok = buf;                              // adjust all pointers
            marker = &buf[marker - bot];
            cursor = &buf[cursor - bot];
            pos = &buf[pos - bot];
            limit = &buf[limit - bot]; // limit now points to first unused spot
            top = &limit[BSIZE];
            MemFree( bot );                         // free old chunk
            bot = buf;
        }
        if((cnt = ppRead( limit, BSIZE )) != BSIZE) { // EOF
            eofPtr = &limit[cnt];
            *eofPtr++ = '\000'; // place a special EOF marker here
        }
        limit += cnt;   // update limit after read
    }
#else

    int len;

    if( !eofPtr ) {
        len = strlen( AsmInStr );
        marker = AsmInStr;
        cursor = AsmInStr;
        tok = AsmInStr;
        limit = &AsmInStr[len];
        eofPtr = limit + 1;
    }
#endif
}

#ifdef __WATCOMC__
//#pragma warning 118 5         // This doesn't work!
#pragma disable_message( 118 );
#pragma off( unreferenced );    // To get rid of "yyaccept is unreferenced"
#endif

/*!re2c
ws      = [ \t]+;
nl      = "\n";
any     = [\000-\377];
dot     = any \ [\n];
ch      = dot \ [\000];
osym    = "."?[a-zA-Z_$@?.][a-zA-Z0-9_$@?.]*;
sym     = osym | "`"dot+"`";
:segment AS_ALPHA
symqual = "."?[a-zA-Z_$][a-zA-Z0-9_.$]*"/"[sSvVuUiIcCmMdD]+; /* ins with qualifier */
:endsegment
esc     = dot \ [\\];
fname   = dot*;
string  = "\"" ((esc \ ["] ) | "\\" dot)* "\"";
ppchar  = "#";          /* preprocessor character */
empstr  = "";           /* empty string */
*/

extern int asyylex( void ) {
//**************************

        if( DirGetNextScanState() == TRUE ) goto getdirop;
std:    tok = cursor;
/*!re2c
ws                      { goto std; }

:segment _STANDALONE_
ppchar"error "dot*      { ppError( yytext()+7 ); goto std; }
ppchar"line "[0-9]*     { newlineno = atoi( yytext()+6 ); goto getfname; }
:endsegment

:segment AS_ALPHA
"$"[0-9]                { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
"$"[1-2][0-9]           { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
"$"[3][0-1]             { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }

"$v0"                   { asyylval.reg = MakeReg( RC_GPR, 0 ); return( T_REGISTER ); }
"$t"[0-7]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 1 ); return( T_REGISTER ); }
"$s"[0-6]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 9 ); return( T_REGISTER ); }
"$fp"                   { asyylval.reg = MakeReg( RC_GPR, 15 ); return( T_REGISTER ); }
"$a"[0-5]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 16 ); return( T_REGISTER ); }
"$t"[8-9]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 14 ); return( T_REGISTER ); }
"$t"[1][0-1]            { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 14 ); return( T_REGISTER ); }
"$ra"                   { asyylval.reg = MakeReg( RC_GPR, 26 ); return( T_REGISTER ); }
"$pv"                   { asyylval.reg = MakeReg( RC_GPR, 27 ); return( T_REGISTER ); }
"$t12"                  { asyylval.reg = MakeReg( RC_GPR, 27 ); return( T_REGISTER ); }
"$at"                   { asyylval.reg = MakeReg( RC_GPR, 28 ); return( T_REGISTER ); }
"$gp"                   { asyylval.reg = MakeReg( RC_GPR, 29 ); return( T_REGISTER ); }
"$sp"                   { asyylval.reg = MakeReg( RC_GPR, 30 ); return( T_REGISTER ); }
"$zero"                 { asyylval.reg = MakeReg( RC_GPR, 31 ); return( T_REGISTER ); }

"$f"[0-9]               { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
"$f"[1-2][0-9]          { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
"$f"[3][0-1]            { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
:elsesegment AS_MIPS
"$"[0-9]                { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
"$"[1-2][0-9]           { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
"$"[3][0-1]             { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }

"$zero"                 { asyylval.reg = MakeReg( RC_GPR, 0 ); return( T_REGISTER ); }
"$at"                   { asyylval.reg = MakeReg( RC_GPR, 1 ); return( T_REGISTER ); }
"$v"[0-1]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 2 ); return( T_REGISTER ); }
"$a"[0-3]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 4 ); return( T_REGISTER ); }
"$t"[0-7]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 8 ); return( T_REGISTER ); }
"$s"[0-7]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 16 ); return( T_REGISTER ); }
"$t"[8-9]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 16 ); return( T_REGISTER ); }
"$k"[0-1]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+2 ) + 26 ); return( T_REGISTER ); }
"$gp"                   { asyylval.reg = MakeReg( RC_GPR, 28 ); return( T_REGISTER ); }
"$sp"                   { asyylval.reg = MakeReg( RC_GPR, 29 ); return( T_REGISTER ); }
"$s8"                   { asyylval.reg = MakeReg( RC_GPR, 30 ); return( T_REGISTER ); }
"$fp"                   { asyylval.reg = MakeReg( RC_GPR, 30 ); return( T_REGISTER ); }
"$ra"                   { asyylval.reg = MakeReg( RC_GPR, 31 ); return( T_REGISTER ); }

"$f"[0-9]               { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
"$f"[1-2][0-9]          { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
"$f"[3][0-1]            { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
:elsesegment AS_PPC
[rR][0-9]               { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
[rR][1-2][0-9]          { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
[rR][3][0-1]            { asyylval.reg = MakeReg( RC_GPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
"sp"                    { asyylval.reg = MakeReg( RC_GPR, 1 ); return( T_REGISTER ); }
"rtoc"                  { asyylval.reg = MakeReg( RC_GPR, 2 ); return( T_REGISTER ); }

[fF][0-9]               { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
[fF][1-2][0-9]          { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }
[fF][3][0-1]            { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+1 ) ); return( T_REGISTER ); }

[fF][rR][0-9]           { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
[fF][rR][1-2][0-9]      { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }
[fF][rR][3][0-1]        { asyylval.reg = MakeReg( RC_FPR, atoi( yytext()+2 ) ); return( T_REGISTER ); }

[cC][rR][bB][0-9]       { asyylval.reg = MakeReg( RC_CRB, atoi( yytext()+3 ) ); return( T_REGISTER ); }
[cC][rR][bB][1-2][0-9]  { asyylval.reg = MakeReg( RC_CRB, atoi( yytext()+3 ) ); return( T_REGISTER ); }
[cC][rR][bB][3][0-1]    { asyylval.reg = MakeReg( RC_CRB, atoi( yytext()+3 ) ); return( T_REGISTER ); }

[cC][rR][0-7]           { asyylval.reg = MakeReg( RC_CRF, atoi( yytext()+2 ) ); return( T_REGISTER ); }

/* The following symbols are defined just for the BI field of the simplified
   branch mnemonics. */
"lt"                    { asyylval.val = BI_LT; return( T_BI_OFFSET ); }
"gt"                    { asyylval.val = BI_GT; return( T_BI_OFFSET ); }
"eq"                    { asyylval.val = BI_EQ; return( T_BI_OFFSET ); }
"so"                    { asyylval.val = BI_SO; return( T_BI_OFFSET ); }
"un"                    { asyylval.val = BI_UN; return( T_BI_OFFSET ); }
:endsegment

:segment AS_ALPHA
symqual                 {
                            sym_handle sym;

                            sym = SymLookup( yytext() );
                            if( sym != NULL ) {
                                asyylval.sym = sym;
                                assert( SymClass( sym ) == SYM_INSTRUCTION );
                                return( T_OPCODE );
                            }
                            return( T_ERROR );
                        }
:endsegment

sym                     {
                            sym_handle sym;
                            char       *symstr;

                            if( *yytext() == '`' ) {
                                symstr = stripBackQuotes( _yytext );
                            } else {
                                symstr = _yytext;
                            }
                            sym = SymLookup( symstr );
                            if( sym != NULL ) {
                                asyylval.sym = sym;
                                switch( SymClass( sym ) ) {
                                case SYM_INSTRUCTION: return( T_OPCODE );
                                case SYM_LABEL: return( T_IDENTIFIER );
                                case SYM_DIRECTIVE: DirSetNextScanState( sym ); return( T_DIRECTIVE );
                                }
                            } else {
                                sym = SymAdd( symstr, SYM_LABEL );
                                asyylval.sym = sym;
                                return( T_IDENTIFIER );
                            }
                        }

[0-9]":"                { asyylval.val = *yytext() - '0' + 1; return( T_NUMERIC_LABEL ); }
[0-9]"b"                { asyylval.val = '0' - *yytext() - 1; return( T_NUMLABEL_REF ); }
[0-9]"f"                { asyylval.val = *yytext() - '0' + 1; return( T_NUMLABEL_REF ); }
[1-9][0-9]*             { asyylval.val = atoi( yytext() ); return( T_INTEGER_CONST ); }
"0"[Xx][0-9a-fA-F]+ |
"0"[0-7]*               { asyylval.val = strtoul( yytext(), NULL, 0 ); return( T_INTEGER_CONST ); }
"0"[Bb][0-1]+           { asyylval.val = strtoul( yytext()+2, NULL, 2 ); return( T_INTEGER_CONST ); }
[0-9]+"."[0-9]* |
[0-9]*"."[0-9]+         { asyylval.fval = strtod( yytext(), NULL ); return( T_FLOAT_CONST ); }
string                  {
                            MemFree( cStr );
                            asyylval.str = ( cStr = AsStrdup( yytext()+1 ) );
                            cStr[yytextlen - 2] = 0;
                            return( T_STRING_CONST );
                        }

[Ll]"^"                 { asyylval.rtype = ASM_RELOC_HALF_LO; return( T_RELOC_MODIFIER ); }
[Hh]"^"                 { asyylval.rtype = ASM_RELOC_HALF_HI; return( T_RELOC_MODIFIER ); }
[Hh][Aa]"^"             { asyylval.rtype = ASM_RELOC_HALF_HA; return( T_RELOC_MODIFIER ); }
[Jj]"^"                 { asyylval.rtype = ASM_RELOC_JUMP; return( T_RELOC_MODIFIER ); }
:segment !AS_ALPHA
[Bb]"^"                 { asyylval.rtype = ASM_RELOC_BRANCH; return( T_RELOC_MODIFIER ); }
:endsegment
"("                     { return( T_LEFT_PAREN ); }
")"                     { return( T_RIGHT_PAREN ); }
","                     { return( T_COMMA ); }
":"                     { return( T_COLON ); }
";"                     { return( T_SEMICOLON ); }

"|"                     { return( T_OR ); }
"^"                     { return( T_XOR ); }
"&"                     { return( T_AND ); }
">>"                    { return( T_SHIFT_RIGHT ); }
"<<"                    { return( T_SHIFT_LEFT ); }
"+"                     { return( T_PLUS ); }
"-"                     { return( T_MINUS ); }
"*"                     { return( T_TIMES ); }
"/"                     { return( T_DIVIDE ); }
"%"                     { return( T_MOD ); }
"~"                     { return( T_NOT ); }

:segment _STANDALONE_
nl                      { ++asyylineno; return( T_NEWLINE ); }
:endsegment
any                     {
                            if( eofPtr && ( cursor == eofPtr ) ) {
                                return( 0 ); // end of input
                            }
                            return( T_ERROR );
                        }
*/

#ifdef _STANDALONE_
getfname:   tok = cursor;
/*!re2c
ws                      { goto getfname; }
"\""fname"\""           {
                            MemFree( asyyfname );
                            asyyfname = AsStrdup( yytext()+1 );
                            asyyfname[yytextlen - 2] = 0;
                            dropDblBackSlashes( asyyfname );
                            goto getfname;
                        }
nl                      {
                            fileinfo    *file;

                            file = MemAlloc( sizeof( fileinfo ) + strlen( asyyfname ) );
                            file->line = asyylineno = newlineno;
                            strcpy( file->name, asyyfname );
                            asyylval.file = file;
                            return( T_FILE_SWITCH );
                        }
(dot \ [ \t])+          {
                            yylexError( IMPROPER_LINE_DIRECTIVE );
                            goto std;
                        }
*/
#endif

getdirop:       tok = cursor;
/*!re2c
ws                      { goto getdirop; }
((ch \ [ \t]) ch*) |
empstr                  {
                            MemFree( dirOpStr );
                            asyylval.str = ( dirOpStr = AsStrdup( yytext() ) );
                            return( T_DIRECTIVE_OPERAND );
                        }
*/
}

#ifdef __WATCOMC__
#pragma on( unreferenced );
#endif

void asyyerror( char *s ) {
//*************************
// Code within this module should use yylexError() instead

    if( *yytext() == '\n' ) {
        // to beautify the error output
        Error( PROBLEM_AT_EOL, s );
        return;
    }
    if( *_yytext == '\0' ) {
        Error( PROBLEM_AT_EOS, s );
        return;
    }
    Error( PROBLEM_AT_WHERE, s, _yytext );
}
