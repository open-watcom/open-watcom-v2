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
* Description:  declaration for character set bit array routines
*
****************************************************************************/


extern void __setbits( unsigned char *vector, const char *charset );
extern void __fsetbits( unsigned char _WCFAR *vector, const char _WCFAR *charset );

extern const unsigned char _HUGEDATA _Bits[8];

#define GETCHARBIT(vector,c) (vector[ (unsigned char)c >> 3 ] & _Bits[ c & 0x07 ])
#define SETCHARBIT(vector,c) vector[ (unsigned char)c >> 3 ] |= _Bits[ c & 0x07 ]

#define CHARVECTOR_SIZE (256/8)
