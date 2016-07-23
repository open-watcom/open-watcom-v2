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
* Description:  POSIX ps utility.
*
****************************************************************************/


#include <stdio.h>
#if defined(__NT__)
void main( void ) { printf( "Not implemented for NT\n" ); }
#elif defined(__OS2__)
void main( void ) { printf( "Not implemented for OS/2\n" ); }
#elif defined(__QNX__)
void main( void ) { printf( "Not implemented for QNX\n" ); }
#else
#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <process.h>

typedef unsigned short segment_t;
typedef unsigned short segsize_t;

struct memblk {
    char        memtag;
    segment_t   owner;
    segsize_t   size;
};

enum {
    DA_STDIN            = 0x0001,
    DA_STDOUT           = 0x0002,
    DA_NUL              = 0x0004,
    DA_CLOCK            = 0x0008,
    DA_FAST_CON         = 0x0010,
    /* reserved         = 0x0020, */
    DA_GENERIC_IOCTL    = 0x0040,
    /* reserved         = 0x0080, */
    /* reserved         = 0x0100, */
    /* reserved         = 0x0200, */
    /* reserved         = 0x0400, */
    DA_OPEN_CLOSE       = 0x0800,
    /* reserved         = 0x1000, */
    DA_NON_IBM_B        = 0x2000, /* block device */
    DA_OUTPUT_BUSY      = 0x2000, /* char device */
    DA_RW_IOCTL         = 0x4000,
    DA_CHAR_DEVICE      = 0x8000
};

struct device {
    struct device __far *next;
    unsigned short      attr;
    unsigned short      strategy_entry;
    unsigned short      interrupt_entry;
    char                name[8];
};

void __far *__first_pid( void );
#ifdef _M_I86
#pragma aux __first_pid = \
        /* 0xb4 0x52    */ "mov ah,52h" \
        /* 0xcd 0x21    */ "int 21h"  \
                           "mov ax,es" \
        value [ax bx];
#else
#pragma aux __first_pid = \
                           "xor ebx,ebx" \
        /* 0xb4 0x52    */ "mov ah,52h" \
        /* 0xcd 0x21    */ "int 21h"  \
                           "mov ax,es" \
        value [ax ebx];
#endif

segment_t __far *envp;

static void printCommand( unsigned seg )
{
    segment_t __far     *env_seg;
    unsigned char __far *p;

    if( _osmajor < 3 ) {
        putchar( '\n' );
        return;
    }
    p = MK_FP( seg, 0 );
    if( p[0] != 0xcd || p[1] != 0x20 ) {
        putchar( '\n' );
        return;
    }
    env_seg = MK_FP( seg, 0x2c );
    if( *env_seg == 0 ) {
        putchar( '\n' );
        return;
    }
    if( *env_seg == *envp && FP_SEG( envp ) != seg ) {
        putchar( '\n' );
        return;
    }
    p = MK_FP( *env_seg, 0 );
    while( *p ) {
        while( *p ) {
            ++p;
        }
        ++p;
    }
    if( p[1] != 1 || p[2] != 0 ) {
        putchar( '\n' );
        return;
    }
    p += 3;
    while( *p ) {
        putchar( *p );
        ++p;
    }
    putchar( '\n' );
}

static void do_ps( void )
{
    struct memblk __far *p;
    segment_t __far *pid;
    segment_t seg;

    envp = MK_FP( _psp, 0x2c );
    printf( "PID  PPID SIZE\n", *envp );
    pid = __first_pid();
    seg = *--pid;
    for(;;) {
        p = MK_FP( seg, 0 );
        if( p->owner == 0 ) {
            printf( "%4.4X FREE %4.4X  %lu bytes\n", seg+1,p->size,p->size*16L);
        } else {
            printf( "%4.4X %4.4X %4.4X  ", seg+1, p->owner, p->size );
            printCommand( p->owner );
        }
        if( p->memtag == 'Z' ) break;
        seg += p->size + 1;
    }
}

void main( void )
{
    do_ps();
}
#endif
