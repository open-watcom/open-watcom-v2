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


#include "standard.h"

#if defined(__386__) || defined(M_I86)

#include "cypfunc.h"

extern  byte    *Copy( byte *x, byte *y, uint len ) {
/***************************************************/

    return(  CypCopy( x, y, len ) );
}

extern  bool    Equal( char *src, char *dst, int length ) {
/*********************************************************/

    return( CypEqual( src, dst, length ) );
}

extern  char    *CopyStr( char *src, char *dst ) {
/************************************************/

    return( CypCopy( src, dst, CypLength( src ) + 1 ) - 1 );
}

extern  uint    Length( char *string ) {
/**************************************/

    return( CypLength( string ) );
}

#else

#include <string.h>

extern  byte    *Copy( byte *x, byte *y, uint len ) {
/***************************************************/

    return( memcpy( y, x, len ) );
}

extern  char    *Fill( char *start, uint len, byte filler ) {
/***********************************************************/

    return( memset( start,  filler, len  ) );
}

extern  bool    Equal( char *src, char *dst, int length ) {
/*********************************************************/

    return( memcmp( src, dst, length ) == 0 );
}

extern  char    *CopyStr( char *src, char *dst ) {
/************************************************/

    strcpy( dst, src );
    return( dst + strlen( dst ) );
}

extern  uint    Length( char *string ) {
/**************************************/

    return( strlen( string ) );
}

#endif

extern  uint_32 CountBits( uint_32 value ) {
/******************************************/

    uint_32             r, l;

    r = ( value      ) & 0x55555555;
    l = ( value >> 1 ) & 0x55555555;
    value = r + l;
    r = ( value      ) & 0x33333333;
    l = ( value >> 2 ) & 0x33333333;
    value = r + l;
    r = ( value      ) & 0x0f0f0f0f;
    l = ( value >> 4 ) & 0x0f0f0f0f;
    value = r + l;
    r = ( value      ) & 0x00ff00ff;
    l = ( value >> 8 ) & 0x00ff00ff;
    value = r + l;
    r = ( value       ) & 0x0000ffff;
    l = ( value >> 16 ) & 0x0000ffff;
    value = r + l;
    return( value );
}

