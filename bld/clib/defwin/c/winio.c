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
#include "win.h"

/*
 * _DoStdin - get stuff from stdin
 */
unsigned _DoStdin( LPWDATA w, void *in_buff, unsigned size )
{
    unsigned            rc;
    char *              buff;

    buff = (char *)in_buff;

    if( w == NULL ) return( 0 );
    if( !w->active) _MakeWindowActive( w );
    rc = _GetString( w, buff, size );
    if( _GotEOF ) {
        if( rc < size ) {
            buff[rc++] = CTRL_Z;
        }
    } else {
        if( (rc+2) < size ) {
            buff[rc++] = 0x0d;
            buff[rc++] = 0x0a;
        } else if( ( rc == 0 ) && ( size == 1 ) ) {/* rc==0 ==> key was enter */
            /* kludge to allow readers to get a single new line from the
             * system
             */
            buff[rc++] = 0x0a;
        }
    }
    return( rc );

} /* _DoStdin */

/*
 * _DoStdout - put stuff to stdout
 */
unsigned _DoStdout( LPWDATA w, const void *buff, unsigned size )
{

    if( w == NULL ) return( 0 );
    if( !w->active) _MakeWindowActive( w );
    _AddLine( w, buff, size );
    _MessageLoop( TRUE );
    return( size );

} /* _DoStdout */

/*
 * _DoKbhit - test if the keyboard has been pressed
 */
unsigned _DoKbhit( LPWDATA w )
{
    if( w == NULL ) return( FALSE );
    if( !w->active) _MakeWindowActive( w );
    return( _KeyboardHit( FALSE ) );

} /* _DoKbhit */

/*
 * _DoGetch - get a character
 */
unsigned _DoGetch( LPWDATA w )
{
    unsigned    ci;

    if( w == NULL ) return( 0 );
    if( !w->active) _MakeWindowActive( w );
    while( !_KeyboardHit( TRUE ) );
    ci = _GetKeyboard( NULL );
    if( ci > 0x80 ) ci -= 0x80;
    return( ci );

} /* _DoGetch */

/*
 * _DoGetche - get a character and echo it
 */
unsigned _DoGetche( LPWDATA w )
{
    unsigned    ci;

    ci = _DoGetch( w );
    if( ci == '\r' ) {
        _DoPutch( w, '\n' );
    } else {
        _DoPutch( w, ci );
    }
    return( ci );

} /* _DoGetche */

/*
 * _DoPutch - put a character to stdout
 */
void _DoPutch( LPWDATA w, unsigned ch )
{
    char tmp[2];

    if( w == NULL ) return;
    if( !w->active) _MakeWindowActive( w );
    tmp[0] = ch;
    tmp[1] = 0;
    _AddLine( w, tmp, 1 );
    _MessageLoop( TRUE );

} /* _DoPutch */
