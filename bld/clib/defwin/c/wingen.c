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


#include "variety.h"
#if defined( __OS2__ )
  #define INCL_GPI
  #define INCL_WIN
  #include <wos2.h>
#endif
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "win.h"

/*
 * _GetWindowData - test if a window handle is a display window...
 */
LPWDATA _GetWindowData( HWND hwnd )
{
    int i;

    for( i=0;i<_MainWindowData->window_count;i++ ) {
        if( _MainWindowData->windows[i]->hwnd == hwnd ) {
            return( _MainWindowData->windows[i] );
        }
    }
    return( NULL );

} /* _GetWindowData */

/*
 * _GetActiveWindowData
 */
LPWDATA _GetActiveWindowData( void )
{
    int i;

    for( i=0;i<_MainWindowData->window_count;i++ ) {
        if( _MainWindowData->windows[i]->active ) {
            return( _MainWindowData->windows[i] );
        }
    }
    return( NULL );

} /* _GetActiveWindowData */

/*
 * _MakeWindowActive - force a given window to become active
 */
void _MakeWindowActive( LPWDATA w )
{
    int         i;
    LPWDATA     last=NULL;

    for( i=0;i<_MainWindowData->window_count;i++ ) {
        if( _MainWindowData->windows[i]->active ) {
            last = _MainWindowData->windows[i];
            last->active = FALSE;
            break;
        }
    }
    w->active = TRUE;
    if( last == w ) last = NULL;
    _ShowWindowActive( w, last );

} /* _MakeWindowActive */

/*
 * _IsWindowedHandle - check if given handle is a windowed one or not
 */
LPWDATA _IsWindowedHandle( int handle )
{
    int         i,j;
    LPWDATA     w;

    for( i=0;i<_MainWindowData->window_count;i++ ) {
        w = _MainWindowData->windows[i];
        for( j=0;j<w->handle_cnt;j++ ) {
            if( handle == w->handles[j] ) return( w );
        }
    }
    return( NULL );

} /* _IsWindowedHandle */

/*
 * _RemoveWindowedHandle - remove given handle from list of windowed handles
 */
void _RemoveWindowedHandle( int handle )
{
    int         i,j,k;
    LPWDATA     w;

    for( i=0;i<_MainWindowData->window_count;i++ ) {
        w = _MainWindowData->windows[i];
        for( j=0;j<w->handle_cnt;j++ ) {
            if( handle == w->handles[j] ) {
                w->handle_cnt--;
                for( k=j;k<w->handle_cnt;k++ ) {
                    w->handles[k] = w->handles[k+1];
                }
            }
        }
    }

} /* _RemoveWindowedHandle */

/*
 * _WindowsExit - hang around and wait for finish...
 */
void _WindowsExit( void )
{
    _ExecutionComplete();
    if( !_MainWindowDestroyed )  while( _BlockingMessageLoop( FALSE ) );
    _WindowExitRtn = NULL;

} /* _WindowsExit */

/*
 * _DestroyOnClose - Destroy the window when closed
 */
int _DestroyOnClose( LPWDATA w )
{
    w->destroy = 1;
    return( TRUE );
} /* _DestroyOnClose */

/*
 * _YieldControl - Calls _MessageLoop to yeild control to OS.
 */
int _YieldControl( void )
{
    _MessageLoop( TRUE );
    return( TRUE );
} /* _YieldControl */

/*
 * _InitFunctionPointers - set up ptrs for default windowing routines
 */
void _InitFunctionPointers( void )
{
    _WindowsStdin = _DoStdin;
    _WindowsStdout = _DoStdout;
    _WindowsKbhit = _DoKbhit;
    _WindowsGetch = _DoGetch;
    _WindowsGetche = _DoGetche;
    _WindowsPutch = _DoPutch;
    _WindowExitRtn = _WindowsExit;
    _WindowsIsWindowedHandle = _IsWindowedHandle;
    _WindowsRemoveWindowedHandle = _RemoveWindowedHandle;
    _WindowsNewWindow = _NewWindow;
    _WindowsCloseWindow = _CloseWindow;
    _WindowsSetAbout = _SetAboutDlg;
    _WindowsSetAppTitle = _SetAppTitle;
    _WindowsSetConTitle = _SetConTitle;
    _WindowsDestroyOnClose = _DestroyOnClose;
    _WindowsYieldControl = _YieldControl;
    _WindowsShutDown = _ShutDown;

} /* _InitFunctionPointers */

/*
 * _MemAlloc - allocate some memory
 */
void _WCI86FAR *_MemAlloc( unsigned size )
{
    void _WCI86FAR      *tmp;

    tmp = FARmalloc( size );
    if( tmp == NULL ) {
        _OutOfMemory();
        while( _MessageLoop( FALSE ) );
        _WindowExitRtn = NULL;
        exit( 0 );
    }
    FARmemset( tmp, 0, size );
    return( tmp );

} /* _MemAlloc */

/*
 * _MemReAlloc - allocate some memory
 */
void _WCI86FAR *_MemReAlloc( void _WCI86FAR *ptr, unsigned size )
{
    void _WCI86FAR      *tmp;

    tmp = FARrealloc( ptr, size );
    if( tmp == NULL ) {
        _OutOfMemory();
        while( _MessageLoop( FALSE ) );
        _WindowExitRtn = NULL;
        exit( 0 );
    }
    return( tmp );

} /* _MemReAlloc */

/*
 * _MemFree - free a block
 */
void _MemFree( void _WCI86FAR *ptr )
{
    FARfree( ptr );

} /* _MemFree */

#ifdef __NT__
#define X_MAX   70
#else
#define X_MAX   79
#endif
#define Y_MAX   27
/*
 * _GetWindowNameAndCoords - set up name and coordinates for a new window
 */
void _GetWindowNameAndCoords( char *name, char *dest,
                        int *x1, int *x2, int *y1, int *y2 )
{
    static int  _concnt = 0;
    static int  _x1 = 0;
    static int  _x2 = X_MAX;
    static int  _y1 = 0;
    static int  _y2 = Y_MAX;

    if( name == NULL ) {
        _concnt++;
        sprintf( dest,"Console %d", _concnt );
    } else {
        strcpy( dest, name );
    }
    *x1 = _x1;
    *x2 = _x2;
    *y1 = _y1;
    *y2 = _y2;

    _x1 += 2;
    _y1 += 2;
    _x2 -= 2;
    _y2 -= 2;
    if( _x1 > X_MAX/3 ) {
        _x1 = 0;
        _x2 = X_MAX;
    }
    if( _y1 > Y_MAX/3 ) {
        _y1 = 0;
        _y2 = Y_MAX;
    }

} /* _GetWindowNameAndCoords */

/*
 * _InitMainWindowData - set up main window data area
 */
void _InitMainWindowData( HANDLE inst )
{

    _MainWindowData = _MemAlloc( sizeof( window_data ) );
    FARmemset( _MainWindowData, 0, sizeof( window_data ) );
    _MainWindowData->inst = inst;
    _MainWindowData->window_count = 0;
    _MainWindowData->windows = NULL;

} /* _InitMainWindowData */

/*
 * _AnotherWindowData - create yet another window data item
 */
LPWDATA _AnotherWindowData( HWND hwnd, va_list al )
{
    LPWDATA     w;
    int         h,hcnt,*hlist;

    w = _MemAlloc( sizeof( window_data ) );
    FARmemset( w, 0, sizeof( window_data ) );
    _MainWindowData->windows = _MemReAlloc( _MainWindowData->windows,
                sizeof( LPWDATA ) * ( _MainWindowData->window_count + 1 ) );
    _MainWindowData->windows[_MainWindowData->window_count] = w;
    _MainWindowData->window_count++;

    w->CurrentLineNumber = 1L;
    w->TopLineNumber = 1L;
    w->LastLineNumber = 1L;
    w->tmpbuff = _MemAlloc( sizeof( line_data ) + MAX_BUFF+1 );
    w->CaretType = ORIGINAL_CURSOR;
    w->hwnd = hwnd;

    hcnt = 0;
    hlist = NULL;
    while( 1 ) {
        h = va_arg( al, int );
        if( h == -1 ) break;
        hlist = realloc( hlist, (hcnt+1) * sizeof( h ) );
        hlist[hcnt] = h;
        hcnt++;
    }
    w->handles = hlist;
    w->handle_cnt = hcnt;
    return( w );

} /* _AnotherWindowData */

/*
 * _DestroyAWindow - get rid of a windows data
 */
void _DestroyAWindow( LPWDATA w )
{
    int i,j;

    for( i=0;i<_MainWindowData->window_count;i++ ) {
        if( _MainWindowData->windows[i] == w ) {
            _MainWindowData->window_count--;
            for( j=i;j<_MainWindowData->window_count;j++ ) {
                _MainWindowData->windows[j] = _MainWindowData->windows[j+1];
            }
            _ReleaseWindowResources( w );
            _MemFree( w->handles );
            _MemFree( w );
            return;
        }
    }

} /* _DestroyAWindow */
