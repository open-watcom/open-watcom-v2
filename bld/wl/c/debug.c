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
* Description:  Support routines for debug build of linker.
*
****************************************************************************/


#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "linkstd.h"
#include "msg.h"
#include "ideentry.h"
#include "alloc.h"

#ifdef _INT_DEBUG

long unsigned SpyWrite = -1;

static jmp_buf  lj;
static int      segViolationCount;

static void RecordSegViolation( int sig_number )
{
    sig_number = sig_number;
    segViolationCount++;
    longjmp( lj, 1 );
}

bool CanReadWord( void *p )
{
    int     c = segViolationCount;
    int     temp = 0;
    bool    retval;

    signal( SIGSEGV, RecordSegViolation );
    if( 0 == setjmp( lj ) ) {
        temp = *(int *)p;   // try to access pointer
    } else {
    }
    if( temp != 0 ) {       // fool optimizer to keep temp around
        *(int *)p = temp;
    }
    if( segViolationCount != c ) {
        retval = false;
    } else {
        retval = true;
    }
    signal( SIGSEGV, SIG_DFL );
    return( retval );
}

//---------------------------------------------------------------

void LPrint( const char *str, ... )
{
    va_list     arglist;
    char        buff[200];
    size_t      len;

    va_start( arglist, str );
    len = DoFmtStr( buff, 200, str, &arglist );
    WriteStdOut( buff );
}

//---------------------------------------------------------------

extern int Debug;

void _Debug( unsigned int mask, const char *str, ... )
{
    va_list     arglist;
    char        buff[128];
    size_t      len;

    if( (Debug & mask) || (mask & DBG_INFO_MASK) == DBG_ALWAYS ) {
        va_start( arglist, str );
        len = DoFmtStr( buff, 128, str, &arglist );
        WriteStdOut( buff );
        if( (mask & DBG_NOCRLF) == 0 ) {
            WriteStdOutNL();
        }
    }
}

//---------------------------------------------------------------

long unsigned   TrecCount;
bool            TrecHit = false;

static void TrecFailCondition( void )
// set TrecHit here if failure detected:
// Here you may put any condition you like
{
#ifdef TRMEM
    if( !ValidateMem() ) {
        TrecHit = true;
    }
#endif
}

void Trec( const char *str, ... )
{
    enum { max=10 };
    static int  currBuff;
    static char buff[max][128];
    va_list     arglist;

    TrecCount++;
    va_start( arglist, str );
    DoFmtStr( buff[currBuff], 128, str, &arglist );
    currBuff = ( currBuff + 1 ) % max;

    TrecFailCondition();

    if( TrecHit ) {
        char    buf[100];
        int     i, j;

        FmtStr( buf, 100, "Trec break! Count=%d. Trace history:\n", TrecCount );
        WriteStdOut( buf );
        for( i = currBuff, j = 0; j < max; j++, i = ( i + 1 ) % max ) {
            if( buff[i] != NULL ) {
                WriteStdOut( buff[i] );
                WriteStdOutNL();
            }
        }
        LnkFatal( "trec sucess of failure" );
    }
}

//---------------------------------------------------------------

static char Digit2Char( unsigned d )
{
    if( d >= 10 ) {
        return( d + 'A' - 10 );
    } else {
        return( d + '0' );
    }
}

static void OneLineDumpByte( const unsigned char *p, int size )
{
    int i;
    int len;
    enum { max = 80 };
    int ascii_offset = 60;
    char buf[max + 1];

    len = FmtStr( buf, max, "%h ", p );
    for( i = 0; i < size; i++ ) {
        if( len >= max - 4 ) {
            break;
        }
        if( i % 8 == 0 ) {
            buf[len++] = ' ';
        }
        buf[len++] = Digit2Char( p[i] / 16 );
        buf[len++] = Digit2Char( p[i] % 16 );
        buf[len++] = ' ';
    }

    if( len >= ascii_offset )
        ascii_offset = len;
    if( max < ascii_offset )
        ascii_offset = max;
    memset( buf + len, ' ', ascii_offset - len );
    len = ascii_offset;

    for( i = 0; i < size; i++ ) {
        if( len >= max - 2 ) {
            break;
        }
        buf[len++] = isprint( p[i] ) ? p[i] : '.';
    }
    buf[len++] = '\n';
    buf[len] = 0;
    WriteStdOut( buf );
}

static void OneLineDumpDWord( const unsigned_32 *p, int byte_size )
{
    int     size = byte_size / sizeof( p[0] );
    size_t  max = 80;
    char    buf[80 + 10];
    size_t  len;
    int     i;

    len = FmtStr( buf, max, "%h: ", p );
    for( i = 0; i < size; i++ ) {
        len += FmtStr( buf + len, max - len, " %h", p[i] );
        if( len > max ) {
            break;
        }
    }
    buf[len++] = '\n';
    buf[len] = 0;
    WriteStdOut( buf );
}

void PrintMemDump( const void *p, unsigned long size, DbgDumpType type )
{
    const unsigned char *buf = p;
    static struct {
        void (*OneLineDump)(const void *, int);
        int max;
    } MemDump[DUMP_MAX] = {
        { (void(*)(const void *,int))OneLineDumpByte,  16 },   // DUMP_BYTE
        { (void(*)(const void *,int))OneLineDumpByte,  16 },   // DUMP_WORD: NYI
        { (void(*)(const void *,int))OneLineDumpDWord, 4 * 4 } // DUMP_DWORD
    };
    int max = MemDump[type].max;

    for( ; size > max; size -= max ) {
        MemDump[type].OneLineDump( buf, max );
        buf += max;
    }
    MemDump[type].OneLineDump( buf, size );
}

#endif
