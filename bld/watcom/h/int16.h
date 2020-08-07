/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _INT16_H_INCLUDED
#define _INT16_H_INCLUDED

#if defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( _M_I86 ) && defined( __OS2__ )


#define VECTOR_KEYB         0x16

#define _INT_16             0xcd VECTOR_KEYB

#define KEYB_STD            0
#define KEYB_EXT            0x10

extern unsigned short _BIOSKeyboardGet( unsigned char );
#pragma aux  _BIOSKeyboardGet = \
        _INT_16         \
    __parm              [__ah] \
    __value             [__ax] \
    __modify __exact    [__ax]

extern unsigned char _BIOSKeyboardHit( unsigned char );
#pragma aux _BIOSKeyboardHit = \
        "or     ah,1"   \
        _INT_16         \
        "jz short L1"   \
        "mov    al,1"   \
        "jmp short L2"  \
    "L1: xor    al,al"  \
    "L2:"               \
    __parm              [__ah] \
    __value             [__al] \
    __modify __exact    [__ax]

extern unsigned char _BIOSKeyboardTest( unsigned char );
#pragma aux _BIOSKeyboardTest =  \
        "or     ah,2"       \
        "mov    al,0ffh"    \
        _INT_16             \
    __parm              [__ah] \
    __value             [__al] \
    __modify __exact    [__ax]


#endif  /* defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( _M_I86 ) && defined( __OS2__ ) */

#endif  /* _INT16_H_INCLUDED */
