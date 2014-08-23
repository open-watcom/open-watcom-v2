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


#include "guiwind.h"
#include <string.h>

/*
 * GUIStripTralingBlanks -- return a copy of label with the appended blanks
 *                          stripped off
 */

bool GUIStripTrailingBlanks( char *label, char **new )
{
    int         length;

    if( label == NULL ) {
        *new = NULL;
        return( true );
    }
    length = strlen( label ) - 1;
    for( ; length >=0;  length-- ) {
        if( label[length] != ' ' ) {
            length++;
            break;
        }
    }
    if( length <= 0 ) {
        *new = NULL;
    } else {
        *new = (char *)GUIMemAlloc( length + 1 );
        if( *new == NULL ) {
            return( false );
        } else {
            strncpy( *new, label, length );
            (*new)[length] = NULLCHAR;
        }
    }
    return( true );
}

bool GUIStrDup( char *text, char **new )
{
    if( text == NULL ) {
        if( new == NULL ) {
            new = GUIMemAlloc( sizeof( char * ) );
        }
        *new = NULL;
        return( true );
    } else {
        if( new == NULL ) {
            new = GUIMemAlloc( sizeof( char * ) );
        }
        *new = (char *)GUIMemAlloc( strlen( text ) + 1 );
        if( *new == NULL ) {
            return( false );
        }
        strcpy( *new, text );
    }
    return( true );
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

