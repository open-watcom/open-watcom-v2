/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Application Wizard main source file.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <errno.h>
#include "gui.h"
#include "newproj.h"
#include "projtype.h"
#include "errmsg.h"
#include "rcstr.grh"
#include "wresdefn.h"


#define NEWPROJ_INF_FILE    "newproj.inf"

int     projectTypeIndex;
char    projectDir[256];
char    projectName[256];

#define MKDIR(d)    (mkdir(d) && errno != EEXIST && errno != EACCES)

static bool createProjectDir( char *dir )
/***************************************/
{
    char *dircopy;
    char *ptr;
    char ch;

    dircopy = strdup( dir );
    ptr = dircopy;
    while( *ptr != '\0' ) {
        if( *ptr == '/' || (*ptr == '\\' && ptr != dircopy && *(ptr - 1) != ':') ) {
            ch = *ptr;
            *ptr = '\0';
            if( MKDIR( dircopy ) ) {
                free( dircopy );
                ShowError( APPWIZ_MKDIR_FAILED );
                return( false );
            }
            *ptr = ch;
        }
        ptr++;
    }
    if( MKDIR( dircopy ) ) {
        free( dircopy );
        ShowError( APPWIZ_MKDIR_FAILED );
        return( false );
    }
    free( dircopy );
    return( true );
}

bool NewProjGUIEventProc( gui_window *wnd, gui_event gui_ev, void *extra )
/************************************************************************/
{
    project_type_iterator   iter;
    char                    friendlyname[128];
    unsigned                id;
    char                    *ctltext;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        iter = GetFirstProjectType();
        while( iter != NULL ) {
            GetNextProjectType( &iter, NULL, friendlyname );
            GUIAddText( wnd, CTL_NEWPROJ_PROJTYPE, friendlyname );
        }
        GUISetCurrSelect( wnd, CTL_NEWPROJ_PROJTYPE, 0 );
        return( true );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( extra, id );
        switch( id ) {
        case CTL_NEWPROJ_OK:
            ctltext = GUIGetText( wnd, CTL_NEWPROJ_PROJDIR );
            if( ctltext == NULL ) {
                ShowError( APPWIZ_NO_PROJDIR );
                return( true );
            }
            strcpy( projectDir, ctltext );
            ctltext = GUIGetText( wnd, CTL_NEWPROJ_PROJNAME );
            if( ctltext == NULL ) {
                ShowError( APPWIZ_NO_PROJNAME );
                return( true );
            }
            strcpy( projectName, ctltext );
            GUIGetCurrSelect( wnd, CTL_NEWPROJ_PROJTYPE, &projectTypeIndex );
            if( !createProjectDir( projectDir ) ) {
                return( true );
            }
            GUICloseDialog( wnd );
            return( true );
        case CTL_NEWPROJ_CANCEL:
            projectTypeIndex = -1;
            GUICloseDialog( wnd );
            return( true );
        }
        break;
    default:
        break;
    }
    return( false );
}

extern void GUImain( void )
/*************************/
{
    gui_create_info newProjInfo = {
        NULL,
        { 0, 0, 0, 0 },
        GUI_NOSCROLL,
        GUI_VISIBLE | GUI_CLOSEABLE,
        NULL,
        { 0, NULL },                        // Menu array
        { 0, NULL },                        // Colour attribute array
        &NewProjGUIEventProc,               // GUI Event Callback Function
        NULL,
        NULL,
        NULL                                // Menu Resource
    };

    gui_rect                rect;
    project_type_iterator   iter;
    char                    typename[256];
    char                    newprojpath[256];

    rect.x = 0;
    rect.y = 0;
    rect.width = 1000;
    rect.height = 1000;
    GUISetScale( &rect );
    GUIWndInit( 300 /* ms */, GUI_GMOUSE );

    if( !ReadProjectTypes() ) {
        FreeProjectTypes();
        return;
    }
    GUICreateResDialog( &newProjInfo, MAKEINTRESOURCE( DIALOG_NEWPROJ ) );
    if( projectTypeIndex >= 0 ) {
        iter = GetFirstProjectType();
        while( projectTypeIndex >= 0 ) {
            GetNextProjectType( &iter, typename, NULL );
            projectTypeIndex--;
        }
        GetTemplateFilePath( typename, NEWPROJ_INF_FILE, newprojpath );
    }
    FreeProjectTypes();
}

