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


extern void BIOSSetPage( unsigned char pagenb );
#pragma aux BIOSSetPage =   \
        CALL_INT10( 5 )     \
    parm [al] modify exact [ah]

extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage =   \
        CALL_INT10( 0x0f )  \
    value [bh] modify exact [ax bh]

extern void BIOSSetMode( unsigned char mode );
#pragma aux BIOSSetMode =   \
        CALL_INT10( 0 )     \
    parm [al] modify exact [ax]

extern unsigned short BIOSGetCurPos( unsigned char pagenb );
#pragma aux BIOSGetCurPos = \
        CALL_INT10( 3 )     \
    parm [bh] value [dx] modify exact [ax cx dx]

extern void BIOSSetCurPos( unsigned short rowcol, unsigned char pagenb );
#pragma aux BIOSSetCurPos = \
        CALL_INT10( 2 )     \
    parm [dx] [bh] modify exact [ah]

extern unsigned short BIOSGetCurTyp( unsigned char pagenb );
#pragma aux BIOSGetCurTyp = \
        CALL_INT10( 3 )     \
    parm [bh] value [cx] modify exact [ax cx dx]

extern void BIOSSetCurTyp( unsigned short startend );
#pragma aux BIOSSetCurTyp = \
        CALL_INT10( 1 )     \
    parm [cx] modify exact [ah]

extern unsigned char BIOSGetAttr( unsigned char pagenb );
#pragma aux BIOSGetAttr =   \
        CALL_INT10( 8 )     \
    parm [bh] value [ah] modify exact [ax]

extern void BIOSSetAttr( unsigned char attr );
#pragma aux BIOSSetAttr =   \
        "xor  cx,cx"        \
        "mov  dx,3250h"     \
        "xor  al,al"        \
        CALL_INT10( 6 )     \
    parm [bh] modify exact [ax cx dx]

extern unsigned char BIOSGetRows( void );
#pragma aux BIOSGetRows =   \
        "push es"           \
        "mov  al,30h"       \
        "xor  bh,bh"        \
        CALL_INT10( 0x11 )  \
        "inc  dl"           \
        "pop  es"           \
/*      modify exact [ax bh cx dl] */   \
    value [dl] modify exact [ax ebx ecx edx edi] /* workaround bug in DOS4G */

extern unsigned short BIOSGetPoints( void );
#pragma aux BIOSGetPoints = \
        "push es"           \
        "mov  al,30h"       \
        "xor  bh,bh"        \
        CALL_INT10( 0x11 )  \
        "pop  es"           \
/*      modify exact [ax bh cx dl] */   \
    value [cx] modify exact [ax ebx ecx edx edi] /* workaround bug in DOS4G */

extern void BIOSEGAChrSet( unsigned char vidroutine );
#pragma aux BIOSEGAChrSet = \
        "xor  bl,bl"        \
        CALL_INT10( 0x11 )  \
    parm [al] modify exact [ah bl]
