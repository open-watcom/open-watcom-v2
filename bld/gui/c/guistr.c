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
#include "guiutil.h"
#define DEFINE_STRINGS
#include "guistr.h"

#define BUFFER_SIZE     128

static char     GUIStrTextBuffer[BUFFER_SIZE];
static char *GUIGetInternalLiteralString( int id )
{
    char        *str;

    str = NULL;
    GUIStrTextBuffer[0] = '\0';
    if( GUIIsLoadStrInitialized() ) {
        if( !GUILoadString( id, GUIStrTextBuffer, BUFFER_SIZE ) ) {
            GUIStrTextBuffer[0] = '\0';
        }
    }

    GUIStrDup( GUIStrTextBuffer, &str );

    return( str );
}

bool GUIInitInternalStringTable( void )
{
    int         i;

#undef guipick
#define guipick( a, b, c ) __LIT##a = GUIGetInternalLiteralString( i ); i++;
    i=GUI_LITERAL_Internal_First_Message;
#include "gui.msg"
#undef guipick

    return( TRUE );
}

bool GUIFiniInternalStringTable( void )
{
#undef guipick
#define guipick( a, b, c ) GUIMemFree( __LIT##a );
#include "gui.msg"
#undef guipick
    return( TRUE );
}

