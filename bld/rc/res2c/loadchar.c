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
// This file contains functions that return a character from buffered
// input, but use the posx style read routines;
//
//  Modified    By              Reason
//  93/01/27    Alex Brodsky    Created
//  93/03/10    Alex Brodsky    add PeekFirstChar()
//                              fixed small bug


#include <io.h>
#include "loadchar.h"

// =================================================================
// This function loads the first char in a file from buffer which is
// loded using the posix style reads.
// The parameters it takes are
//      char                    *buff           Poitner to the buffer to use;
//      long                    size            maximum buffer size;
//      int                     f_handle        file handle to use
//      struct read_file        *fdata          ptr to where this data is to be kept
// The function returns : int, the character ot -1 if an error occurs.

extern int GetFirstChar( char *buff, long size, int f_handle, struct read_file *fdata ) {
//===================================================================================

    fdata->buffer = buff;
    fdata->cur_pos = buff;
    fdata->file_handle = f_handle;
    fdata->size = read( fdata->file_handle, fdata->buffer, size );
    if ( fdata->size < 1 ) return( -1 );
    fdata->end_pos = fdata->buffer + fdata->size;
    return( *( fdata->cur_pos ) );
}


extern int PeekFirstChar( char *buff, long size, int f_handle, struct read_file *fdata ) {
//===================================================================================

    fdata->buffer = buff;
    fdata->cur_pos = buff - 1;
    fdata->file_handle = f_handle;
    fdata->size = read( fdata->file_handle, fdata->buffer, size );
    if ( fdata->size < 1 ) return( -1 );
    fdata->end_pos = fdata->buffer + fdata->size;
    return( *( fdata->cur_pos + 1 ) );
}


extern int GetNextChar( struct read_file *fdata ) {
//=================================================

    if ( ( fdata->cur_pos + 1 ) == fdata->end_pos ) {
        fdata->size = read( fdata->file_handle, fdata->buffer, fdata->size );
        if ( fdata->size < 1 ) return( -1 );
        fdata->end_pos = fdata->buffer + fdata->size;
        fdata->cur_pos = fdata->buffer;
        return( *( fdata->cur_pos ) );
    } else {
        ( fdata->cur_pos )++;
        return( *( fdata->cur_pos ) );
    }
}


extern int PeekNextChar( struct read_file *fdata ) {
//=================================================

    if ( ( fdata->cur_pos + 1 ) == fdata->end_pos ) {
        fdata->size = read( fdata->file_handle, fdata->buffer, fdata->size );
        if ( fdata->size < 1 ) return( -1 );
        fdata->end_pos = fdata->buffer + fdata->size;
        fdata->cur_pos = fdata->buffer - 1;
        return( *( fdata->cur_pos + 1 ) );
    } else {
        return( *( fdata->cur_pos + 1) );
    }
}

