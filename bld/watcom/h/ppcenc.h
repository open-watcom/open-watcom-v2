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


#define _SixBits( x )   ( (x) & 0x003f )
#define _SevenBits( x ) ( (x) & 0x007f )
#define _FiveBits( x )  ( (x) & 0x001f )
#define _EightBits( x ) ( (x) & 0x00ff )
#define _TenBits( x )   ( (x) & 0x03ff )
#define _OneBit( x )    ( (x) & 0x0001 )

// these correspond to letters in section 3.3 of the Alpha Architecture Manual
#define _A( x )         ( _FiveBits(x) << 16 )
#define _B( x )         ( _FiveBits(x) << 11 )
#define _C( x )         ( _FiveBits(x) <<  6 )
#define _S( x )         ( _FiveBits(x) << 21 )
#define _D( x )         ( _FiveBits(x) << 21 )

#define _Op1( x )       ( _SixBits(x) << 26 )
#define _Op2( x )       ( _TenBits(x) << 1 )

// Note: this assumes SPR[5-9] is 0 (so we can put simple decimal value as parm)
#define _SPR( x )       ( _TenBits( x ) << 16 )

#define _AA( x )        ( _OneBit( x ) << 1 )
#define _LK( x )        ( _OneBit( x ) )

// sixteen bit signed immediate
#define _SignedImmed( x )       ( (x) & 0xffffU )

// twenty-four bit signed immediate (note - this effectively divides by 4)
#define _BranchImmed( x )       ( (x) & 0x3fffffc )

// SPR registers (page 10-131)
#define SPR_LR          8
#define SPR_CTR         9

typedef uint_32                 ppc_ins;
