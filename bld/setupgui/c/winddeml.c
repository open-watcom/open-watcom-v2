/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Functions for adding groups/icons to the Windows shell.
*               Supports both Windows 3.x/NT 3.x and Win9x/NT 4 shells.
*
****************************************************************************/


#define INCLUDE_DDEML_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "wio.h"
#include "watcom.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"
#include "guistats.h"
#include "utils.h"

#include "clibext.h"


#define DDE_WAITTIME    3000

static bool ddeSendCommand( DWORD ddeinst, HCONV hconv, VBUF *buff )
/******************************************************************/
{
    HDDEDATA    hData;
    HDDEDATA    hrc;

    // Note data handle is freed by call to DdeClientTransaction
    hData = DdeCreateDataHandle( ddeinst, (LPBYTE)VbufString( buff ), (DWORD)( VbufLen( buff ) + 1 ), 0, (HSZ)NULL, 0L, 0L );
    if( hData == (HDDEDATA)NULL ) {
        return( false );
    }

    hrc = DdeClientTransaction( (LPBYTE)hData, (DWORD)-1L, hconv, (HSZ)NULL, 0L, XTYP_EXECUTE, DDE_WAITTIME, NULL );
    if( hrc == (HDDEDATA)NULL ) {
        return( false );
    }

    return( true );
}

static bool ddeCreateGroup( DWORD ddeinst, HCONV hconv, const VBUF *group, const VBUF *fname )
/********************************************************************************************/
{
    VBUF    buff;
    bool    ok;

    // send "[CreateGroup(%s,%s)]"
    //   or
    // send "[CreateGroup(%s,0)]"
    //   or
    // send "[CreateGroup(%s)]"
    VbufInit( &buff );
    VbufConcStr( &buff, "[CreateGroup(" );
    VbufConcVbuf( &buff, group );
    if( VbufLen( fname ) > 0 ) {
        VbufConcChr( &buff, ',' );
        VbufConcVbuf( &buff, fname );
#if defined( __NT__ )
    } else {
        VbufConcStr( &buff, ",0" );     // create a personal group
#endif
    }
    VbufConcStr( &buff, ")]" );
    ok = ddeSendCommand( ddeinst, hconv, &buff );
    VbufFree( &buff );
    return( ok );
}

static bool ddeDeleteGroup( DWORD ddeinst, HCONV hconv, const VBUF *group )
/*************************************************************************/
{
    VBUF    buff;
    bool    ok;

    // send "[DeleteGroup(%s)]"
    VbufInit( &buff );
    VbufConcStr( &buff, "[DeleteGroup(" );
    VbufConcVbuf( &buff, group );
    VbufConcStr( &buff, ")]" );
    ok = ddeSendCommand( ddeinst, hconv, &buff );
    VbufFree( &buff );
    return( ok );
}

static bool ddeGroupReplaceItem( DWORD ddeinst, HCONV hconv, const VBUF *prog_desc )
/**********************************************************************************/
{
    VBUF    buff;
    bool    ok;

    // send "[ReplaceItem(%s)]"
    VbufInit( &buff );
    VbufConcStr( &buff, "[ReplaceItem(" );
    VbufConcVbuf( &buff, prog_desc );
    VbufConcStr( &buff, ")]" );
    ok = ddeSendCommand( ddeinst, hconv, &buff );
    VbufFree( &buff );
    return( ok );
}

static bool ddeGroupAddItem1( DWORD ddeinst, HCONV hconv, const VBUF *prog_name,
            const VBUF *prog_desc, const VBUF *prog_args, const VBUF *working_dir,
                                const VBUF *iconfile, int iconindex )
/********************************************************************************/
{
    VBUF    buff;
    bool    ok;

    // send "[AddItem(%s,%s,%s,%d,-1,-1,%s)]"
    VbufInit( &buff );
    VbufConcStr( &buff, "[AddItem(" );
    VbufConcVbuf( &buff, prog_name );
    VbufConcVbuf( &buff, prog_args );
    VbufConcChr( &buff, ',' );
    VbufConcVbuf( &buff, prog_desc );
    VbufConcChr( &buff, ',' );
    VbufConcVbuf( &buff, iconfile );
    VbufConcChr( &buff, ',' );
    VbufConcInteger( &buff, iconindex, 0 );
    VbufConcStr( &buff, ",-1,-1," );
    VbufConcVbuf( &buff, working_dir );
    VbufConcStr( &buff, ")]" );
    ok = ddeSendCommand( ddeinst, hconv, &buff );
    VbufFree( &buff );
    return( ok );
}

#if defined( __WINDOWS__ )
static bool ddeGroupAddItem2( DWORD ddeinst, HCONV hconv, const VBUF *prog_name,
            const VBUF *prog_desc, const VBUF *prog_args, const VBUF *working_dir,
                                const VBUF *iconfile, int iconindex )
/********************************************************************************/
{
    VBUF    buff;
    bool    ok;

    // send "[AddItem(%s%s,%s,%s,%d)]"
    VbufInit( &buff );
    VbufConcStr( &buff, "[AddItem(" );
    VbufConcVbuf( &buff, working_dir );
    VbufConcVbuf( &buff, prog_name );
    VbufConcVbuf( &buff, prog_args );
    VbufConcChr( &buff, ',' );
    VbufConcVbuf( &buff, prog_desc );
    VbufConcChr( &buff, ',' );
    VbufConcVbuf( &buff, iconfile );
    VbufConcChr( &buff, ',' );
    VbufConcInteger( &buff, iconindex, 0 );
    VbufConcStr( &buff, ")]" );
    ok = ddeSendCommand( ddeinst, hconv, &buff );
    VbufFree( &buff );
    return( ok );
}
#endif

static bool ddeGroupAddItem( DWORD ddeinst, HCONV hconv, const VBUF *prog_name,
            const VBUF *prog_desc, const VBUF *prog_args, const VBUF *working_dir,
                                const VBUF *iconfile, int iconindex )
/********************************************************************************/
{
    bool    ok;
#if defined( __WINDOWS__ )
    WORD    version;
#endif

#if defined( __WINDOWS__ )
    version = (WORD)GetVersion();
    if( (LOBYTE( version ) > 3) ||    // Version 3.1 or higher
        (LOBYTE( version ) == 3 && HIBYTE( version ) > 0) ) {
#endif
        ok = ddeGroupReplaceItem( ddeinst, hconv, prog_desc );
        ok = ddeGroupAddItem1( ddeinst, hconv, prog_name, prog_desc, prog_args, working_dir, iconfile, iconindex );
#if defined( __WINDOWS__ )
    } else {
        ok = ddeGroupAddItem2( ddeinst, hconv, prog_name, prog_desc, prog_args, working_dir, iconfile, iconindex );
    }
#endif
    return( ok );
}

static bool UseDDE( bool uninstall )
/**********************************/
{
    int                 dir_index;
    int                 iconindex;
    int                 i;
    int                 num_icons;
    int                 num_groups;
    int                 num_installed;
    int                 num_total_install;
    bool                ok;
    VBUF                group;
    VBUF                prog_name;
    VBUF                prog_desc;
    VBUF                prog_args;
    VBUF                iconfile;
    VBUF                working_dir;
    VBUF                tmp;
    HWND                hwnd_pm;
    DWORD               ddeinst = 0; // Important that this is initially 0
    UINT                rc;
    HSZ                 happ;
    HSZ                 htopic;
    HCONV               hconv;
    char                progman[] = "PROGMAN";

    if( !uninstall ) {
        if( !SimIsPMApplGroupDefined() ) {
            return( true );
        }
    }
    /*
     * Initiate a conversation with the Program Manager.
     * NOTE: No callback provided since we only issue execute commands
     */
    rc = DdeInitialize( &ddeinst, NULL, APPCMD_CLIENTONLY, 0L );
    if( rc != 0 ) {
        return( false );
    }
    happ = DdeCreateStringHandle( ddeinst, progman, CP_WINANSI );
    htopic = DdeCreateStringHandle( ddeinst, progman, CP_WINANSI );
    hconv = DdeConnect( ddeinst, happ, htopic, NULL );
    ok = ( hconv != (HCONV)NULL );
    if( ok ) {
        VbufInit( &group );
        VbufInit( &working_dir );
        VbufInit( &prog_name );
        VbufInit( &prog_desc );
        VbufInit( &prog_args );
        VbufInit( &iconfile );
        VbufInit( &tmp );
        /*
         * Disable the Program Manager so that the user can't work with it
         * while we are doing our stuff.
         */
        hwnd_pm = FindWindow( progman, NULL );
        if( hwnd_pm != NULL ) {
            ShowWindow( hwnd_pm, SW_RESTORE );
            EnableWindow( hwnd_pm, false );
        }
        if( uninstall ) {
            // Delete the PM Group box
            num_groups = SimGetPMGroupsNum();
            for( i = 0; i < num_groups; i++ ) {
                SimGetPMGroupName( i, &group );
                if( VbufLen( &group ) > 0 ) {
                    /*
                     * Delete the PM Group box
                     */
                    ok = ddeDeleteGroup( ddeinst, hconv, &group );
                }
            }
        } else {
            SimGetPMApplGroupName( &group );
            /*
             * Delete the PM Group box to get rid of stale icons
             * (Don't do this for SQL install, since user may install
             * the server, and then install the client)
             */
            ok = ddeDeleteGroup( ddeinst, hconv, &group );
            /*
             * re-Create the PM Group box.
             */
            SimGetPMApplGroupFile( &tmp );
            ok = ddeCreateGroup( ddeinst, hconv, &group, &tmp );

            /*
             * Add the individual PM files to the Group box.
             */
            num_icons = SimGetPMsNum();
            StatusLines( STAT_CREATEPROGRAMFOLDER, "" );
            num_total_install = 0;
            for( i = 0; i < num_icons; i++ ) {
                if( SimCheckPMCondition( i ) ) {
                    ++num_total_install;
                }
            }
            num_installed = 0;
            StatusAmount( 0, num_total_install );
            for( i = 0; ok && ( i < num_icons ); i++ ) {
                if( !SimCheckPMCondition( i ) ) {
                    continue;
                }
                SimGetPMDesc( i, &prog_desc );
                if( SimPMIsGroup( i ) ) {
                    /*
                     * Delete the PM Group box to get rid of stale icons
                     */
                    ok = ddeDeleteGroup( ddeinst, hconv, &prog_desc );
                    /*
                     * Creating a new group
                     */
                    SimGetPMParms( i, &prog_args );
                    ok = ddeCreateGroup( ddeinst, hconv, &prog_desc, &prog_args );
                } else {
                    /*
                     * Adding item to group
                     */
                    dir_index = SimGetPMProgInfo( i, &prog_name );
                    if( dir_index == -1 ) {
                        VbufRewind( &working_dir );
                        ReplaceVars1( &prog_name );
                    } else {
                        SimDirNoEndSlash( dir_index, &working_dir );
                    }
                    /*
                     * Get parameters
                     */
                    SimGetPMParms( i, &prog_args );
                    ReplaceVars1( &prog_args );
                    /*
                     * Append the subdir where the icon file is and the icon file's name.
                     */
                    dir_index = SimGetPMIconInfo( i, &iconfile, &iconindex );
                    if( iconindex == -1 ) {
                        iconindex = 0;
                    }
                    if( dir_index != -1 ) {
                        SimGetDir( dir_index, &tmp );
                        VbufPrepVbuf( &iconfile, &tmp );
                    }
                    /*
                     * Add the new file to the already created PM Group.
                     */
                    ok = ddeGroupAddItem( ddeinst, hconv, &prog_name, &prog_desc, &prog_args, &working_dir, &iconfile, iconindex );
                }
                ++num_installed;
                StatusAmount( num_installed, num_total_install );
                if( StatusCancelled() ) {
                    break;
                }
            }
            StatusAmount( num_total_install, num_total_install );
        }
        /*
         * Enable the Program Manager
         */
        if( hwnd_pm != NULL ) {
            EnableWindow( hwnd_pm, TRUE );
        }
        /*
         * Terminate the DDE conversation with the Program Manager.
         * DdeDisconnect( hconv ); // win95 setup was crashing on ddeuninitialize
         * (only if running from CD)removing this call seems to be an OK workaround
         */
//         DdeDisconnect( hconv );
        VbufFree( &tmp );
        VbufFree( &iconfile );
        VbufFree( &prog_args );
        VbufFree( &prog_desc );
        VbufFree( &prog_name );
        VbufFree( &working_dir );
        VbufFree( &group );
    }
    DdeFreeStringHandle( ddeinst, happ );
    DdeFreeStringHandle( ddeinst, htopic );
    DdeUninitialize( ddeinst );
    return( ok );
}

#if defined( __NT__ )

#include <direct.h>
#include <shlobj.h>

// DDE method does not work reliably under Windows 95. Preferred
// method is to use IShellLink interface

// Directory names cannot have forward slashes in them, and probably other
// characters. This is a problem for "C/C++". Not all platforms are restricted
// like this, so just munge the file name here.
static void munge_fname_add( VBUF *buff, const VBUF *name )
/*********************************************************/
{
    const char  *start;
    const char  *p;
    const char  *replacement;

    start = VbufString( name );
    for( p = start; *p != '\0'; p++ ) {
        if( *p == '/' ) {
            replacement = " - ";
        } else {
            continue;
        }
        VbufConcBuffer( buff, start, p - start );
        VbufConcStr( buff, replacement );
        start = p + 1;
    }
    if( p != start ) {
        VbufConcBuffer( buff, start, p - start );
    }
}

static void get_group_name( VBUF *buff, const VBUF *group )
/*********************************************************/
{
    LPITEMIDLIST    ppidl;
    char            tmp[_MAX_PATH];

    VbufRewind( buff );
    if( SHGetSpecialFolderLocation( NULL, CSIDL_PROGRAMS, &ppidl ) == NOERROR ) {
        SHGetPathFromIDList( ppidl, tmp );
        VbufConcStr( buff, tmp );
    } else {
        GetWindowsDirectoryVbuf( buff );
        VbufConcStr( buff, "\\Start Menu\\Programs" );
    }
    VbufConcChr( buff, '\\' );
    munge_fname_add( buff, group );
}

static bool linkCreateGroup( const VBUF *group )
/**********************************************/
{
    VBUF            dir_name;
    bool            ok;

    VbufInit( &dir_name );
    get_group_name( &dir_name, group );
    ok = ( mkdir_vbuf( &dir_name ) == 0 || errno == EEXIST || errno == EACCES );
    VbufFree( &dir_name );
    return( ok );
}

static void delete_dir( const VBUF *dir )
/***************************************/
{
    DIR                 *dirp;
    struct dirent       *dire;
    VBUF                file;
    size_t              dir_len;

    VbufInit( &file );
    // Delete contents of directory
    VbufConcVbuf( &file, dir );
    VbufConcChr( &file, '\\' );
    dir_len = VbufLen( &file );
    VbufConcStr( &file, "*.*" );
    dirp = opendir_vbuf( &file );
    if( dirp != NULL ) {
        for( ; (dire = readdir( dirp )) != NULL; ) {
            if( dire->d_attr & _A_SUBDIR ) {   /* don't care about directories */
                continue;
            }
            VbufSetStrAt( &file, dire->d_name, dir_len );
            remove_vbuf( &file );
        }
        closedir( dirp );
    }
    rmdir_vbuf( dir );
    VbufFree( &file );
}

static void linkDeleteGroup( const VBUF *group )
/**********************************************/
{
    VBUF    dir_name;

    VbufInit( &dir_name );
    get_group_name( &dir_name, group );
    delete_dir( &dir_name );
    VbufFree( &dir_name );
}

static bool linkGroupAddItem( const VBUF *group, const VBUF *prog_name, const VBUF *prog_desc,
        const VBUF *prog_args, const VBUF *working_dir, const VBUF *iconfile, int iconindex )
/********************************************************************************************/
{
    HRESULT             hres;
    IShellLink          *m_link;
    IPersistFile        *p_file;
    WORD                w_link[_MAX_PATH];
    VBUF                link;

    VbufInit( &link );
    // Determine names of link files
    get_group_name( &link, group );
    VbufConcChr( &link, '\\' );
    munge_fname_add( &link, prog_desc );
    VbufConcStr( &link, ".lnk" );

    MultiByteToWideChar( CP_ACP, 0, VbufString( &link ), -1, w_link, _MAX_PATH );

    // Create an IShellLink object and get a pointer to the IShellLink interface
    m_link = NULL;
    hres = CoCreateInstance( &CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (void **)&m_link );
    if( SUCCEEDED( hres ) ) {
        // Query IShellLink for the IPersistFile interface for
        // saving the shortcut in persistent storage.
        p_file = NULL;
        hres = m_link->lpVtbl->QueryInterface( m_link, &IID_IPersistFile, (void **)&p_file );
        if( SUCCEEDED( hres ) ) {
            // Set the properties of the shortcut
            hres = m_link->lpVtbl->SetPath( m_link, VbufString( prog_name ) );
            hres = m_link->lpVtbl->SetDescription( m_link, VbufString( prog_desc ) );
            hres = m_link->lpVtbl->SetWorkingDirectory( m_link, VbufString( working_dir ) );
            hres = m_link->lpVtbl->SetArguments( m_link, VbufString( prog_args ) );
            hres = m_link->lpVtbl->SetIconLocation( m_link, VbufString( iconfile ), iconindex );

            // Save the shortcut via the IPersistFile::Save member function.
            hres = p_file->lpVtbl->Save( p_file, w_link, TRUE );
            // Release the pointer to IPersistFile.
            p_file->lpVtbl->Release( p_file );
       }
       // Release the pointer to IShellLink.
       m_link->lpVtbl->Release( m_link );
    }
    VbufFree( &link );
    return( SUCCEEDED( hres ) );
}

static bool UseIShellLink( bool uninstall )
/*****************************************/
{
    int                 dir_index;
    int                 iconindex;
    int                 i;
    int                 num_icons;
    int                 num_groups;
    int                 num_installed;
    int                 num_total_install;
    VBUF                prog_name;
    VBUF                prog_desc;
    VBUF                iconfile;
    VBUF                working_dir;
    VBUF                group;
    VBUF                prog_args;
    VBUF                tmp;
    bool                ok;

    ok = true;
    VbufInit( &group );
    if( uninstall ) {
        num_groups = SimGetPMGroupsNum();
        for( i = 0; i < num_groups; i++ ) {
            SimGetPMGroupName( i, &group );
            if( VbufLen( &group ) > 0 ) {
                // Delete the PM Group box
                linkDeleteGroup( &group );
            }
        }
    } else if( SimIsPMApplGroupDefined() ) {
        CoInitialize( NULL );
        // Create the PM Group box.
        SimGetPMApplGroupName( &group );
        ok = linkCreateGroup( &group );
        if( ok ) {
            // Add the individual PM files to the Group box.
            num_icons = SimGetPMsNum();
            StatusLines( STAT_CREATEPROGRAMFOLDER, "" );
            num_total_install = 0;
            for( i = 0; i < num_icons; i++ ) {
                if( SimCheckPMCondition( i ) ) {
                    ++num_total_install;
                }
            }
            VbufInit( &prog_name );
            VbufInit( &prog_desc );
            VbufInit( &iconfile );
            VbufInit( &working_dir );
            VbufInit( &prog_args );
            VbufInit( &tmp );
            num_installed = 0;
            StatusAmount( 0, num_total_install );
            for( i = 0; ok && i < num_icons; i++ ) {
                if( !SimCheckPMCondition( i ) ) {
                    continue;
                }
                if( SimPMIsGroup( i ) ) {
                    /* creating a new group */
                    SimGetPMDesc( i, &group );
                    ok = linkCreateGroup( &group );
                } else {
                    SimGetPMDesc( i, &prog_desc );
                    /*
                     * Adding item to group
                     */
                    dir_index = SimGetPMProgInfo( i, &prog_name );
                    if( dir_index == -1 ) {
                        VbufRewind( &working_dir );
                        ReplaceVars1( &prog_name );
                    } else {
                        SimDirNoEndSlash( dir_index, &working_dir );
                    }
                    /*
                     * Get parameters
                     */
                    SimGetPMParms( i, &prog_args );
                    ReplaceVars1( &prog_args );
                    /*
                     * Append the subdir where the icon file is and the icon file's name.
                     */
                    dir_index = SimGetPMIconInfo( i, &iconfile, &iconindex );
                    if( iconindex == -1 )
                        iconindex = 0;
                    if( dir_index != -1 ) {
                        SimGetDir( dir_index, &tmp );
                        VbufPrepVbuf( &iconfile, &tmp );
                    }
                    /*
                     * Add the new file to the already created PM Group.
                     */
                    ok = linkGroupAddItem( &group, &prog_name, &prog_desc, &prog_args, &working_dir, &iconfile, iconindex );
                }
                ++num_installed;
                StatusAmount( num_installed, num_total_install );
                if( StatusCancelled() ) {
                    break;
                }
            }
            StatusAmount( num_total_install, num_total_install );

            VbufFree( &tmp );
            VbufFree( &prog_args );
            VbufFree( &working_dir );
            VbufFree( &iconfile );
            VbufFree( &prog_desc );
            VbufFree( &prog_name );
        }
        CoUninitialize();
    }
    VbufFree( &group );
    return( ok );
}

#endif


bool CreatePMInfo( bool uninstall )
/*********************************/
{
#if defined( __NT__ )
    if( GetVariableBoolVal( "IsWin95" ) || GetVariableBoolVal( "IsWinNT40" ) ) {
        return( UseIShellLink( uninstall ) );
    }
#endif
    return( UseDDE( uninstall ) );
}
