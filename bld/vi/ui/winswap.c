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
* Description:  Swap window data to disk.
*
****************************************************************************/


#include "vi.h"
#include <fcntl.h>
#include "posix.h"
#include "win.h"

static char swapName[L_tmpnam];
static int swapHandle = -1;

/*
 * windowSwapFileOpen - do just that
 */
static void windowSwapFileOpen( void )
{
    vi_rc   rc;

    if( swapHandle >= 0 ) {
        return;
    }

    rc = TmpFileOpen( swapName, &swapHandle );
    if( rc != ERR_NO_ERR ) {
        swapHandle = -1;
    }

} /* windowSwapFileOpen */

/*
 * buffSize - compute the size of a i/o buffer for a window
 */
static long buffSize( void )
{
    long        tmp;

    tmp = (long)WindMaxWidth * (long)WindMaxHeight * 4L;
    tmp = tmp / 512;
    tmp++;
    tmp *= 512;
    return( tmp );

} /* buffSize */

/*
 * windowSwap - swap a windows data
 */
void static windowSwap( wind *w )
{
    int         i, size;
    long        pos;

    pos = (long)w->id * buffSize();
    FileSeek( swapHandle, pos );
    size = w->width * w->height;
    i = write( swapHandle, w->overlap, size );
    if( i != size ) {
        return;
    }
    i = write( swapHandle, w->whooverlapping, size );
    if( i != size ) {
        return;
    }
    i = write( swapHandle, w->text, size * sizeof( char_info ) );
    if( i != sizeof( char_info ) * size ) {
        return;
    }
    MemFree2( &w->text );
    MemFree2( &w->whooverlapping );
    MemFree2( &w->overlap );
    w->isswapped = TRUE;

} /* windowSwap */

/*
 * SwapAllWindows - swap any window data we can
 */
void SwapAllWindows( void )
{
    info        *cinfo;
    wind        *w;

    windowSwapFileOpen();
    if( swapHandle < 0 ) {
        return;
    }
    if( EditFlags.Verbose ) {
        Message1( "Swapping window data" );
    }
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        w = Windows[cinfo->CurrentWindow];
        if( !TestVisible( w ) && !w->isswapped && w->accessed == 0 ) {
            windowSwap( w );
        }
    }

} /* SwapAllWindows */

/*
 * fetchWindow - get contents of a window
 */
static void fetchWindow( wind *w )
{
    int         size;
    long        pos;

    size = w->width * w->height;
    w->text = MemAlloc( sizeof( char_info ) * size );
    w->whooverlapping = MemAlloc( size );
    w->overlap = MemAlloc( size );

    pos = (long)w->id * buffSize();
    FileSeek( swapHandle, pos );
    read( swapHandle, w->overlap, size );
    read( swapHandle, w->whooverlapping, size );
    read( swapHandle, w->text, sizeof( char_info ) * size );
    w->isswapped = FALSE;

} /* fetchWindow */

/*
 * AccessWindow - get at window data
 */
wind *AccessWindow( window_id id )
{
    wind        *w;

    w = Windows[id];
    w->accessed++;
    if( w->isswapped ) {
        fetchWindow( w );
    }
    return( w );

} /* AccessWindow */

/*
 * ReleaseWindow - no longer using window
 */
void ReleaseWindow( wind *w )
{
    w->accessed--;

} /* ReleaseWindow */

/*
 * WindowSwapFileClose - do just that
 */
void WindowSwapFileClose( void )
{
    TmpFileClose( swapHandle, swapName );

} /* WindowSwapFileClose */
