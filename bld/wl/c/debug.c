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

static jmp_buf lj;
long unsigned SpyWrite = -1;

static int segViolationCount;
static void RecordSegViolation(int sig_number) {
    sig_number = sig_number;
    segViolationCount++;
    longjmp(lj, 1);
}

int CanReadWord(void *p) {
    int c = segViolationCount;
    int  temp = 0;
    int retval;
    signal(SIGSEGV, RecordSegViolation);
    if ( 0 == setjmp(lj)) {
        temp = *(int*)p;    // try to access pointer
    } else {
    }
    if (temp != 0) {    // fool optimizer to keep temp around
        *(int*)p = temp;
    }
    if (segViolationCount != c) {
        retval = 0;
    } else {
        retval = 1;
    }
    signal(SIGSEGV, SIG_DFL);
    return retval;
}

//---------------------------------------------------------------

void LPrint( char *str, ... ) {
    va_list     arglist;
    char        buff[200];
    unsigned    len;

    va_start( arglist, str );
    len = DoFmtStr( buff, 200, str, &arglist );
    WriteStdOut( buff );
}

//---------------------------------------------------------------

extern int Debug;

void _Debug( unsigned int mask, char *str, ... )
{
    va_list     arglist;
    char        buff[128];
    unsigned    len;

    if( Debug & mask || (mask & DBG_INFO_MASK) == DBG_ALWAYS ) {
        va_start( arglist, str );
        len = DoFmtStr( buff, 128, str, &arglist );
        WriteStdOut( buff );
        if( !(mask & DBG_NOCRLF) ) {
            WriteNLStdOut();
        }
    }
}

//---------------------------------------------------------------

long unsigned TrecCount;
int TrecHit;

static void TrecFailCondition(void) {
    // set TrecHit here if failure detected:
    // Here you may put any condition you like

#ifdef TRMEM
    if( !ValidateMem() ) {
        TrecHit = 1;
    }
#endif
}

void Trec(char *str, ...) {
    enum { max=10 };
    static int  currBuff;
    static char buff[max][128];
    va_list     arglist;

    TrecCount++;
    va_start( arglist, str );
    DoFmtStr( buff[currBuff], 128, str, &arglist );
    currBuff = (currBuff+1)%max;

    TrecFailCondition();

    if( TrecHit ) {
        char buf[100];
        int i,j;
        FmtStr(buf, 100, "Trec break! Count=%d. Trace history:\n", TrecCount);
        WriteStdOut(buf);
        for (i = currBuff, j = 0;   j < max;   j++, i = (i+1)%max) {
            if (buff[i] != NULL) {
                WriteStdOut(buff[i]);
                WriteNLStdOut();
            }
        }
        LnkFatal("trec sucess of failure");
    }
}

//---------------------------------------------------------------

static char Digit2Char(unsigned d) {
    if (d >= 10) {
        return d + 'A'-10;
    } else {
        return d + '0';
    }
}

static void OneLineDumpByte(unsigned char *p, int size) {
    int i;
    int len;
    enum { max = 80 };
    int ascii_offset = 60;
    char buf[max+1];
    len = FmtStr(buf, max, "%h ", p);
    for( i = 0; i < size; i++) {
        if (len >= max-4) {
            break;
        }
        if (i % 8 == 0) {
            buf[len++] = ' ';
        }
        buf[len++] = Digit2Char(p[i]/16);
        buf[len++] = Digit2Char(p[i]%16);
        buf[len++] = ' ';
    }

    if( len >= ascii_offset ) ascii_offset = len;
    if( max < ascii_offset ) ascii_offset = max;
    memset(buf+len, ' ', ascii_offset-len);
    len = ascii_offset;

    for( i = 0; i < size; i++) {
        if (len >= max-2) {
            break;
        }
        buf[len++] = isprint(p[i]) ? p[i] : '.';
    }
    buf[len++] = '\n';
    buf[len] = 0;
    WriteStdOut(buf);
}

static void OneLineDumpDWord(unsigned_32 *p, int byte_size) {
    int size = byte_size / sizeof p[0];
    enum { max = 80 };
    char buf[max+10];
    int len;
    int i;

    len = FmtStr(buf, max, "%h: ", p);
    for (i = 0; i < size; i++) {
        len += FmtStr(buf+len, max-len, " %h", p[i]);
        if (len > max) break;
    }
    buf[len++] = '\n';
    buf[len] = 0;
    WriteStdOut(buf);
}

void PrintMemDump(void *p, unsigned long size, DbgDumpType type) {
    unsigned char *buf = p;
    static struct {
        void (*OneLineDump)(void*, int);
        int max;
    } MemDump[DUMP_MAX] = {
        { (void(*)(void*,int))OneLineDumpByte,  16 },     // DUMP_BYTE
        { (void(*)(void*,int))OneLineDumpByte, 16 },     // DUMP_WORD: NYI
        { (void(*)(void*,int))OneLineDumpDWord, 4*4 }    // DUMP_DWORD
    };
    int max = MemDump[type].max;

    for(; size > max; size -= max) {
        MemDump[type].OneLineDump(buf, max);
        buf += max;
    }
    MemDump[type].OneLineDump(buf, size);
}

#endif
