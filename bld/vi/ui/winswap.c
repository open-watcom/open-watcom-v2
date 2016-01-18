/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Swap window data to disk.
*
****************************************************************************/


#include "vi.h"
#include <fcntl.h>
#include "posix.h"
#include "win.h"

#include "clibext.h"

#define SWAP_FILE_NAME  "swXXXXXX"

static char     swapName[sizeof( SWAP_FILE_NAME )];
static int      swapHandle = -1;

/*
 * windowSwapFileOpen - do just that
 */
static vi_rc windowSwapFileOpen( void )
{
    char    file[_MAX_PATH];

    if( swapHandle == -1 ) {
        MakeTmpPath( file, SWAP_FILE_NAME );
        swapHandle = mkstemp( file );
        if( swapHandle == -1 ) {
            return( ERR_SWAP_FILE_OPEN );
        }
        memcpy( swapName, file + strlen( file ) - ( sizeof( SWAP_FILE_NAME ) - 1 ), sizeof( SWAP_FILE_NAME ) );
    }
    return( ERR_NO_ERR );

} /* windowSwapFileOpen */

/*
 * buffSize - compute the size of a i/o buffer for a window
 */
static long buffSize( void )
{
    long        tmp;

    tmp = (long)EditVars.WindMaxWidth * (long)EditVars.WindMaxHeight * 4L;
    tmp = tmp / 512;
    tmp++;
    tmp *= 512;
    return( tmp );

} /* buffSize */

/*
 * windowSwap - swap a windows data
 */
void static windowSwap( window *w )
{
    int         i, size;
    long        pos;

    pos = (long)w->id * buffSize();
    FileSeek( swapHandle, pos );
    size = w->width * w->height;
    i = write( swapHandle, w->overlap, size * sizeof( window_id ) );
    if( i != size * sizeof( window_id ) ) {
        return;
    }
    i = write( swapHandle, w->whooverlapping, size * sizeof( window_id ) );
    if( i != size * sizeof( window_id ) ) {
        return;
    }
    i = write( swapHandle, w->text, size * sizeof( char_info ) );
    if( i != size * sizeof( char_info ) ) {
        return;
    }
    MemFreePtr( (void **)&w->text );
    MemFreePtr( (void **)&w->whooverlapping );
    MemFreePtr( (void **)&w->overlap );
    w->isswapped = true;

} /* windowSwap */

/*
 * SwapAllWindows - swap any window data we can
 */
void SwapAllWindows( void )
{
    info        *cinfo;
    window      *w;

    if( windowSwapFileOpen() == ERR_NO_ERR ) {
        if( EditFlags.Verbose ) {
            Message1( "Swapping window data" );
        }
        for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
            w = WINDOW_FROM_ID( cinfo->CurrentWindow );
            if( !TestVisible( w ) && !w->isswapped && w->accessed == 0 ) {
                windowSwap( w );
            }
        }
    }

} /* SwapAllWindows */

/*
 * fetchWindow - get contents of a window
 */
static void fetchWindow( window *w )
{
    int         size;
    long        pos;

    size = w->width * w->height;
    w->text = MemAlloc( size * sizeof( char_info ) );
    w->whooverlapping = MemAlloc( size * sizeof( window_id ) );
    w->overlap = MemAlloc( size * sizeof( window_id ) );

    pos = (long)w->id * buffSize();
    FileSeek( swapHandle, pos );
    read( swapHandle, w->overlap, size * sizeof( window_id ) );
    read( swapHandle, w->whooverlapping, size * sizeof( window_id ) );
    read( swapHandle, w->text, size * sizeof( char_info ) );
    w->isswapped = false;

} /* fetchWindow */

/*
 * AccessWindow - get at window data
 */
void AccessWindow( window *w )
{
    w->accessed++;
    if( w->isswapped ) {
        fetchWindow( w );
    }

} /* AccessWindow */

/*
 * ReleaseWindow - no longer using window
 */
void ReleaseWindow( window *w )
{
    w->accessed--;

} /* ReleaseWindow */

/*
 * WindowSwapFileClose - do just that
 */
void WindowSwapFileClose( void )
{
    char    file[_MAX_PATH];

    if( swapHandle != -1 ) {
        close( swapHandle );
        swapHandle = -1;
        MakeTmpPath( file, swapName );
        remove( file );
    }

} /* WindowSwapFileClose */
