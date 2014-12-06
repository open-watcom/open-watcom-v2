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


#include <string.h>
#include "dbgio.h"

static unsigned long FilePos;

typedef struct {
    char        *start;
    char        *end;
    char        *ptr;
    unsigned    size;
} buffer;

typedef enum { EXACT, MATCH, LOW, HIGH } status;


static char GetLine( unsigned handle, buffer *buff, unsigned str_len )
{
    unsigned    keep;
    char        old;

    ++str_len;
    for( ;; ) {
        if( buff->ptr + str_len > buff->end ) {
            keep = buff->end - buff->ptr;
            memcpy( buff->start, buff->ptr, keep );
            buff->end = ReadStream( handle, buff->start+keep, buff->size-keep )
                                + buff->start + keep;
            if( buff->start + str_len >= buff->end ) return( 0 );
            buff->ptr = buff->start;
        }
        old = *buff->ptr;
        buff->ptr += 1;
        ++FilePos;
        if( old == '\n' ) return( 1 );
    }
}


static unsigned Match( char *str, char *ptr )
{
    char        *start;

    start = str;
    while( *str == *ptr ) {
        ++str;
        ++ptr;
    }
    return( str - start );
}


static status FindTag( unsigned handle, buffer *buff, char *str,
                       unsigned str_len, unsigned prefix_len )
{
    unsigned    diff;

    for( ;; ) {
        if( !GetLine( handle, buff, str_len ) ) return( HIGH );
        diff = Match( str, buff->ptr );
        if( diff >= prefix_len ) break;
    }
    if( diff == str_len ) return( (buff->ptr[diff] <= ' ') ? EXACT : MATCH );
    return( (buff->ptr[diff] > str[diff]) ? HIGH : LOW );
}


int FSearch( unsigned handle, char *str, char *buff_start,
             unsigned buff_size, unsigned prefix_len )
{
    unsigned long       size;
    unsigned long       offset;
    unsigned long       match_pos;
    unsigned            str_len;
    status              stat;
    buffer              buff;


    buff.size  = buff_size;
    buff.start = buff_start;
    str_len = strlen( str );
    match_pos = -1UL;
    offset = 0UL;
    for( size = SeekStream( handle, 0L, DIO_SEEK_END ) >> 1; size > 0; size >>= 1 ) {
        FilePos = offset + size;
        SeekStream( handle, FilePos, DIO_SEEK_ORG );
        buff.end = buff.start;
        buff.ptr = buff.end;
        stat = FindTag( handle, &buff, str, str_len, prefix_len );
        if( stat == EXACT ) {
            match_pos = FilePos;
            break; /* from for loop */
        } else if( stat == MATCH ) {
            match_pos = FilePos;
        } else if( stat == LOW ) {
            offset += size;
        }
    }
    if( match_pos == -1UL ) return( 0 );
    SeekStream( handle, match_pos, DIO_SEEK_ORG );
    return( 1 );
}
