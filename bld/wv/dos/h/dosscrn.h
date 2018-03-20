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


#include "pcscrnio.h"


extern void        BIOSSetPage( unsigned char );
#pragma aux BIOSSetPage =       \
        CALL_INT10( 5 )         \
    parm [al] modify exact [ah];


extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage =       \
        CALL_INT10( 15 )        \
    value [bh] modify exact [ax bh];


extern void     BIOSSetMode( unsigned char );
#pragma aux BIOSSetMode =       \
        CALL_INT10( 0 )         \
    parm [al] modify exact [ax];


extern unsigned BIOSGetCurPos( unsigned char );
#pragma aux BIOSGetCurPos =     \
        CALL_INT10( 3 )         \
    parm [bh] value [dx] modify exact [ax cx dx];


extern void     BIOSSetCurPos( unsigned, unsigned char );
#pragma aux BIOSSetCurPos =     \
        CALL_INT10( 2 )         \
    parm [dx] [bh] modify [bx cx dx];


extern unsigned BIOSGetCurTyp( unsigned char );
#pragma aux BIOSGetCurTyp =     \
        CALL_INT10( 3 )         \
    parm [bh] value [cx] modify exact [ax cx dx];


extern void        BIOSSetCurTyp();
#pragma aux BIOSSetCurTyp =     \
        CALL_INT10( 1 )         \
    parm [cx] modify [cx];


extern unsigned char    BIOSGetAttr();
#pragma aux BIOSGetAttr =       \
        CALL_INT10( 8 )         \
    parm [bh] value [ah];


extern void        BIOSSetAttr( unsigned char );
#pragma aux BIOSSetAttr =       \
        "sub    cx,cx"          \
        "mov    dx,3250h"       \
        "xor    al,al"          \
        CALL_INT10( 6 )         \
    parm [bh];


extern void        VIDWait( void );
#pragma aux VIDWait = "jmp short L1" "L1:" \
    parm [] modify exact []


extern signed long BIOSEGAInfo();
#pragma aux BIOSEGAInfo =       \
        "mov    bx,0ff0ah"      \
        CALL_INT10( 18 )         \
        "mov    ax,bx"          \
        "mov    dx,cx"          \
    parm modify [bx cx];


extern unsigned char    BIOSGetRows();
#pragma aux BIOSGetRows =       \
        "push   es"             \
        "mov    al,30h"       \
        "xor    bh,bh"          \
        CALL_INT10( 17 )         \
        "inc    dl"             \
        "pop    es"             \
    parm value [dl] modify [ax bx cx];


extern unsigned    BIOSGetPoints();
#pragma aux BIOSGetPoints =     \
        "push   es"             \
        "mov    al,30h"       \
        "xor    bh,bh"          \
        CALL_INT10( 17 )         \
        "pop    es"             \
    parm value [cx] modify [ax bx dx];


extern void        BIOSEGAChrSet();
#pragma aux BIOSEGAChrSet =     \
        "xor    bl,bl"          \
        CALL_INT10( 17 )         \
    parm [al] modify [ax bx];


extern void        BIOSCharSet();
#pragma aux BIOSCharSet =       \
        "xchg   bp,si"          \
        "mov    ah,11h"         \
        "xor    bl,bl"          \
        "int    10h"            \
        "xchg   bp,si"          \
    parm [al] [bh] [cx] [dx] [es] [si] modify [ax bx cx dx];

/* get video save size */
extern unsigned _VidStateSize( unsigned );
#pragma aux     _VidStateSize = \
        "mov    ax,1c00h"       \
        "int    10h"            \
        "cmp    al,1ch"         \
        "je short L1"           \
        "xor    bx,bx"          \
    "L1:"                       \
    parm [cx] value [bx] modify exact [ax bx];

/* save video state */
extern void     _VidStateSave( unsigned, unsigned, unsigned );
#pragma aux     _VidStateSave = \
        "mov    ax,1c01h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax];

/* restore video state */
extern void     _VidStateRestore( unsigned, unsigned, unsigned );
#pragma aux     _VidStateRestore = \
        "mov    ax,1c02h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax];

