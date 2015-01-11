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
#include "guiwind.h"
#include "guiutil.h"

/*
 * GUIStripTralingBlanks -- return a copy of label with the appended blanks
 *                          stripped off
 */

char *GUIStripTrailingBlanks( const char *label, bool *ok )
{
    size_t      length;
    char        *new_str;

    if( ok != NULL )
        *ok = true;
    if( label == NULL || *label == '\0' )
        return( NULL );
    for( length = strlen( label ); length > 0; --length ) {
        if( label[length - 1] != ' ' ) {
            break;
        }
    }
    if( length == 0 )
        return( NULL );
    new_str = (char *)GUIMemAlloc( length + 1 );
    if( new_str == NULL ) {
        if( ok != NULL )
            *ok = false;
        return( NULL );
    }
    memcpy( new_str, label, length );
    new_str[length] = '\0';
    return( new_str );
}

/*
 * GUIStrDupLen -- duplicate the string text up to length characters
 */

char *GUIStrDupLen( const char *text, size_t len, bool *ok )
{
    size_t  new_len;
    char    *new_str;

    if( ok != NULL )
        *ok = true;
    if( text == NULL )
        return( NULL );
    new_len = strlen( text );
    if( new_len > len ) {
        new_len = len;
    }
    new_str = (char *)GUIMemAlloc( new_len + 1 );
    if( new_str == NULL ) {
        if( ok != NULL )
            *ok = false;
        return( NULL );
    }
    memcpy( new_str, text, new_len );
    new_str[new_len] = '\0';
    return( new_str );
}


char *GUIStrDup( const char *text, bool *ok )
{
    void    *new_str;

    if( ok != NULL )
        *ok = true;
    if( text != NULL ) {
        new_str = GUIMemAlloc( strlen( text ) + 1 );
        if( new_str != NULL ) {
            return( strcpy( new_str, text ) );
        }
        if( ok != NULL ) {
            *ok = false;
        }
    }
    return( NULL );
}

static char     **GUI_argv      = NULL;
static int      GUI_argc        = 0;
static int      ArgsSet         = false;

void GUIStoreArgs( char **argv, int argc )
{
    GUI_argv = argv;
    GUI_argc = argc;
    ArgsSet  = true;
}

bool GUIGetArgs( char ***argv, int *argc )
{
    if( ArgsSet && argv && argc ) {
        *argv = GUI_argv;
        *argc = GUI_argc;
        return( true );
    }
    return( false );
}

bool GUIIsRootWindow( gui_window *wnd )
{
    if( wnd != NULL ) {
        return( (bool)(GUIGetParentWindow( wnd ) == NULL)  );
    }
    return( false );
}

gui_window *GUIGetRootWindow( void )
{
    gui_window  *root;

    root = GUIXGetRootWindow();

    return( root );
}

