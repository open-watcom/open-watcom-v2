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
* Description:  POSIX dev utility.
*
****************************************************************************/


#include <stdio.h>
#if defined(__OS_nt386__) || defined(__OS_ntaxp__)
void main( void ) { printf( "Not implemented for NT\n" ); }
#elif defined(__OS_os2__) || defined(__OS_os2386__)
void main( void ) { printf( "Not implemented for OS/2\n" ); }
#elif defined(__OS_qnx__) || defined(__OS_qnx16__)
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
    struct device far   *next;
    unsigned short      attr;
    unsigned short      strategy_entry;
    unsigned short      interrupt_entry;
    char                name[8];
};

void far *__first_pid( void );
#ifdef __386__
#pragma aux __first_pid = \
                           "xor ebx,ebx" \
        /* 0xb4 0x52    */ "mov ah,52h" \
        /* 0xcd 0x21    */ "int 21h"  \
                           "mov ax,es" \
        value [ax ebx];
#else
#pragma aux __first_pid = \
        /* 0xb4 0x52    */ "mov ah,52h" \
        /* 0xcd 0x21    */ "int 21h"  \
                           "mov ax,es" \
        value [ax bx];
#endif

static void do_dev( void )
{
    void far *first_pid;
    struct device far *device_chain;
    unsigned blocks;
    int i;

    blocks = 0;
    first_pid = __first_pid();
    device_chain = MK_FP( FP_SEG( first_pid ), FP_OFF( first_pid ) + 0x22 );
    while( FP_OFF( device_chain ) != 0xffff ) {
        if(( device_chain->attr & DA_CHAR_DEVICE ) == 0 ) {
            blocks += device_chain->name[ 0 ];
        }
        device_chain = device_chain->next;
    }
    device_chain = MK_FP( FP_SEG( first_pid ), FP_OFF( first_pid ) + 0x22 );
    while( FP_OFF( device_chain ) != 0xffff ) {
        printf( "%04x:%04x ", FP_SEG( device_chain ), FP_OFF( device_chain ) );
        if( device_chain->attr & DA_CHAR_DEVICE ) {
            printf( "CHAR  " );
            for( i = 0; i < 8; ++i ) {
                putchar( device_chain->name[ i ] );
            }
        } else {
            printf( "BLOCK " );
            i = device_chain->name[0];
            if( i == 1 ) {
                printf( "(%c:)", 'A' + --blocks );
            } else {
                printf( "(%c:-", 'A' + ( blocks - i ) );
                printf( "%c:)", 'A' + ( blocks - 1 ) );
                blocks -= i;
            }
        }
        putchar( '\n' );
        device_chain = device_chain->next;
    }
}

void main( void )
{
    do_dev();
}
#endif
