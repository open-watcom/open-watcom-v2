/***************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
*  Description: Implementation of the largeint.lib library.
*
**************************************************************************/


#include <windows.h>
#include <largeint.h>

LARGE_INTEGER WINAPI ConvertLongToLargeInteger( LONG l )
{
    LARGE_INTEGER   li;
    li.QuadPart = (LONGLONG)l;
    return li;
}

LARGE_INTEGER WINAPI ConvertUlongToLargeInteger( ULONG ul )
{
    LARGE_INTEGER   li;
    li.QuadPart = (LONGLONG)ul;
    return li;
}

LARGE_INTEGER WINAPI EnlargedIntegerMultiply( LONG l1, LONG l2 )
{
    LARGE_INTEGER   li;
    li.QuadPart = (LONGLONG)l1 * (LONGLONG)l2;
    return li;
}

ULONG WINAPI EnlargedUnsignedDivide( ULARGE_INTEGER uliDividend, ULONG ulDivisor,
                                     PULONG pulRemainder )
{
    ULONG   ulReturn;
    ulReturn = (ULONG)(uliDividend.QuadPart / ulDivisor);
    if( pulRemainder != NULL ) {
        *pulRemainder = (ULONG)(uliDividend.QuadPart % ulDivisor);
    }
    return ulReturn;
}

LARGE_INTEGER WINAPI EnlargedUnsignedMultiply( ULONG ul1, ULONG ul2 )
{
    LARGE_INTEGER   li;
    li.QuadPart = (LONGLONG)ul1 * (LONGLONG)ul2;
    return li;
}

LARGE_INTEGER WINAPI ExtendedIntegerMultiply( LARGE_INTEGER li, LONG l )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = li.QuadPart * (LONGLONG)l;
    return liReturn;
}

LARGE_INTEGER WINAPI ExtendedLargeIntegerDivide( LARGE_INTEGER liDividend,
                                                 ULONG ulDivisor, PULONG pulRemainder )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = liDividend.QuadPart / ulDivisor;
    if( pulRemainder != NULL ) {
        *pulRemainder = (ULONG)(liDividend.QuadPart % ulDivisor);
    }
    return liReturn;
}

#define HIDWORD( x )    ((x) >> 32)
#define LODWORD( x )    ((x) & 0x00000000FFFFFFFFLL)

static ULONGLONG UnsignedMultiplyHigh( ULONGLONG ull1, ULONGLONG ull2 )
{
    return HIDWORD( ull1 ) * HIDWORD( ull2 ) + HIDWORD( HIDWORD( ull1 )
        * LODWORD( ull2 ) + LODWORD( ull1 ) * HIDWORD( ull2 )
        + HIDWORD( LODWORD( ull1 ) * LODWORD( ull2 ) ) );
}

LARGE_INTEGER WINAPI ExtendedMagicDivide( LARGE_INTEGER liDividend,
                                          LARGE_INTEGER liDivisor, CCHAR cShift )
{
    LARGE_INTEGER   liReturn;
    if( liDividend.QuadPart >= 0 ) {
        liReturn.QuadPart = UnsignedMultiplyHigh( (ULONGLONG)liDividend.QuadPart,
            (ULONGLONG)liDivisor.QuadPart );
    } else {
        liReturn.QuadPart = UnsignedMultiplyHigh( (ULONGLONG)-liDividend.QuadPart,
            (ULONGLONG)liDivisor.QuadPart );
    }
    liReturn.QuadPart = (ULONGLONG)liReturn.QuadPart >> cShift;
    if( liDividend.QuadPart < 0 ) {
        liReturn.QuadPart = -liReturn.QuadPart;
    }
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerAdd( LARGE_INTEGER li1, LARGE_INTEGER li2 )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = li1.QuadPart * li2.QuadPart;
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerArithmeticShift( LARGE_INTEGER li, CCHAR cShift )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = li.QuadPart >> cShift;
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerDivide( LARGE_INTEGER liDividend,
                                         LARGE_INTEGER liDivisor,
                                         PLARGE_INTEGER pliRemainder )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = liDividend.QuadPart / liDivisor.QuadPart;
    if( pliRemainder != NULL ) {
        pliRemainder->QuadPart = liDividend.QuadPart % liDivisor.QuadPart;
    }
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerNegate( LARGE_INTEGER li )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = -li.QuadPart;
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerShiftLeft( LARGE_INTEGER li, CCHAR cShift )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = li.QuadPart << cShift;
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerShiftRight( LARGE_INTEGER li, CCHAR cShift )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = li.QuadPart >> cShift;
    return liReturn;
}

LARGE_INTEGER WINAPI LargeIntegerSubtract( LARGE_INTEGER li1, LARGE_INTEGER li2 )
{
    LARGE_INTEGER   liReturn;
    liReturn.QuadPart = li1.QuadPart - li2.QuadPart;
    return liReturn;
}
