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


#include <stdio.h>
#include "standard.h"
#include "hostsys.h"
#include "dump.h"

extern  void    DumpChar(char);
extern  void    DumpNL();

static  char    *dumpFileName = "\\tmp\\cgdump.tmp";
static  FILE    *dumpFile;

extern  void    DumpRedirect() {
/******************************/

    if( dumpFile != NULL ) return;
    dumpFile = fopen( dumpFileName, "wt" );
}

extern  void    DumpUnredirect() {
/********************************/

    if( dumpFile == NULL ) return;
    fclose( dumpFile );
    dumpFile = NULL;
}

extern  void    DumpChar( char c ) {
/**********************************/

    if( dumpFile != NULL ) {
        fputc( c, dumpFile );
    } else {
        fputc( c, stdout );
    }
}

extern  void    DumpNL() {
/************************/

    FILE        *fp;

    fp = (dumpFile != NULL) ? dumpFile : stdout;

    fputc( '\n', fp );
    fflush( fp );
}

extern  void    DumpPadString( char *s, int i ) {
/***********************************************/

    while( *s ) {
        DumpChar( *s );
        ++s;
        --i;
    }
    while( --i >= 0 ) {
        DumpChar( ' ' );
    }
}


extern  void    DumpString( char const *s ) {
/*************************************/

    while( *s ) {
        DumpChar( *s );
        s++;
    }
}


extern  void    DumpXString( char const *s ) {
/**************************************/

    DumpString( s );
}


extern  void    DumpByte( byte n ) {
/**********************************/

    char        c;

    c = (n>>4) & 0x0f;
    if( c > 9 ) {
        c -= 10;
        c += 'A';
    } else {
        c += '0';
    }
    DumpChar( c );
    c = n & 0x0f;
    if( c > 9 ) {
        c -= 10;
        c += 'A';
    } else {
        c += '0';
    }
    DumpChar( c );
}

extern  void    Dump8h( unsigned_32 n ) {
/***************************************/

    DumpByte( n >> 24 );
    DumpByte( n >> 16 );
    DumpByte( n >> 8  );
    DumpByte( n >> 0  );
}


extern  void    DumpLLong(  signed_32 n,  int  len ) {
/******************************************************/

    char        b[30];
    char        *bp;
    bool        neg;

    bp = b;
    bp += 20;
    *--bp = NULLCHAR;
    if( n < 0 ) {
        neg = TRUE;
        n = -n;
    } else {
        neg = FALSE;
    }
    if( n != 0 ) {
        for(;;) {
            *--bp = n % 10 + '0';
            --len;
            n /= 10;
            if( n == 0 ) break;
        }
    } else {
        *--bp = '0';
        --len;
    }
    if( neg ) {
        *--bp = '-';
        --len;
    }
    while( --len >= 0 ) {
        *--bp = ' ';
    }
    DumpXString( bp );
}

extern  void    DumpLong( signed_32 n ) {
/***************************************/

    DumpLLong( n, 0 );
}

extern  void    DumpInt( int n ) {
/********************************/

    DumpLong( n );
}


extern  void    DumpId( unsigned id ) {
/*******************************************/


    DumpChar( '(' );
    DumpLLong( id, 8 );
    DumpChar( ')' );
}

extern  void    DumpPtr( pointer ptr ) {
/**************************************/

    Dump8h( (unsigned_32)ptr );
}
