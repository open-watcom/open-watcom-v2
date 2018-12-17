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


extern unsigned_8       In_b( unsigned_16 port );
extern unsigned_16      In_w( unsigned_16 port );
extern unsigned_32      In_d( unsigned_16 port );
extern void             Out_b( unsigned_16 port, unsigned_8  value );
extern void             Out_w( unsigned_16 port, unsigned_16 value );
extern void             Out_d( unsigned_16 port, unsigned_32 value );

#pragma aux In_b =      \
        "in     al,dx"  \
    __parm __routine [__dx] __value [__al]

#pragma aux Out_b =     \
        "out    dx,al"  \
    __parm __routine [__dx] [__al]


#pragma aux In_w =      \
        "in     ax,dx"  \
    __parm __routine [__dx] __value [__ax]

#pragma aux Out_w =     \
        "out    dx,ax"  \
    __parm __routine [__dx] [__ax]


#if defined(__386__)

#pragma aux In_d =      \
        "in     eax,dx" \
    __parm __routine [__dx] __value [__eax]

#pragma aux Out_d =     \
        "out    dx,eax" \
    __parm __routine [__dx] [__eax]

#else

#pragma aux In_d =      \
        ".386p"         \
        "in     eax,dx" \
        "mov    edx,eax"\
        "shr    edx,16" \
    __parm __routine [__dx] __value [__dx __ax]

#pragma aux Out_d =     \
        ".386p"         \
        "ror    eax,16" \
        "mov    ax,cx"  \
        "ror    eax,16" \
        "out    dx,eax" \
    __parm __routine [__dx] [__cx __ax]

#endif
