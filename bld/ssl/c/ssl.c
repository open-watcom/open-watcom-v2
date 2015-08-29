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

#include "clibext.h"


token           CurrToken;
char            TokenBuff[MAX_TOKEN_LEN];
unsigned        TokenLen;
FILE            *TblFile;
FILE            *PrsFile;
char            CurrFile[256];
unsigned        LineNum;
char            Language;
unsigned        Offset;
int             SavedChar;


void OutByte( unsigned char byte )
{
    static char Digs[] = "0123456789abcdef";

    if( Language ) {
        if( Offset > 70 ) {
            fputc( '\n', PrsFile );
            Offset = 0;
        }
        fputs( " 0x", PrsFile );
        fputc( Digs[ byte >> 4 ] , PrsFile );
        fputc( Digs[ byte & 0xf ], PrsFile );
        fputc( ',', PrsFile );
        Offset += 6;
    } else {
        fputc( byte, PrsFile );
    }
}

void OutWord( unsigned short word )
{

    OutByte( word & 0xff );
    OutByte( word >> 8 );
}

void OutStartSect( char *name, unsigned short len )
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
{
    if( Language ) {
        fputs( "\n};\n", PrsFile );
    }
}


void Dump( char *fmt, ... )
{
    va_list     arglist;

    if( TblFile == NULL ) return;
    va_start( arglist, fmt );
    vfprintf( TblFile, fmt, arglist );
    va_end( arglist );
}


void Error( char *fmt, ... )
{
    va_list     arglist;

    if( CurrFile[0] != '\0' ) {
        fprintf( stderr, "%s(%u) Error! ", CurrFile, LineNum );
    }
    va_start( arglist, fmt );
    vfprintf( stderr, fmt, arglist );
    putc( '\n', stderr );
    va_end( arglist );
    exit( 1 );
}


static void Usage( void )
{
    Error( "Usage: ssl {-(v|c)} filename[.ssl] [out_file]" );
}

void UngetChar( int c )
{
    SavedChar = c;
}

int NextChar( void )
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
{
    return( LineNum );
}


static void OpenFiles( char verbose, char *path, char *out_file )
{
    char        buff[_MAX_PATH2];
    char        file_name[_MAX_PATH];
    char        *drive;
    char        *dir;
    char        *fname;
    char        *ext;
    int         given;

    _splitpath2( path, buff, &drive, &dir, &fname, &ext );
    if( ext == NULL || ext[0] == '\0' ) {
        ext = ".ssl";
    }
    _makepath( file_name, drive, dir, fname, ext );
    PreProcChar = '!';
    if( PP_Init( file_name, PPFLAG_EMIT_LINE, NULL ) != 0 ) {
        Error( "Unable to open '%s'", file_name );
    }
    given = 1;
    if( out_file == NULL ) {
        out_file = path;
        given = 0;
    }
    _splitpath2( out_file, buff, &drive, &dir, &fname, &ext );
    if( !given ) {
        drive = "";
        dir = "";
        ext = "";
    }
    if( ext == NULL || ext[0] == '\0' ) {
        ext = ".prs";
    }
    _makepath( file_name, drive, dir, fname, ext );
    PrsFile = fopen( file_name, Language ? "wt" : "wb" );
    if( PrsFile == NULL ) Error( "can not open '%s'", file_name );
    if( verbose ) {
        _makepath( file_name, drive, dir, fname, ".tbl" );
        TblFile = fopen( file_name, "w" );
        if( TblFile == NULL ) Error( "can not open '%s'", file_name );
    }
}


static void CloseFiles( void )
{
    fclose( PrsFile );
    if( TblFile != NULL ) fclose( TblFile );
}


unsigned short GetNum( void )
{
    char        *end;
    unsigned    value;

    if( CurrToken != T_NAME ) Error( "expecting number" );
    value = strtol( TokenBuff, &end, 0 );
    if( *end != '\0' ) Error( "invalid number" );
    return( value );
}


void Scan( void )
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
            if( ch != '\n' ) break;
            ++LineNum;
        }
        if( ch != '%' ) break;
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
            if( ch == term ) break;
            if( ch == '\\' ) ch = NextChar();
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
    if( TokenBuff[0]==PreProcChar && strcmp( &TokenBuff[1], "line" )==0 ) {
        Scan();
        LineNum = strtoul( TokenBuff, NULL, 0 ) - 1;
        Scan();
        strcpy( CurrFile, TokenBuff );
        Scan();
    } else if( TokenBuff[0]==PreProcChar && strcmp( &TokenBuff[1], "error" )==0 ) {
        TokenLen = 0;
        for( ;; ) {
            ch = NextChar();
            if( ch == '\n' || ch == EOF ) break;
            TokenBuff[TokenLen++] = ch;
        }
        TokenBuff[TokenLen] = '\0';
        Error( TokenBuff );
    }
}


void WantColon( void )
{
    if( CurrToken != T_COLON ) Error( "expecting ':'" );
    Scan();
}


static void Parse( void )
{
    Decls();
    if( CurrToken != T_RULES ) Error( "expecting rules" );
    Scan();
    Rules();
}


int main( int argc, char *argv[] )
{
    char        *file;
    char        verbose;

    if( argc < 2 ) Usage();
    file = argv[1];
    if( strcmp( file, "?" ) == 0 ) Usage();
    verbose = 0;
    Language = 0;
    for( ;; ) {
        file = *++argv;
        if( file == 0 ) Usage();
        if( file[0] != '-' ) break;
        switch( file[1] ) {
        case 'v':
            verbose = 1;
            break;
        case 'c':
            Language = 1;
            break;
        default:
            Usage();
            break;
        }
    }
    OpenFiles( verbose, file, argv[1] );
    Scan();
    Parse();
    GenCode();
    Dump( " #### CODE ####\n\n" );
    DumpGenCode();
    Dump( "\n\n #### DECLS ####\n\n" );
    DumpSymTbl();
    CloseFiles();
    return( 0 );
}
