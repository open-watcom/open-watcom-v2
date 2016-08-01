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
* Description:  Generic utility functions for cg.
*
****************************************************************************/


#include "cgstd.h"
#include "utils.h"

#if defined( _M_IX86 ) && defined(__WATCOMC__)

#include "cypfunc.h"

void    *Copy( const void *x, void *y, uint len )
/***********************************************/
{
    return( CypCopy( x, y, len ) );
}

void    *Fill( void *start, uint len, byte filler )
/*************************************************/
{
    return( CpyFill( start, len, filler  ) );
}

bool    Equal( const void *src, const void *dst, uint length )
/************************************************************/
{
    return( CypEqual( src, dst, length ) );
}

char    *CopyStr( const char *src, char *dst )
/********************************************/
{
    return( (char *)CypCopy( src, dst, CypLength( src ) + 1 ) - 1 );
}

uint    Length( const char *string )
/**********************************/
{
    return( CypLength( string ) );
}

#else

void    *Copy( const void *x, void *y, uint len )
/***********************************************/
{
    return( memcpy( y, x, len ) );
}

void    *Fill( void *start, uint len, byte filler )
/*************************************************/
{
    return( memset( start, filler, len ) );
}

bool    Equal( const void *src, const void *dst, uint length )
/************************************************************/
{
    return( memcmp( src, dst, length ) == 0 );
}

char    *CopyStr( const char *src, char *dst )
/********************************************/
{
    strcpy( dst, src );
    return( dst + strlen( dst ) );
}

uint    Length( const char *string )
/**********************************/
{
    return( strlen( string ) );
}

#endif

uint_32 CountBits( uint_32 value )
/********************************/
{
    uint_32     temp;

    value = value - ((value >> 1) & 0x55555555);
    temp  = ((value >> 2) & 0x33333333);
    value = (value & 0x33333333) + temp;
    value = (value + (value >> 4)) & 0x0F0F0F0F;
    value = value + (value << 8);
    value = value + (value << 16);
    return( value >> 24 );
}
