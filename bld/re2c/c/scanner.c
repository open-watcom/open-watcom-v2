/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wio.h"
#include "scanner.h"
#include "parser.h"
#include "ytab.h"

extern YYSTYPE yylval;

#define BSIZE   8192

#define YYCTYPE         uchar
#define YYCURSOR        cursor
#define YYLIMIT         s->lim
#define YYMARKER        s->ptr
#define YYFILL(n)       {cursor = fill( s, cursor );}

#define RETURN(i)       {s->cur = cursor; return i;}

static void Scanner_init( Scanner *s, FILE *i )
{
    s->in = i;
    s->bot = s->tok = s->ptr = s->cur = s->pos = s->lim = s->top = s->eof = NULL;
    s->tchar = s->tline = 0;
    s->cline = 1;
}

static SubStr Scanner_token( Scanner *s )
{
    SubStr  r;

    SubStr_init( &r, (char *)s->tok, s->cur - s->tok );
    return( r );
}

static uchar *fill( Scanner *s, uchar *cursor )
{
    if( s->eof == NULL ) {
        uint cnt = s->tok - s->bot;
        if( cnt ){
            memcpy( s->bot, s->tok, s->lim - s->tok );
            s->tok = s->bot;
            s->ptr -= cnt;
            cursor -= cnt;
            s->pos -= cnt;
            s->lim -= cnt;
        }
        if( ( s->top - s->lim ) < BSIZE ) {
            uchar *buf = malloc( ( s->lim - s->bot ) + BSIZE );
            memcpy( buf, s->tok, s->lim - s->tok );
            s->tok = buf;
            s->ptr = &buf[s->ptr - s->bot];
            cursor = &buf[cursor - s->bot];
            s->pos = &buf[s->pos - s->bot];
            s->lim = &buf[s->lim - s->bot];
            s->top = &s->lim[BSIZE];
            free( s->bot );
            s->bot = buf;
        }
        if( (cnt = fread( s->lim, 1, BSIZE, s->in )) != BSIZE ) {
            s->eof = &s->lim[cnt]; *s->eof++ = '\n';
        }
        s->lim += cnt;
    }
    return( cursor );
}

static Token *token( Scanner *s )
{
    Token   *r = malloc( sizeof( Token ) );
    uint    len = s->cur - s->tok;

    r->line = s->tline;
    r->text.len = len;
    r->text.str = malloc( len );
    memcpy( r->text.str, s->tok, len );
    return( r );
}


int Scanner_echo( Scanner *s, FILE *out )
{
    uchar   *cursor = s->cur;

    s->tok = cursor;
echo:
{
        YYCTYPE yych;
        uint yyaccept;
        goto yy0;
yy1:    ++YYCURSOR;
yy0:
        if((YYLIMIT - YYCURSOR) < 7) YYFILL(7);
        yych = *YYCURSOR;
        if(yych == '\n')        goto yy4;
        if(yych != '/') goto yy6;
yy2:    yyaccept = 0;
        yych = *(YYMARKER = ++YYCURSOR);
        if(yych == '*') goto yy7;
yy3:
        { goto echo; }
yy4:    yych = *++YYCURSOR;
yy5:
        { if( cursor == s->eof )
                                  RETURN(0);
                                  fwrite( s->tok, 1, cursor - s->tok, out );
                                  s->tok = s->pos = cursor; s->cline++;
                                  goto echo; }
yy6:    yych = *++YYCURSOR;
        goto yy3;
yy7:    yych = *++YYCURSOR;
        if(yych == '!') goto yy9;
yy8:    YYCURSOR = YYMARKER;
        switch(yyaccept){
        case 0: goto yy3;
        }
yy9:    yych = *++YYCURSOR;
        if(yych != 'r') goto yy8;
yy10:   yych = *++YYCURSOR;
        if(yych != 'e') goto yy8;
yy11:   yych = *++YYCURSOR;
        if(yych != '2') goto yy8;
yy12:   yych = *++YYCURSOR;
        if(yych != 'c') goto yy8;
yy13:   yych = *++YYCURSOR;
yy14:
        { fwrite( s->tok, 1, &cursor[-7] - s->tok, out );
                                  s->tok = cursor;
                                  RETURN(1); }
}

}


int Scanner_scan( Scanner *s )
{
    uchar   *cursor = s->cur;
    uint    depth;

scan:
    s->tchar = cursor - s->pos;
    s->tline = s->cline;
    s->tok = cursor;
{
        YYCTYPE yych;
        uint yyaccept;
        goto yy15;
yy16:   ++YYCURSOR;
yy15:
        if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
        yych = *YYCURSOR;
        if(yych <= ':'){
                if(yych <= '"'){
                        if(yych <= '\n'){
                                if(yych <= '\b')        goto yy35;
                                if(yych <= '\t')        goto yy31;
                                goto yy33;
                        } else {
                                if(yych == ' ') goto yy31;
                                if(yych <= '!') goto yy35;
                                goto yy23;
                        }
                } else {
                        if(yych <= '*'){
                                if(yych <= '\'')        goto yy35;
                                if(yych <= ')') goto yy27;
                                goto yy21;
                        } else {
                                if(yych <= '+') goto yy28;
                                if(yych == '/') goto yy19;
                                goto yy35;
                        }
                }
        } else {
                if(yych <= 'Z'){
                        if(yych <= '='){
                                if(yych == '<') goto yy35;
                                goto yy27;
                        } else {
                                if(yych == '?') goto yy28;
                                if(yych <= '@') goto yy35;
                                goto yy29;
                        }
                } else {
                        if(yych <= '`'){
                                if(yych <= '[') goto yy25;
                                if(yych <= '\\')        goto yy27;
                                goto yy35;
                        } else {
                                if(yych <= 'z') goto yy29;
                                if(yych <= '{') goto yy17;
                                if(yych <= '|') goto yy27;
                                goto yy35;
                        }
                }
        }
yy17:   yych = *++YYCURSOR;
yy18:
        { depth = 1;
                                  goto code;
                                }
yy19:   yych = *++YYCURSOR;
        if(yych == '*') goto yy54;
yy20:
        { RETURN(*s->tok); }
yy21:   yych = *++YYCURSOR;
        if(yych == '/') goto yy52;
yy22:
        { yylval.op = *s->tok;
                                  RETURN(CLOSE); }
yy23:   yyaccept = 0;
        yych = *(YYMARKER = ++YYCURSOR);
        if(yych != '\n')        goto yy48;
yy24:
        { Scanner_fatal( s, "bad string" ); }
yy25:   yyaccept = 1;
        yych = *(YYMARKER = ++YYCURSOR);
        if(yych != '\n')        goto yy42;
yy26:
        { Scanner_fatal( s, "bad character constant" ); }
yy27:   yych = *++YYCURSOR;
        goto yy20;
yy28:   yych = *++YYCURSOR;
        goto yy22;
yy29:   yych = *++YYCURSOR;
        goto yy40;
yy30:
        { s->cur = cursor;
                                  yylval.symbol = Symbol_find( Scanner_token( s ) );
                                  return ID; }
yy31:   yych = *++YYCURSOR;
        goto yy38;
yy32:
        { goto scan; }
yy33:   yych = *++YYCURSOR;
yy34:
        { if(cursor == s->eof) RETURN(0);
                                  s->pos = cursor; s->cline++;
                                  goto scan;
                                }
yy35:   yych = *++YYCURSOR;
yy36:
        { fprintf( stderr, "unexpected character: %s\n", *s->tok );
                                  goto scan;
                                }
yy37:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
yy38:   if(yych == '\t')        goto yy37;
        if(yych == ' ') goto yy37;
        goto yy32;
yy39:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
yy40:   if(yych <= '@'){
                if(yych <= '/') goto yy30;
                if(yych <= '9') goto yy39;
                goto yy30;
        } else {
                if(yych <= 'Z') goto yy39;
                if(yych <= '`') goto yy30;
                if(yych <= 'z') goto yy39;
                goto yy30;
        }
yy41:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
yy42:   if(yych <= '['){
                if(yych != '\n')        goto yy41;
        } else {
                if(yych <= '\\')        goto yy44;
                if(yych <= ']') goto yy45;
                goto yy41;
        }
yy43:   YYCURSOR = YYMARKER;
        switch(yyaccept){
        case 0: goto yy24;
        case 1: goto yy26;
        }
yy44:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
        if(yych == '\n')        goto yy43;
        goto yy41;
yy45:   yych = *++YYCURSOR;
yy46:
        { s->cur = cursor;
                                  yylval.regexp = ranToRE( Scanner_token( s ) );
                                  return RANGE; }
yy47:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
yy48:   if(yych <= '!'){
                if(yych == '\n')        goto yy43;
                goto yy47;
        } else {
                if(yych <= '"') goto yy50;
                if(yych != '\\')        goto yy47;
        }
yy49:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
        if(yych == '\n')        goto yy43;
        goto yy47;
yy50:   yych = *++YYCURSOR;
yy51:
        { s->cur = cursor;
                                  yylval.regexp = strToRE( Scanner_token( s ) );
                                  return STRING; }
yy52:   yych = *++YYCURSOR;
yy53:
        { s->tok = cursor;
                                  RETURN(0); }
yy54:   yych = *++YYCURSOR;
yy55:
        { depth = 1;
                                  goto comment; }
}


code:
{
        YYCTYPE yych;
        uint yyaccept;
        goto yy56;
yy57:   ++YYCURSOR;
yy56:
        if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
        yych = *YYCURSOR;
        if(yych <= '&'){
                if(yych <= '\n'){
                        if(yych <= '\t')        goto yy64;
                        goto yy62;
                } else {
                        if(yych == '"') goto yy66;
                        goto yy64;
                }
        } else {
                if(yych <= '{'){
                        if(yych <= '\'')        goto yy67;
                        if(yych <= 'z') goto yy64;
                        goto yy60;
                } else {
                        if(yych != '}') goto yy64;
                }
        }
yy58:   yych = *++YYCURSOR;
yy59:
        { if(--depth == 0){
                                        s->cur = cursor;
                                        yylval.token = token( s );
                                        return CODE;
                                  }
                                  goto code; }
yy60:   yych = *++YYCURSOR;
yy61:
        { ++depth;
                                  goto code; }
yy62:   yych = *++YYCURSOR;
yy63:
        { if(cursor == s->eof) Scanner_fatal( s, "missing '}'" );
                                  s->pos = cursor; s->cline++;
                                  goto code;
                                }
yy64:   yych = *++YYCURSOR;
yy65:
        { goto code; }
yy66:   yyaccept = 0;
        yych = *(YYMARKER = ++YYCURSOR);
        if(yych == '\n')        goto yy65;
        goto yy73;
yy67:   yyaccept = 0;
        yych = *(YYMARKER = ++YYCURSOR);
        if(yych == '\n')        goto yy65;
        goto yy69;
yy68:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
yy69:   if(yych <= '&'){
                if(yych != '\n')        goto yy68;
        } else {
                if(yych <= '\'')        goto yy64;
                if(yych == '\\')        goto yy71;
                goto yy68;
        }
yy70:   YYCURSOR = YYMARKER;
        switch(yyaccept){
        case 0: goto yy65;
        }
yy71:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
        if(yych == '\n')        goto yy70;
        goto yy68;
yy72:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
yy73:   if(yych <= '!'){
                if(yych == '\n')        goto yy70;
                goto yy72;
        } else {
                if(yych <= '"') goto yy64;
                if(yych != '\\')        goto yy72;
        }
yy74:   ++YYCURSOR;
        if(YYLIMIT == YYCURSOR) YYFILL(1);
        yych = *YYCURSOR;
        if(yych == '\n')        goto yy70;
        goto yy72;
}


comment:
{
        YYCTYPE yych;
        goto yy75;
yy76:   ++YYCURSOR;
yy75:
        if((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
        yych = *YYCURSOR;
        if(yych <= ')'){
                if(yych == '\n')        goto yy80;
                goto yy82;
        } else {
                if(yych <= '*') goto yy77;
                if(yych == '/') goto yy79;
                goto yy82;
        }
yy77:   yych = *++YYCURSOR;
        if(yych == '/') goto yy85;
yy78:
        { goto comment; }
yy79:   yych = *++YYCURSOR;
        if(yych == '*') goto yy83;
        goto yy78;
yy80:   yych = *++YYCURSOR;
yy81:
        { if(cursor == s->eof) RETURN(0);
                                  s->tok = s->pos = cursor; s->cline++;
                                  goto comment;
                                }
yy82:   yych = *++YYCURSOR;
        goto yy78;
yy83:   yych = *++YYCURSOR;
yy84:
        { ++depth;
                                  goto comment; }
yy85:   yych = *++YYCURSOR;
yy86:
        { if(--depth == 0)
                                        goto scan;
                                    else
                                        goto comment; }
}

}

void Scanner_fatal( Scanner *s, const char *msg ) {
    fprintf( stderr, "line %d, column %d: %s\n", s->tline, s->tchar + 1, msg );
    exit(1);
}

Scanner *Scanner_new( FILE *i )
{
    Scanner *r = malloc( sizeof( Scanner ) );

    Scanner_init( r, i );
    return( r );
}

