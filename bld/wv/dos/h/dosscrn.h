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
    parm [al] modify exact [ah]


extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage =       \
        CALL_INT10( 0x0f )      \
    value [bh] modify exact [ax bh]


extern void     BIOSSetMode( unsigned char );
#pragma aux BIOSSetMode =       \
        CALL_INT10( 0 )         \
    parm [al] modify exact [ax]


extern unsigned BIOSGetCurPos( unsigned char );
#pragma aux BIOSGetCurPos =     \
        CALL_INT10( 3 )         \
    parm [bh] value [dx] modify exact [ax cx dx]


extern void     BIOSSetCurPos( unsigned, unsigned char );
#pragma aux BIOSSetCurPos =     \
        CALL_INT10( 2 )         \
    parm [dx] [bh] modify exact [ah]


extern unsigned BIOSGetCurTyp( unsigned char );
#pragma aux BIOSGetCurTyp =     \
        CALL_INT10( 3 )         \
    parm [bh] value [cx] modify exact [ax cx dx]


extern void     BIOSSetCurTyp( unsigned );
#pragma aux BIOSSetCurTyp =     \
        CALL_INT10( 1 )         \
    parm [cx] modify exact [ah]


extern unsigned char    BIOSGetAttr( unsigned char );
#pragma aux BIOSGetAttr =       \
        CALL_INT10( 8 )         \
    parm [bh] value [ah] modify exact [ax]


extern void        BIOSSetAttr( unsigned char );
#pragma aux BIOSSetAttr =       \
        "sub    cx,cx"          \
        "mov    dx,3250h"       \
        "xor    al,al"          \
        CALL_INT10( 6 )         \
    parm [bh] modify exact [ax cx dx]


extern unsigned char    BIOSGetRows( void );
#pragma aux BIOSGetRows =       \
        "push   es"             \
        "mov    al,30h"         \
        "xor    bh,bh"          \
        CALL_INT10( 0x11 )      \
        "inc    dl"             \
        "pop    es"             \
    value [dl] modify exact [ax bh cx dl]


extern unsigned    BIOSGetPoints( void );
#pragma aux BIOSGetPoints =     \
        "push   es"             \
        "mov    al,30h"         \
        "xor    bh,bh"          \
        CALL_INT10( 0x11 )      \
        "pop    es"             \
    value [cx] modify exact [ax bh cx dl]


extern void        BIOSEGAChrSet( unsigned char );
#pragma aux BIOSEGAChrSet =     \
        "xor    bl,bl"          \
        CALL_INT10( 0x11 )      \
    parm [al] modify exact [ah bl]

extern void        BIOSCharSet( unsigned char, unsigned char, unsigned, unsigned, unsigned, unsigned  );
#pragma aux BIOSCharSet =       \
        "xchg   bp,si"          \
        "mov    ah,11h"         \
        "xor    bl,bl"          \
        "int    10h"            \
        "xchg   bp,si"          \
    parm [al] [bh] [cx] [dx] [es] [si] modify exact [ah bl]

/* get video save size */
extern unsigned _VidStateSize( unsigned );
#pragma aux     _VidStateSize = \
        "mov    ax,1c00h"       \
        "int    10h"            \
        "cmp    al,1ch"         \
        "je short L1"           \
        "xor    bx,bx"          \
    "L1:"                       \
    parm [cx] value [bx] modify exact [ax bx]

/* save video state */
extern void     _VidStateSave( unsigned, unsigned, unsigned );
#pragma aux     _VidStateSave = \
        "mov    ax,1c01h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax]

/* restore video state */
extern void     _VidStateRestore( unsigned, unsigned, unsigned );
#pragma aux     _VidStateRestore = \
        "mov    ax,1c02h"       \
        "int    10h"            \
    parm [cx] [es] [bx] modify exact [ax]

extern unsigned MouseStateSize( void );
#pragma aux MouseStateSize = \
        "sub    bx,bx"      \
        "mov    ax,15h"     \
        "int    33h"        \
    value [bx] modify exact [ax bx]


extern void MouseStateSave( unsigned, unsigned, unsigned );
#pragma aux MouseStateSave =    \
        "mov    ax,16h"         \
        "int    33h"            \
    parm [es] [dx] [bx] modify exact [ax]


extern void MouseStateRestore( unsigned, unsigned, unsigned );
#pragma aux MouseStateRestore = \
        "mov    ax,17h"         \
        "int    33h"            \
    parm [es] [dx] [bx] modify exact [ax]

#if 0
extern void vertsync( void );
#pragma aux vertsync =  \
        "mov dx,3dah"   \
    "L1: in al,dx"      \
        "test al,8"     \
        "jz short L1"   \
    modify exact [al dx]
#endif
