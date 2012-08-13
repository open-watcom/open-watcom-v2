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


//
// This file contains functions that return a type from buffered
// input, but use the posx style read routines;
//
//  Modified    By              Reason
//  93/03/10    Alex Brodsky    Created

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "loadchar.h"

#define BUFF_INC        40

extern unsigned short GetNextShort( read_file *ch_info ) {

    char                buffer[2];
    unsigned short      *sptr;
    int                 ch;

    ch = GetNextChar( ch_info );
    if ( ch == -1 ) {
        puts("File read error occured");
        close( ch_info->file_handle );
        abort();
    }
    buffer[0] = ch;
    ch = GetNextChar( ch_info );
    if ( ch == -1 ) {
        puts("File read error occured");
        close( ch_info->file_handle );
        abort();
    }
    buffer[1] = ch;

    sptr = (unsigned short *)buffer;
    return( *sptr );
}


extern unsigned long GetNextLong( read_file *ch_info ) {

    char                buffer[4];
    unsigned long       *sptr;
    int                 ch;

    ch = GetNextChar( ch_info );
    if ( ch == -1 ) {
        puts("File read error occured");
        close( ch_info->file_handle );
        abort();
    }
    buffer[0] = ch;
    ch = GetNextChar( ch_info );
    if ( ch == -1 ) {
        puts("File read error occured");
        close( ch_info->file_handle );
        abort();
    }
    buffer[1] = ch;
    ch = GetNextChar( ch_info );
    if ( ch == -1 ) {
        puts("File read error occured");
        close( ch_info->file_handle );
        abort();
    }
    buffer[2] = ch;
    ch = GetNextChar( ch_info );
    if ( ch == -1 ) {
        puts("File read error occured");
        close( ch_info->file_handle );
        abort();
    }
    buffer[3] = ch;

    sptr = (unsigned long *)buffer;
    return( *sptr );
}

extern char *GetNextString( read_file *ch_info ) {

    char                *buffer;
    int                 x = 0;
    int                 current_max = BUFF_INC;
    int                 ch;

    buffer = malloc( BUFF_INC );

    do {
        if ( x >= current_max ) {
            current_max += BUFF_INC;
            buffer = realloc( buffer, current_max );
            if ( !buffer ) {
                puts("Out of memory");
                close( ch_info->file_handle );
                abort();
            }
        }
        ch = GetNextChar( ch_info );
        if ( ch == -1 ) {
            puts("File read error occured");
            close( ch_info->file_handle );
            free( buffer );
            abort();
        }
        buffer[ x ] = ch;
        x++;
    } while ( ch );
    return( buffer );
}
