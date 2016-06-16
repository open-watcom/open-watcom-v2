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


#include "plusplus.h"
#include <errno.h>
#include "memmgr.h"
#include "scan.h"
#include "escchars.h"

#include "clibext.h"

char *stpcpy_after(             // COPY STRING, UPDATE SOURCE POINTER
    char *tgt,                  // - target
    char const **src )          // - addr( source )
{
    const char *src_ptr;        // - source

    for( src_ptr = *src; (*tgt++ = *src_ptr++) != '\0'; )
        ;
    *src = src_ptr;
    return( tgt );
}

#ifndef __APPLE__
char *stpcpy(                   // CONCATENATE STRING AS STRING
    char *string,               // - target location
    const char *src_string )    // - source string
{
    return( stpcpy_after( string, &src_string ) - 1 );
}
#endif

char *stvcpy_after(             // COPY VECTOR, UPDATE SOURCE POINTER
    char *tgt,                  // - target
    char const **src,           // - addr( source )
    size_t vsize )              // - vector size
{
    char const *src_ptr;        // - source

    for( src_ptr = *src; vsize > 0; --vsize ) {
        *tgt++ = *src_ptr++;
    }
    *tgt++ = '\0';
    *src = src_ptr;
    return( tgt );
}


char *stvcpy(                   // CONCATENATE VECTOR AS A STRING
    char *string,               // - target location
    const char *vector,         // - source vector to be copied
    size_t vsize )              // - size of source vector
{
    return( stvcpy_after( string, &vector, vsize ) - 1 );
}


char *stvcpyr(                  // CONCATENATE VECTOR AS A STRING REVERSE
    char *string,               // - target location
    const char *vector,         // - source vector to be copied reversed
    size_t vsize )              // - size of source vector
{
    const char *src_ptr;        // - source

    for( src_ptr = vector + vsize - 1; vsize > 0; --vsize ) {
        *string++ = *src_ptr--;
    }
    *string++ = '\0';
    return( string );
}


char *vctsave(                  // ALLOCATE AND SAVE VECTOR AS STRING
    const char *vector,         // - source vector
    size_t vsize )              // - size of source vector
{
    char * new_str;             // - target string

    new_str = (char *) CMemAlloc( vsize + 1 );
    stvcpy( new_str, vector, vsize );
    return( new_str );
}


char *strsave(                  // ALLOCATE AND SAVE A STRING
    const char *str )           // - source string
{
    return( vctsave( str, strlen( str ) ) );
}

char *strpermsave(              // ALLOCATE AND SAVE A STRING IN PERMANENT STORAGE
    const char *str )           // - source string
{
    size_t len;
    char *save;

    len = strlen( str );
    save = CPermAlloc( len + 1 );
    return( memcpy( save, str, len + 1 ) );
}


char *strend(                   // POINT AT END OF STRING
    char *string )              // - string
{
    return( string + strlen( string ) );
}

double SafeAtof( char *p )
{
    double r;

    errno = 0;
    r = atof( p );
    if( errno ) {
        if( r == 0 && errno == ERANGE ) {
            CErr1( ERR_FLOATING_CONSTANT_UNDERFLOW );
        } else {
            CErr1( ERR_FLOATING_CONSTANT_OVERFLOW );
        }
    }
    return( r );
}


int hex_dig(                    // GET HEXADECIMAL DIGIT FOR CHAR (OR 16)
    int c )                     // - character
{
    if(( CharSet[ c ] & (C_HX|C_DI) ) == 0 ) {
        return( 16 );
    }
    if( CharSet[ c ] & C_HX ) {
        /* a-f or A-F */
        c = (( c | HEX_MASK ) - HEX_BASE ) + 10 + '0';
    }
    return( c - '0' );
}


int octal_dig(                 // GET OCTAL DIGIT FOR CHAR (OR 8)
    int chr )                  // - character
{
    int     retn;              // - digit returned

    if( ( chr >= '0' ) && ( chr <= '7' ) ) {
        retn = chr - '0';
    } else {
        retn = 8;
    }
    return( retn );
}


int escape_char(               // GET ESCAPE CHAR FOR A LETTER
    int chr )                  // - character after "\"
{
    switch( chr ) {
      case 'a' :
        chr = ESCAPE_a;
        break;
      case 'b' :
        chr = ESCAPE_b;
        break;
      case 'f' :
        chr = ESCAPE_f;
        break;
      case 'n' :
        chr = ESCAPE_n;
        break;
      case 'r' :
        chr = ESCAPE_r;
        break;
      case 't' :
        chr = ESCAPE_t;
        break;
      case 'v' :
        chr = ESCAPE_v;
        break;
      case '\\' :
      case '\'' :
      case '\"' :
      case '?'  :
        break;
      default :
        chr = ESCAPE_NONE;
        break;
    }
    return( chr );
}


int classify_escape_char(       // CLASSIFY TYPE OF ESCAPE
    int chr )                   // - character after "\"
{
    int     retn;               // - classification returned

    if( octal_dig( chr ) != 8 ) {
        retn = ESCAPE_OCTAL;
    } else if( chr == 'x' ) {
        retn = ESCAPE_HEX;
    } else {
        retn = escape_char( chr );
    }
    return( retn );
}


char *stdcpy(                   // CONCATENATE DECIMAL NUMBER
    char *tgt,                  // - target location
    unsigned int value )        // - value to be concatenated
{
    char buffer[16];

    return stpcpy( tgt, ultoa( value, buffer, 10 ) );
}


char *sticpy(                   // CONCATENATE INTEGER NUMBER
    char *tgt,                  // - target location
    signed int value )          // - value to be concatenated
{
    char buffer[16];

    return stpcpy( tgt, ltoa( value, buffer, 10 ) );
}


char *sti64cpy(                 // CONCATENATE I64 NUMBER
    char *tgt,                  // - target location
    signed_64 value )           // - value to be concatenated
{
    sprintf( tgt, "%lld", VAL64( value ) );
    return strend( tgt );
}

bool strpref(                   // IS STRING A PREFIX OF A STRING
    char const *prefix,         // - possible prefix
    char const *str )           // - string
{
    while( *prefix ) {
        if( *prefix != *str ) {
            return( false );
        }
        ++prefix;
        ++str;
    }
    return( true );
}
