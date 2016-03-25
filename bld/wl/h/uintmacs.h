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


#define _GetU16( P )            (*((unsigned_16 *) (P) ))
#define _GetU32( P )            (*((unsigned_32 *) (P) ))
#define _TargU16toHost( X, Y )  Y = (unsigned_16) (X)
#define _HostU16toTarg( X, Y )  Y = (unsigned_16) (X)
#define _TargU32toHost( X, Y )  Y = (unsigned_32) (X)
#define _HostU32toTarg( X, Y )  Y = (unsigned_32) (X)

#define GET_U8(  P )            (*(unsigned_8  *)(P))
#define GET_U16( P )            (*(unsigned_16 *)(P))
#define GET_U32( P )            (*(unsigned_32 *)(P))

#define PUT_U8(  P, V )         (GET_U8(  P ) = V )
#define PUT_U16( P, V )         (GET_U16( P ) = V )
#define PUT_U32( P, V )         (GET_U32( P ) = V )

#define GET_I8(  P )            (*(signed_8  *)(P))

#define _GetU16UN( P )          (*((unsigned_16 _WCUNALIGNED *) (P) ))
#define _GetU32UN( P )          (*((unsigned_32 _WCUNALIGNED *) (P) ))

#define GET_U8_UN(  P )         (*(unsigned_8  _WCUNALIGNED *)(P))
#define GET_U16_UN( P )         (*(unsigned_16 _WCUNALIGNED *)(P))
#define GET_U32_UN( P )         (*(unsigned_32 _WCUNALIGNED *)(P))

#define PUT_U8_UN(  P, V )      (GET_U8_UN(  P ) = V )
#define PUT_U16_UN( P, V )      (GET_U16_UN( P ) = V )
#define PUT_U32_UN( P, V )      (GET_U32_UN( P ) = V )

#define _ReadBigEndian32UN( P )     GET_BE_32( *(unsigned_32 _WCUNALIGNED *)(P) )
#define _ReadLittleEndian32UN( P )  GET_LE_32( *(unsigned_32 _WCUNALIGNED *)(P) )
#define _ReadLittleEndian16UN( P )  GET_LE_16( *(unsigned_16 _WCUNALIGNED *)(P) )
