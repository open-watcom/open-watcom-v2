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


#include "wsyshelp.hpp"


WEXPORT WSystemHelp::WSystemHelp( WWindow *win, const char *title,
                    const char *hlib, const char *chmfile, int offset )
    : _title( title )
    , _library( hlib )
    , _chmfile( chmfile )
    , _offset( offset )
    , _helpWindow( win )
    , _helpInstance( NULL )
/***************************/
{
    _helpInstance = GUIHelpInit( win->handle(), (char *)hlib, (char *)title );
}


WEXPORT WSystemHelp::~WSystemHelp()
/*********************************/
{
    if( _helpInstance ) {
        if( _helpWindow->handle() ) {
            GUIHelpFini( _helpInstance, _helpWindow->handle(), (char *)_library );
        }
    }
}


bool WEXPORT WSystemHelp::sysHelpContent( void )
/**********************************************/
{
    if( !_helpInstance ) {
        return( FALSE );
    }
#ifdef __NT__
    if( _chmfile != NULL && *_chmfile != '\0' ) {
        if( GUIShowHtmlHelp( _helpInstance, _helpWindow->handle(),
                             GUI_HELP_CONTENTS, (char *)_chmfile, NULL ) ) {
            return( TRUE );
        }
    }
#endif
    return( GUIShowHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_CONTENTS, (char *)_library, NULL ) );
}


bool WEXPORT WSystemHelp::sysHelpOnHelp( void )
/*********************************************/
{
    if( !_helpInstance ) {
        return( FALSE );
    }
#ifdef __NT__
#if 0
    if( _chmfile != NULL && *_chmfile != '\0' ) {
        if( GUIShowHtmlHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_ON_HELP, (char *)_chmfile, NULL ) ) {
            return( TRUE );
        }
    }
#endif
#endif
    return( GUIShowHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_ON_HELP, (char *)_library, NULL ) );
}


bool WEXPORT WSystemHelp::sysHelpSearch( const char *topic )
/**********************************************************/
{
    if( !_helpInstance ) {
        return( FALSE );
    }
#ifdef __NT__
    if( _chmfile != NULL && *_chmfile != '\0' ) {
        if( GUIShowHtmlHelp( _helpInstance, _helpWindow->handle(),
                             GUI_HELP_SEARCH, (char *)_chmfile, (char *)topic ) ) {
            return( TRUE );
        }
    }
#endif
    return( GUIShowHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_SEARCH, (char *)_library, (char *)topic ) );
}


bool WEXPORT WSystemHelp::sysHelpTopic( const char *topic )
/*********************************************************/
{
    if( !_helpInstance ) {
        return( FALSE );
    }
#ifdef __NT__
    if( _chmfile != NULL && *_chmfile != '\0' ) {
        if( GUIShowHtmlHelp( _helpInstance, _helpWindow->handle(),
                             GUI_HELP_KEY, (char *)_chmfile, (char *)topic ) ) {
            return( TRUE );
        }
    }
#endif
    return( GUIShowHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_KEY, (char *)_library, (char *)topic ) );
}


bool WEXPORT WSystemHelp::sysHelpId( int help_id )
/************************************************/
{
    if( !_helpInstance ) {
        return( FALSE );
    }
#ifdef __NT__
    if( _chmfile != NULL && *_chmfile != '\0' ) {
        return( GUIShowHtmlHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_CONTEXT, (char *)_chmfile, (char *)(pointer_int)help_id ) );
    }
#endif
    return( GUIShowHelp( _helpInstance, _helpWindow->handle(),
                         GUI_HELP_CONTEXT, (char *)_library, (char *)(pointer_int)help_id ) );
}

const char * WEXPORT WSystemHelp::getHelpFile( void )
/***************************************************/
{
    if( _chmfile != NULL && *_chmfile != '\0' ) {
        return( _chmfile );
    }
    return( _library );
}
