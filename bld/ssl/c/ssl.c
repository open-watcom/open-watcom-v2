/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Mainline for ssl utility.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "preproc.h"
#include "ssl.h"
#include "sslint.h"
#if defined( __UNIX__ ) && defined( __WATCOMC__ )
  #if ( __WATCOMC__ < 1300 )
    // fix for OW 1.9
    #include <limits.h>
  #endif
#endif
#include "pathgrp2.h"

#include "clibext.h"


token           CurrToken;
char            TokenBuff[MAX_TOKEN_LEN];
unsigned        TokenLen;
FILE            *TblFile;
FILE            *PrsFile;
char            CurrFile[256];
unsigned        LineNum;
bool            Language;
unsigned        Offset;
int             SavedChar;


void OutByte( unsigned char byte )
/********************************/
{
    static char Digs[] = "0123456789abcdef";

    if( Language ) {
        if( Offset > 70 ) {
            fputc( '\n', PrsFile );
            Offset = 0;
        }
        fputs( " 0x", PrsFile );
        fputc( Digs[byte >> 4] , PrsFile );
        fputc( Digs[byte & 0xf], PrsFile );
        fputc( ',', PrsFile );
        Offset += 6;
    } else {
        fputc( byte, PrsFile );
    }
}

void OutWord( unsigned short word )
/*********************************/
{

    OutByte( word & 0xff );
    OutByte( word >> 8 );
}

void OutStartSect( char *name, unsigned short len )
/*************************************************/
{
    if( Language ) {
        fputs( "char ", PrsFile );
        fputs( name, PrsFile );
        fputs( "[] = {\n", PrsFile );
        Offset = 0;
    } else {
        OutWord( len );
    }
}

void OutEndSect( void )
/*********************/
{
    if( Language ) {
        fputs( "\n};\n", PrsFile );
    }
}


void Dump( char *fmt, ... )
/*************************/
{
    va_list     args;

    if( TblFile == NULL )
        return;
    va_start( args, fmt );
    vfprintf( TblFile, fmt, args );
    va_end( args );
}


void Error( char *fmt, ... )
/**************************/
{
    va_list     args;

    if( CurrFile[0] != '\0' ) {
        fprintf( stderr, "%s(%u) Error! ", CurrFile, LineNum );
    }
    va_start( args, fmt );
    vfprintf( stderr, fmt, args );
    putc( '\n', stderr );
    va_end( args );
    exit( 1 );
}


static void Usage( void )
/***********************/
{
    Error( "Usage: ssl {-(v|c)} filename[.ssl] [out_file]" );
}

static void UngetChar( int c )
/****************************/
{
    SavedChar = c;
}

static int NextChar( void )
/*************************/
{
    int next;

    if( SavedChar != '\0' ) {
        next = SavedChar;
        SavedChar = '\0';
    } else {
        next = PP_Char();
    }
    return( next );
}

unsigned short SrcLine( void )
/****************************/
{
    return( LineNum );
}

int PP_MBCharLen( const char *p )
/*******************************/
{
    /* unused parameters */ (void)p;

    return( 1 );
}

static void OpenFiles( bool verbose, char *path, char *out_file )
/***************************************************************/
{
    pgroup2     pg;
    char        file_name[_MAX_PATH];
    bool        given;

    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] == '\0' )
        pg.ext = "ssl";
    _makepath( file_name, pg.drive, pg.dir, pg.fname, pg.ext );
    if( PP_FileInit( file_name, PPFLAG_EMIT_LINE | PPFLAG_TRUNCATE_FILE_NAME, NULL ) != 0 ) {
        Error( "Unable to open '%s'", file_name );
    }
    given = true;
    if( out_file == NULL ) {
        out_file = path;
        given = false;
    }
    _splitpath2( out_file, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( !given ) {
        pg.drive = "";
        pg.dir = "";
        pg.ext = "";
    }
    if( pg.ext[0] == '\0' )
        pg.ext = "prs";
    _makepath( file_name, pg.drive, pg.dir, pg.fname, pg.ext );
    PrsFile = fopen( file_name, Language ? "wt" : "wb" );
    if( PrsFile == NULL )
        Error( "can not open '%s'", file_name );
    if( verbose ) {
        _makepath( file_name, pg.drive, pg.dir, pg.fname, "tbl" );
        TblFile = fopen( file_name, "w" );
        if( TblFile == NULL ) {
            Error( "can not open '%s'", file_name );
        }
    }
}


static void CloseFiles( void )
/****************************/
{
    fclose( PrsFile );
    if( TblFile != NULL ) {
        fclose( TblFile );
    }
    PP_FileFini();
}


unsigned short GetNum( void )
/***************************/
{
    char        *end;
    unsigned    value;

    if( CurrToken != T_NAME )
        Error( "expecting number" );
    value = strtol( TokenBuff, &end, 0 );
    if( *end != '\0' )
        Error( "invalid number" );
    return( value );
}


void Scan( void )
/***************/
{
    static char Delims[] =
    {';',':','?','.','#','{','}','[',']','(',')','>','|','*','@',',','=','\0'};
    static char *Keywords[] =
    {"input","output","error","type","mechanism","rules",NULL};
    int         ch;
    int         term;
    char        *delim;
    char        **key;


    CurrToken = T_BAD_CHAR;
    TokenLen = 0;
    for( ;; ) {
        /* eat white space */
        for( ;; ) {
            do {
                ch = NextChar();
            } while( ch == ' ' || ch == '\t' );
            if( ch != '\n' )
                break;
            ++LineNum;
        }
        if( ch != '%' )
            break;
        /* eat comment */
        do {
            ch = NextChar();
        } while( ch != '\n' && ch != EOF );
        LineNum++;
    }
    if( ch == EOF ) {
        CurrToken = T_EOF;
        return;
    }
    for( delim = Delims; *delim != '\0'; ++delim ) {
        if( *delim == ch ) {
            CurrToken = delim - Delims + T_SEMI;
            if( CurrToken == T_GT ) {
                ch = NextChar();
                if( *delim == ch ) {
                    CurrToken = T_GT_GT;
                    return;
                }
                UngetChar( ch );
            }
            return;
        }
    }
    if( ch == '\'' || ch == '"' ) {
        term = ch;
        /* literal */
        CurrToken = T_LITERAL;
        for( ;; ) {
            ch = NextChar();
            if( ch == term )
                break;
            if( ch == '\\' )
                ch = NextChar();
            if( ch == '\n' || ch == EOF ) {
                Error( "missing closing quote" );
            }
            TokenBuff[TokenLen++] = ch;
        }
        TokenBuff[TokenLen] = '\0';
        return;
    }
    /* normal name, maybe keyword */
    CurrToken = T_NAME;
    do {
        TokenBuff[TokenLen++] = ch;
        ch = NextChar();
    } while( ch == '_' || isalnum( ch ) );
    UngetChar( ch );
    TokenBuff[TokenLen] = '\0';
    for( key = Keywords; *key != NULL; ++key ) {
        if( strcmp( *key, TokenBuff ) == 0 ) {
            CurrToken = key - Keywords + T_INPUT;
            return;
        }
    }
    if( strcmp( TokenBuff, "!line" ) == 0 ) {
        Scan();
        LineNum = strtoul( TokenBuff, NULL, 0 ) - 1;
        Scan();
        strcpy( CurrFile, TokenBuff );
        Scan();
    } else if( strcmp( TokenBuff, "!error" ) == 0 ) {
        TokenLen = 0;
        for( ;; ) {
            ch = NextChar();
            if( ch == '\n' || ch == EOF )
                break;
            TokenBuff[TokenLen++] = ch;
        }
        TokenBuff[TokenLen] = '\0';
        Error( TokenBuff );
    }
}


void WantColon( void )
/********************/
{
    if( CurrToken != T_COLON )
        Error( "expecting ':'" );
    Scan();
}


static void Parse( void )
/***********************/
{
    Decls();
    if( CurrToken != T_RULES )
        Error( "expecting rules" );
    Scan();
    Rules();
}


int main( int argc, char *argv[] )
/********************************/
{
    char        *file;
    bool        verbose;

    if( argc < 2 )
        Usage();
    file = argv[1];
    if( strcmp( file, "?" ) == 0 )
        Usage();
    verbose = false;
    Language = false;
    for( ;; ) {
        file = *++argv;
        if( file == NULL )
            Usage();
        if( file[0] != '-' )
            break;
        switch( file[1] ) {
        case 'v':
            verbose = true;
            break;
        case 'c':
            Language = true;
            break;
        default:
            Usage();
            break;
        }
    }
    PP_Init( '!' );
    OpenFiles( verbose, file, argv[1] );
    Scan();
    Parse();
    GenCode();
    Dump( " #### CODE ####\n\n" );
    DumpGenCode();
    Dump( "\n\n #### DECLS ####\n\n" );
    DumpSymTbl();
    CloseFiles();
    PP_Fini();
    return( 0 );
}
