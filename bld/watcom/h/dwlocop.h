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


/*
    This file is used to define the DW_LOC_xx enumerated type as well as
    the DW internal private type OP_xx, and some other private structures.
*/
DW_LOC_OP( fbreg,       0x91 )
DW_LOC_OP( bregx,       0x92 )
DW_LOC_OP( dup,         0x12 )
DW_LOC_OP( drop,        0x13 )
DW_LOC_OP( over,        0x14 )
DW_LOC_OP( pick,        0x15 )
DW_LOC_OP( swap,        0x16 )
DW_LOC_OP( rot,         0x17 )
DW_LOC_OP( deref,       0x06 )
DW_LOC_OP( xderef,      0x18 )
DW_LOC_OP( abs,         0x19 )
DW_LOC_OP( and,         0x1a )
DW_LOC_OP( div,         0x1b )
DW_LOC_OP( minus,       0x1c )
DW_LOC_OP( mod,         0x1d )
DW_LOC_OP( mul,         0x1e )
DW_LOC_OP( neg,         0x1f )
DW_LOC_OP( not,         0x20 )
DW_LOC_OP( or,          0x21 )
DW_LOC_OP( plus,        0x22 )
DW_LOC_OP( plus_uconst, 0x23 )
DW_LOC_OP( shl,         0x24 )
DW_LOC_OP( shr,         0x25 )
DW_LOC_OP( shra,        0x26 )
DW_LOC_OP( xor,         0x27 )
DW_LOC_OP( bra,         0x28 )
DW_LOC_OP( eq,          0x29 )
DW_LOC_OP( ge,          0x2a )
DW_LOC_OP( gt,          0x2b )
DW_LOC_OP( le,          0x2c )
DW_LOC_OP( lt,          0x2d )
DW_LOC_OP( ne,          0x2e )
DW_LOC_OP( skip,        0x2f )
DW_LOC_OP( nop,         0x96 )
DW_LOC_OP( piece,       0x93 )
DW_LOC_OP( deref_size,  0x94 )
DW_LOC_OP( xderef_size, 0x95 )
