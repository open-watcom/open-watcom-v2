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
#include "rcstr.gh"

#define NEWPROJ_INF_FILE    "newproj.inf"

int     projectTypeIndex;
char    projectDir[256];
char    projectName[256];

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
            if( mkdir( dircopy ) != 0 && errno != EEXIST ) {
                free( dircopy );
                ShowError( APPWIZ_MKDIR_FAILED );
                return( FALSE );
            }
            *ptr = ch;
        }
        ptr++;
    }
    if( mkdir( dircopy ) != 0 && errno != EEXIST ) {
        free( dircopy );
        ShowError( APPWIZ_MKDIR_FAILED );
        return( FALSE );
    }
    free( dircopy );
    return( TRUE );
}

bool NewProjCallback( gui_window *wnd, gui_event ev, void *extra )
/****************************************************************/
{
    project_type_iterator   iter;
    char                    friendlyname[128];
    unsigned                id;
    char                    *ctltext;
    
    switch( ev ) {
    case GUI_INIT_DIALOG:
        iter = GetFirstProjectType();
        while( iter != NULL ) {
            GetNextProjectType( &iter, NULL, friendlyname );
            GUIAddText( wnd, CTL_NEWPROJ_PROJTYPE, friendlyname );
        }
        GUISetCurrSelect( wnd, CTL_NEWPROJ_PROJTYPE, 0 );
        break;
    case GUI_CONTROL_CLICKED:
        GUI_GETID( extra, id );
        if( id == CTL_NEWPROJ_OK ) {
            ctltext = GUIGetText( wnd, CTL_NEWPROJ_PROJDIR );
            if( ctltext == NULL ) {
                ShowError( APPWIZ_NO_PROJDIR );
                break;
            }
            strcpy( projectDir, ctltext );
            ctltext = GUIGetText( wnd, CTL_NEWPROJ_PROJNAME );
            if( ctltext == NULL ) {
                ShowError( APPWIZ_NO_PROJNAME );
                break;
            }
            strcpy( projectName, ctltext );
            projectTypeIndex = GUIGetCurrSelect( wnd, CTL_NEWPROJ_PROJTYPE );
            if( !createProjectDir( projectDir ) ) {
                break;
            }
            GUICloseDialog( wnd );
        } else if( id == CTL_NEWPROJ_CANCEL ) {
            projectTypeIndex = -1;
            GUICloseDialog( wnd );
        }
        break;
    }
    return( TRUE );
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
        0,
        NULL,
        0,
        NULL,
        &NewProjCallback,
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
    GUIWndInit( 300, GUI_GMOUSE );

    if( !ReadProjectTypes() ) {
        FreeProjectTypes();
        return;
    }
    GUICreateResDialog( &newProjInfo, DIALOG_NEWPROJ );
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

