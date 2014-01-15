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


#if defined( __BIG_ENDIAN__ )

extern unsigned_16      TargetShort(unsigned_16);
extern unsigned_32      TargetBigInt(unsigned_32);
extern void             TargAddL(unsigned_32 *,unsigned_32);
extern void             TargAddW(unsigned_16 *,unsigned_16);

#define _IntToByte( value )             ( ( value ) & 0xff )
#define _IntToWord( value )             ( value & 0xffff )
#define _TargetShort( value )           TargetShort( value )
#define _TargetBigInt( value )          TargetBigInt( value )
#define _TargetOffset( value )          TargetOffset( value )
#define _TargetLongInt( value )         TargetBigInt( value )
#define _HostOffset( value )            TargetOffset( value )
#define _HostShort( value )             TargetShort( value )
#define _TargetAddL( targetw, value )   TargAddL( &targetw, value )
#define _TargetAddW( targetw, value )   TargAddW( &targetw, value )

#else

#define _IntToByte( value )             ( value & 0xff )
#define _IntToWord( value )             ( value & 0xffff )
#define _TargetShort( value )           ( value )
#define _TargetBigInt( value )          ( value )
#define _TargetOffset( value )          ( value )
#define _TargetLongInt( value )         ( value )
#define _HostOffset( value )            ( value )
#define _HostShort( value )             ( value )
#define _TargetAddL( targetw, value )   targetw += value
#define _TargetAddW( targetw, value )   targetw += value

#endif
