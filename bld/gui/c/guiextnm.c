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
* Description:  Get/set resource file name.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#ifndef __WATCOMC__
    #include "clibext.h"
#endif
#include "bool.h"

static char   GUIExtName[_MAX_PATH] = "";

extern char * GUIGetExtName( void )
{
    if( GUIExtName[0] == '\0' ) {
#if defined( __UNIX__ ) && defined( GUI_EXT_RES )
        _cmdname( GUIExtName );
        strcat( GUIExtName, ".res" );
        return( GUIExtName );
#else
        return( NULL );
#endif
    }
    /* to keep the 10.6 compiler happy */
    return( GUIExtName );
}

extern bool GUISetExtName( char *fname )
{
    if( strlen( fname ) < _MAX_PATH ) {
        strcpy( GUIExtName, fname );
        return TRUE;
    }
    return FALSE;
}
