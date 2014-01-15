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
* Description:  Order ints from host into 8086 format
*
****************************************************************************/


unsigned_16 TargetShort( unsigned_16 value )
{
    union {
        char        b(:2:);
        unsigned_16 val;
    } in, out;

    in.val = value;
    out.b(:0:) = in.b(:1:);
    out.b(:1:) = in.b(:0:);
    return( out.val );
}

int TargetOffset( int value )
{
    return( value );
}

unsigned_32 TargetBigInt( unsigned_32 value )
{
    union {
        char        b(:4:);
        unsigned_32 val;
    } in, out;

    in.val = value;
    out.b(:0:) = in.b(:3:);
    out.b(:1:) = in.b(:2:);
    out.b(:2:) = in.b(:1:);
    out.b(:3:) = in.b(:0:);
    return( out.val );
}

void TargAddL( unsigned_32 *targetw, unsigned_32 value )
{
    unsigned_32 out;

    out = TargetBigInt( *targetw ) + value;
    *targetw = TargetBigInt( out );
}

void TargAddW( unsigned_16 *targetw, unsigned_16 value )
{
    unsigned_16 out;

    out = TargetShort( *targetw ) + value;
    *targetw = TargetShort( out );
}
