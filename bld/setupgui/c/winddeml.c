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
* Description:  Functions for adding groups/icons to the Windows shell.
*               Supports both Windows 3.x/NT 3.x and Win9x/NT 4 shells.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define INCLUDE_DDEML_H
#include <windows.h>
#include <ddeml.h>

#include "gui.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"

#define DDE_WAITTIME    3000

static bool SendCommand( DWORD ddeinst, HCONV hconv, char *buff )
/***************************************************************/
{
    HDDEDATA    hData, hrc;

    // Note data handle is freed by call to DdeClientTransaction
    hData = DdeCreateDataHandle( ddeinst, (LPBYTE)buff, strlen( buff ) + 1,
                                 0, (HSZ)NULL, 0L, 0L );
    if( hData == (HDDEDATA)NULL ) {
        return( FALSE );
    }

    hrc = DdeClientTransaction( (LPBYTE) hData, -1, hconv, (HSZ)NULL, 0L,
                                XTYP_EXECUTE, DDE_WAITTIME, NULL );
    if( hrc == (HDDEDATA)NULL ) {
        return( FALSE );
    }

    return( TRUE );
}

static bool UseDDE( bool uninstall )
/**********************************/
{
    WORD                dir_index, icon_number, version;
    int                 i, num_icons, num_groups;
    int                 num_installed, num_total_install;
    int                 len;
    DWORD               temp;
    bool                ok;
    char                prog_name[_MAX_PATH], prog_desc[_MAX_PATH];
    char                icon_name[_MAX_PATH], working_dir[_MAX_PATH];
    char                buff[_MAX_PATH], t1[_MAX_PATH], t2[_MAX_PATH];
    HWND                hwnd_pm;
    DWORD               ddeinst = 0; // Important that this is initially 0
    UINT                rc;
    HSZ                 happ, htopic;
    HCONV               hconv;
    char                progman[] = "PROGMAN";

    SimGetPMGroup( t1 );
    if( t1[ 0 ] == '\0' ) {
        return( TRUE );
    }

    // Initiate a conversation with the Program Manager.
    // NOTE: No callback provided since we only issue execute commands
    rc = DdeInitialize( &ddeinst, NULL, APPCMD_CLIENTONLY, 0L );
    if( rc != 0 ) {
        return( FALSE );
    }

    happ = DdeCreateStringHandle( ddeinst, progman, CP_WINANSI );
    htopic = DdeCreateStringHandle( ddeinst, progman, CP_WINANSI );

    hconv = DdeConnect( ddeinst, happ, htopic, NULL );
    if( hconv == (HCONV)NULL ) {
        return( FALSE );
    }

    // Disable the Program Manager so that the user can't work with it
    // while we are doing our stuff.
    hwnd_pm = FindWindow( progman, NULL );
    if( hwnd_pm != NULL ) {
        ShowWindow( hwnd_pm, SW_RESTORE );
        EnableWindow( hwnd_pm, FALSE );
    }

    if( uninstall ) {
        // Delete the PM Group box
        num_groups = SimGetNumPMGroups();
        for( i = 0; i < num_groups; i++ ) {
            SimGetPMGroupName( i, t1 );
            if( *t1 != '\0' ) {
                // Delete the PM Group box
                sprintf( buff, "[DeleteGroup(%s)]", t1 );
                ok = SendCommand( ddeinst, hconv, buff );
            }
        }
        goto cleanup;   // I can't believe I'm doing this
    } else {
        // Delete the PM Group box to get rid of stale icons
        // (Don't do this for SQL install, since user may install
        // the server, and then install the client)
        sprintf( buff, "[DeleteGroup(%s)]", t1 );
        ok = SendCommand( ddeinst, hconv, buff );

        // re-Create the PM Group box.
        SimGetPMGroupFileName( t2 );
        if( t2[ 0 ] == '\0' ) {
#if defined( __NT__ )
            sprintf( buff, "[CreateGroup(%s,0)]", t1 );  // create a personal group
#else
            sprintf( buff, "[CreateGroup(%s)]", t1 );
#endif
        } else {
            sprintf( buff, "[CreateGroup(%s,%s)]", t1, t2 );
        }
        ok = SendCommand( ddeinst, hconv, buff );
    }

    // Add the individual PM files to the Group box.
    num_icons = SimGetNumPMProgs();
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
        SimGetPMDesc( i, prog_desc );
        dir_index = SimGetPMProgName( i, prog_name );
        if( strcmp( prog_name, "GROUP" ) == 0 ) {
            // Delete the PM Group box to get rid of stale icons
            sprintf( buff, "[DeleteGroup(%s)]", prog_desc );
            ok = SendCommand( ddeinst, hconv, buff );

            /* creating a new group */
            SimGetPMParms( i, t1 );
            if( t1[ 0 ] == '\0' ) {
#if defined( __NT__ )
                sprintf( buff, "[CreateGroup(%s,0)]", prog_desc );  // create a personal group
#else
                sprintf( buff, "[CreateGroup(%s)]", prog_desc );
#endif
            } else {
                sprintf( buff, "[CreateGroup(%s,%s)]", prog_desc, t1 );
            }
        } else {
            /* adding item to group */
            if( dir_index == SIM_INIT_ERROR ) {
                working_dir[0] = '\0';
                ReplaceVars( t2, prog_name );
                strcpy( prog_name, t2 );
            } else {
                 SimDirNoSlash( dir_index, working_dir );
            }

            // get parameters
            SimGetPMParms( i, t1 );
            if( t1[0] != '\0' ) {
                // add parameters to end of prog_name
                len = strlen( prog_name );
                prog_name[len] = ' ';
                ReplaceVars( &prog_name[len + 1], t1 );
            }

            // Append the subdir where the icon file is and the icon file's name.
            temp = SimGetPMIconInfo( i, icon_name );
            dir_index = LOWORD( temp );
            icon_number = HIWORD( temp );
            if( icon_number == SIM_INIT_ERROR ) {
                icon_number = 0;
            }
            if( dir_index != SIM_INIT_ERROR ) {
                SimGetDir( dir_index, t1 );
                strcat( t1, icon_name );
                strcpy( icon_name, t1 );
            }
            // Add the new file to the already created PM Group.
            version = (WORD)GetVersion();
            if( (LOBYTE( version ) > 3) ||    // Version 3.1 or higher
                (LOBYTE( version ) == 3 && HIBYTE( version ) > 0) ) {
                sprintf( buff, "[ReplaceItem(%s)]", prog_desc );
                SendCommand( ddeinst, hconv, buff );
                sprintf( buff, "[AddItem(%s,%s,%s,%d,-1,-1,%s)]", prog_name,
                               prog_desc, icon_name, icon_number, working_dir );
            } else {
                sprintf( buff, "[AddItem(%s%s,%s,%s,%d)]", working_dir, prog_name,
                               prog_desc, icon_name, icon_number );
            }
        }
        ok = SendCommand( ddeinst, hconv, buff );
        ++num_installed;
        StatusAmount( num_installed, num_total_install );
        if( StatusCancelled() ) break;
    }
    StatusAmount( num_total_install, num_total_install );

cleanup:
    // Terminate the DDE conversation with the Program Manager.
    DdeFreeStringHandle( ddeinst, happ );
    DdeFreeStringHandle( ddeinst, htopic );
    // DdeDisconnect( hconv ); // win95 setup was crashing on ddeuninitialize
    //(only if running from CD)removing this call seems to be an OK workaround
    DdeUninitialize( ddeinst );

    if( hwnd_pm != NULL ) {
        EnableWindow( hwnd_pm, TRUE );
    }
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
void munge_fname( char *buff )
{
    char    *s;
    char    buff2[_MAX_PATH];

    s = buff;
    while( *s ) {
        if( *s == '/' ) {
//    MessageBox(0, buff, 0, 0);
            strcpy( buff2, s + 1 );
            strcpy( s, " - " );     // replace slash by underscore
            strcpy( s + 3, buff2 );
//    MessageBox(0, buff, 0, 0);
        }
        ++s;
    }
}

static void get_group_name( char *buff, char *group )
{
    LPITEMIDLIST        ppidl;

    if( SHGetSpecialFolderLocation( NULL, CSIDL_PROGRAMS, &ppidl ) == NOERROR ) {
        SHGetPathFromIDList( ppidl, buff );
    } else {
        GetWindowsDirectory( buff, _MAX_PATH );
        strcat( buff, "\\Start Menu\\Programs" );
    }
    strcat( buff, "\\" );
    strcat( buff, group );
    munge_fname( buff );
}

static BOOL create_group( char *group )
{
    char                buff[_MAX_PATH];
    int                 rc;

    get_group_name( buff, group );
    rc = mkdir( buff );

    if( rc == -1 && errno != EEXIST ) {
        return( FALSE );
    } else {
        return( TRUE );
    }
}

static void delete_dir( char * dir )
{
    DIR                 *dirp;
    struct dirent       *direntp;
    char                file[_MAX_PATH];

    // Delete contents of directory
    strcpy( file, dir );
    strcat( file, "\\*.*" );
    dirp = opendir( file );
    if( dirp != NULL ) {
        for( ;; ) {
            direntp = readdir( dirp );
            if( direntp == NULL ) {
                break;
            }
            if( direntp->d_attr & 0x10 ) {        /* don't care about directories */
                continue;
            }
            strcpy( file, dir );
            strcat( file, "\\" );
            strcat( file, direntp->d_name );
            remove( file );
        }
        closedir( dirp );
    }
    rmdir( dir );
}

static void remove_group( char *group )
{
    char                buff[_MAX_PATH];

    get_group_name( buff, group );
    delete_dir( buff );
}

static BOOL create_icon( char *group, char *pgm, char *desc,
                         char *arg, char *work, char *icon, int icon_num )
{
    HRESULT             hres;
    IShellLink          *m_link;
    IPersistFile        *p_file;
    WORD                w_link[_MAX_PATH];
    char                link[_MAX_PATH];

    // Determine names of link files
    get_group_name( link, group );
    strcat( link, "\\" );
    strcat( link, desc );
    strcat( link, ".lnk" );
    munge_fname( link );

    MultiByteToWideChar( CP_ACP, 0, link, -1, w_link, _MAX_PATH );

    // Create an IShellLink object and get a pointer to the IShellLink interface
    m_link = NULL;
    hres = CoCreateInstance( &CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                             &IID_IShellLink, (void **)&m_link );
    if( SUCCEEDED( hres ) ) {
        // Query IShellLink for the IPersistFile interface for
        // saving the shortcut in persistent storage.
        p_file = NULL;
        hres = m_link->lpVtbl->QueryInterface( m_link, &IID_IPersistFile,
                                               (void **)&p_file );
        if( SUCCEEDED( hres ) ) {
            // Set the properties of the shortcut
            hres = m_link->lpVtbl->SetPath( m_link, pgm );
            hres = m_link->lpVtbl->SetDescription( m_link, desc );
            hres = m_link->lpVtbl->SetWorkingDirectory( m_link, work );
            hres = m_link->lpVtbl->SetArguments( m_link, arg );
            hres = m_link->lpVtbl->SetIconLocation( m_link, icon, icon_num );

            // Save the shortcut via the IPersistFile::Save member function.
            hres = p_file->lpVtbl->Save( p_file, w_link, TRUE );
            // Release the pointer to IPersistFile.
            p_file->lpVtbl->Release( p_file );
       }
       // Release the pointer to IShellLink.
       m_link->lpVtbl->Release( m_link );
    }
    return( SUCCEEDED( hres ) );
}

static bool UseIShellLink( bool uninstall )
/*****************************************/
{
    WORD                dir_index, icon_number;
    int                 i, num_icons, num_groups;
    int                 num_installed, num_total_install;
    DWORD               temp;
    char                prog_name[_MAX_PATH], prog_desc[_MAX_PATH];
    char                icon_name[_MAX_PATH], working_dir[_MAX_PATH];
    char                group[_MAX_PATH], prog_arg[_MAX_PATH], tmp[_MAX_PATH];
    BOOL                rc;

    if( uninstall ) {
        num_groups = SimGetNumPMGroups();
        for( i = 0; i < num_groups; i++ ) {
            SimGetPMGroupName( i, group );
            if( *group != '\0' ) {
                // Delete the PM Group box
                remove_group( group );
            }
        }
        return( TRUE );
    }

    SimGetPMGroup( group );
    if( group[0] == '\0' ) {
        return( TRUE );
    }

    CoInitialize( NULL );

    // Create the PM Group box.
    if( !create_group( group ) ) {
        CoUninitialize();
        return( FALSE );
    }

    // Add the individual PM files to the Group box.
    num_icons = SimGetNumPMProgs();
    StatusLines( STAT_CREATEPROGRAMFOLDER, "" );
    num_total_install = 0;
    for( i = 0; i < num_icons; i++ ) {
        if( SimCheckPMCondition( i ) ) {
            ++num_total_install;
        }
    }
    num_installed = 0;
    StatusAmount( 0, num_total_install );
    for( i = 0; i < num_icons; i++ ) {
        if( !SimCheckPMCondition( i ) ) {
            continue;
        }
        SimGetPMDesc( i, prog_desc );
        dir_index = SimGetPMProgName( i, prog_name );
        if( strcmp( prog_name, "GROUP" ) == 0 ) {
            /* creating a new group */
            strcpy( group, prog_desc );
            if( !create_group( group ) ) {
                CoUninitialize();
                return( FALSE );
            }
        } else {
            // Adding item to group
            if( dir_index == SIM_INIT_ERROR ) {
                working_dir[ 0 ] = '\0';
                ReplaceVars( tmp, prog_name );
                strcpy( prog_name, tmp );
            } else {
                SimDirNoSlash( dir_index, working_dir );
            }

            // Get parameters
            SimGetPMParms( i, tmp );
            ReplaceVars( prog_arg, tmp );

            // Append the subdir where the icon file is and the icon file's name.
            temp = SimGetPMIconInfo( i, icon_name );
            dir_index = LOWORD( temp );
            icon_number = HIWORD( temp );
            if( icon_number == SIM_INIT_ERROR ) icon_number = 0;
            if( dir_index != SIM_INIT_ERROR ) {
                SimGetDir( dir_index, tmp );
                strcat( tmp, icon_name );
                strcpy( icon_name, tmp );
            }
            // Add the new file to the already created PM Group.
            rc = create_icon( group, prog_name, prog_desc, prog_arg, working_dir,
                              icon_name, icon_number );
            if( rc == FALSE ) {
                CoUninitialize();
                return( FALSE );
            }
        }
        ++num_installed;
        StatusAmount( num_installed, num_total_install );
        if( StatusCancelled() )
            break;
    }
    StatusAmount( num_total_install, num_total_install );

    CoUninitialize();
    return( TRUE );
}

#endif


bool CreatePMInfo( bool uninstall )
/*********************************/
{
#if defined( __NT__ )
    if( GetVariableIntVal( "IsWin95" ) || GetVariableIntVal( "IsWinNT40" ) ) {
        return( UseIShellLink( uninstall ) );
    } else {
        return( UseDDE( uninstall ) );
    }
#else
    return( UseDDE( uninstall ) );
#endif
}

