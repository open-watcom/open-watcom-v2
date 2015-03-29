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
* Description:  Setup program mainline.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include <string.h>
#include "wio.h"
#include "gui.h"
#include "setup.h"
#include "setupinf.h"
#include "setupio.h"
#include "gendlg.h"
#include "genvbl.h"
#include "utils.h"
#include "clibext.h"

extern void DoSpawn( when_time );
extern void SetupTitle();
extern void DeleteObsoleteFiles();
extern void ResetDiskInfo( void );

extern bool     SkipDialogs;
extern bool     CancelSetup;
extern vhandle  UnInstall;

int IsPatch = 0;

typedef enum {
    Stack_Push,
    Stack_Pop,
    Stack_IsEmpty
} DIR_PARAM_STACK_OPS;


static bool SetupOperations( void )
/*********************************/
{
    bool                uninstall;

    // are we doing an UnInstall?
    uninstall = VarGetIntVal( UnInstall );

    if( GetVariableIntVal( "IsUpgrade" ) == 1 ) {
        if( !CheckUpgrade() ) {
            return( false );
        }
    }

    DoSpawn( WHEN_BEFORE );
#ifdef PATCH
    if( GetVariableIntVal( "Patch" ) == 1 ) {
        IsPatch = 1;
        if( !PatchFiles() ) {
            return( false );
        }
    }
#endif

    DeleteObsoleteFiles();

    // Copy the files
    if( GetVariableIntVal( "DoCopyFiles" ) == 1 ) {
        if( !CopyAllFiles() ) {
            return( false );
        }
    }
    DoSpawn( WHEN_AFTER );

    // Modify AUTOEXEC.BAT and CONFIG.SYS
    if( GetVariableIntVal( "DoModifyAuto" ) == 1 ) {
        if( !ModifyStartup( uninstall ) ) {
            return( false );
        }
    }

    // Perform file associations
    if( GetVariableIntVal( "DoFileAssociations" ) == 1 ) {
        if( !ModifyAssociations( uninstall ) ) {
            return( false );
        }
    }

    // Generate batch file
    if( GetVariableIntVal( "GenerateBatchFile" ) == 1 ) {
        if( !GenerateBatchFile( uninstall ) ) {
            return( false );
        }
    }

    // Create program group (folder)
    if( GetVariableIntVal( "DoCreateIcons" ) == 1 ||
        GetVariableIntVal( "DoCreateHelpIcons" ) == 1 ) {
        if( !ModifyEnvironment( uninstall ) ) {
            return( false );
        }
    }

    // Add uninstaller to Add/Remove Programs
    if( GetVariableIntVal( "DoUninstall" ) == 1 ) {
        if( !ModifyUninstall( uninstall ) ) {
            return( false );
        }
    }

    DoSpawn( WHEN_END );

    return( true );
}

#define MAX_DIAGS 20

#ifdef __WINDOWS__
bool CheckForSetup32( int argc, char **argv )
{
    DWORD       version = GetVersion();
    int         winver;
    char        *buff;
    size_t      mem_needed;
    int         i;
    char        new_exe[_MAX_PATH];
    char        drive[_MAX_DRIVE];
    char        path[_MAX_PATH];
    char        name[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        *os;

    winver = LOBYTE( LOWORD( version ) ) * 100 + HIBYTE( LOWORD( version ) );
    os = getenv( "OS" );
    if( winver >= 390 || ( os != NULL && stricmp( os, "Windows_NT" ) == 0 ) ) {
        _splitpath( argv[0], drive, path, name, ext );
        _makepath( new_exe, drive, path, "SETUP32", ext );
        mem_needed = strlen( new_exe );
        for( i = 1; i < argc; i++ ) {
            mem_needed += strlen( argv[i] ); // command line arguments
        }
        mem_needed += i; // spaces between arguments + terminating null
        buff = malloc( mem_needed );
        if( buff == NULL ) {
            return( false );
        }
        strcpy( buff, new_exe );
        if( access( buff, F_OK ) == 0 ) {
            for( i = 1; i < argc; i++ ) {
                strcat( buff, " " );
                strcat( buff, argv[i] );
            }
            WinExec( buff, SW_SHOW );
            return( true );
        }
    }
    return( false );
}
#endif

#if defined( __NT__ ) && !defined( _M_X64 )
static bool CheckWin95Uninstall( int argc, char **argv )
{
// The Windows 95 version of setup gets installed as the
// uninstall utility. So that it can erase itself, the setup
// program gets copied to the Windows directory, and run from
// there. The version in the Windows directory gets erased by
// the WININIT program.

    size_t              len;
    char                buff[2 * _MAX_PATH];
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                name[_MAX_FNAME];

    if( argc > 1 && stricmp( argv[ 1 ], "-u" ) == 0 ) {
        // copy setup program to unsetup.exe in system directory
        GetWindowsDirectory( buff, _MAX_PATH );
        strcat( buff, "\\UnSetup.exe" );
        if( DoCopyFile( argv[0], buff, false ) == CFE_NOERROR ) {
            // add entry to wininit.ini to erase unsetup.exe
            WritePrivateProfileString( "rename", "NUL", buff, "wininit.ini" );
            // setup.inf should be in same directory as setup.exe
            len = strlen( buff );
            buff[len] = ' ';
            buff[len + 1] = '\"';
            _splitpath( argv[0], drive, dir, name, NULL );
            _makepath( &buff[len + 2], drive, dir, name, "inf" );
            strcat( buff, "\"" );
            // execute unsetup
            WinExec( buff, SW_SHOW );
            return( true );
        }
    }
    return( false );
}

static bool CheckWow64( void )
{
    DWORD   version = GetVersion();
    if( version < 0x80000000 && LOBYTE( LOWORD( version ) ) >= 5 && IsWOW64() ) {
        char *msg = "You are using 32-bit installer on 64-bit host\n"
                    "It is recommended to use 64-bit installer\n"
                    "\ton 64-bit host\n"
                    "Press OK button to continue with installation\n"
                    "\tor Cancel button to abort it\n";

        InitGlobalVarList();
        SetVariableByName( "IDS_USEINST64BIT", "%s");
        if( MsgBox( NULL, "IDS_USEINST64BIT", GUI_OK_CANCEL, msg ) != GUI_RET_OK ) {
            /* return true to terminate installer */
            return( true );
        }
    }
    return( false );
}
#endif


bool DirParamStack( char **inf_name, char **src_path, DIR_PARAM_STACK_OPS function)
/*********************************************************************************/
{
    // Not really a stack; stores only one "node"

    static char *       old_inf_name = NULL;
    static char *       old_src_path = NULL;

    if( function == Stack_Push ) {
        // Push values on "stack"
        old_inf_name = *inf_name;
        old_src_path = *src_path;

        *inf_name = GUIMemAlloc( _MAX_PATH );
        if( *inf_name == NULL ) {
            return( false );
        }
        *src_path = GUIMemAlloc( _MAX_PATH );
        if( *src_path == NULL ) {
            GUIMemFree( *inf_name );
            return( false );
        }
        return( true );
    } else if( function == Stack_Pop ) {
        // Pop
        GUIMemFree( *inf_name );
        GUIMemFree( *src_path );
        *inf_name = old_inf_name;
        *src_path = old_src_path;
        old_inf_name = NULL;
        old_src_path = NULL;
        return( true );
    } else {
        // IsEmpty
        return( old_inf_name == NULL );
    }
}

bool DoMainLoop( dlg_state * state )
/**********************************/
{
    const char          *diag_list[MAX_DIAGS + 1];
    const char          *diags;
    bool                got_disk_sizes = false;
    int                 i;
    char                newdst[_MAX_PATH];
    char                *next;
    bool                ret = false;

    SetupTitle();

    // display initial dialog
    diags = GetVariableStrVal( "DialogOrder" );
    if( stricmp( diags, "" ) == 0 ) {
        diags = "Welcome";
    }
    i = 0;
    for( ;; ) {
        diag_list[i] = diags;
        next = strchr( diags, ',' );
        if( next == NULL ) break;
        *next = '\0';
        diags = next + 1;
        ++i;
    }
    diag_list[i + 1] = NULL;
    /* process installation dialogs */

    i = 0;
    for( ;; ) {
        if( i < 0 ) break;
        if( diag_list[i] == NULL ) {
            if( GetVariableIntVal( "DoCopyFiles" ) == 1 ) {
                if( !CheckDrive( true ) ) {
                    i = 0;
                }
            }
            if( GetVariableByName( "SetupPath" ) != NO_VAR ) {
                ret = true;
                break;
            }
            if( diag_list[i] == NULL ) {
                StatusShow( true );
                ret = SetupOperations();
                StatusShow( false );
                if( ret )
                    DoDialog( "Finished" );
                break;
            }
        }
        if( stricmp( diag_list[i], "GetDiskSizesHere" ) == 0 ) {
            if( *state == DLG_NEXT ) {
                SimSetNeedGetDiskSizes();
                ResetDiskInfo();
                got_disk_sizes = true;
            }
        } else {
            *state = DoDialog( diag_list[i] );
            GUIWndDirty( NULL );
            StatusCancelled();
        }
        if( *state == DLG_CAN ) {
            if( MsgBox( NULL, "IDS_QUERYABORT", GUI_YES_NO ) == GUI_RET_YES ) {
                CancelSetup = true;
                break;
            }
        } else if( *state == DLG_DONE ) {
            CancelSetup = true;
            break;
        } else if( *state == DLG_NEXT && stricmp( diag_list[i], "DstDir" ) == 0 ) {
            strcpy( newdst, GetVariableStrVal( "DstDir" ) );
            RemoveDirSep( newdst );
            SetVariableByName( "DstDir", newdst );
        }
        if( got_disk_sizes ) {
            if( !CheckDrive( false ) ) {
                break;
            }
        }
        if( *state == DLG_SAME ) {
            /* nothing */
        } else if( *state == DLG_NEXT || *state == DLG_SKIP ) {
            if( SkipDialogs ) {
                ++i;
            } else {
                for( ;; ) {
                    ++i;
                    if( diag_list[i] == NULL ) break;
                    if( CheckDialog( diag_list[i] ) ) break;
                }
            }
        } else if( *state == DLG_PREV ) {
            for( ;; ) {
                --i;
                if( i < 0 ) break;
                if( CheckDialog( diag_list[i] ) ) break;
            }
        } else if( *state == DLG_START ) {
            i = 0;
        }
    } /* for */

    return( ret );
}

void GUImain( void )
/******************/
{
    int                 argc = 0;
    char                **argv = NULL;
    char                *dir;
    char                *drive;
    char                *inf_name;
    char                *src_path;
    char                *arc_name;
    char                *new_inf;
    char                current_dir[_MAX_PATH];
    bool                ret = false;
    dlg_state           state;

    GUIMemOpen();
    GUIGetArgs( &argv, &argc );
#if defined( __NT__ ) && !defined( _M_X64 )
    if( CheckWin95Uninstall( argc, argv ) ) return;
    if( CheckWow64() ) return;
#endif
#ifdef __WINDOWS__
    if( CheckForSetup32( argc, argv ) ) return;
#endif

    // initialize paths and env. vbls.

    if( !SetupPreInit() ) return;
    if( !GetDirParams( argc, argv, &inf_name, &src_path, &arc_name ) ) return;
    if( !SetupInit() ) return;
    GUIDrainEvents();   // push things along
    FileInit( arc_name );
    InitGlobalVarList();
    strcpy( current_dir, src_path );
    ConcatDirSep( current_dir );
    while( InitInfo( inf_name, src_path ) ) {

        ret = DoMainLoop( &state );

        if( state == DLG_DONE ) break;
//        if( CancelSetup || !ret ) break;
        if( CancelSetup ) break;
//        if( !ret ) break;

        // look for another SETUP.INF
        if( GetVariableByName( "SetupPath" ) == NO_VAR ) {
            if( !DirParamStack( &inf_name, &src_path, Stack_IsEmpty ) ) {  // "IsEmpty"?
                DirParamStack( &inf_name, &src_path, Stack_Pop ); // "Pop"
                CloseDownMessage( ret );
                CancelSetup = false;
                ret = true;
            } else {
                CloseDownMessage( ret );
                break;
            }
        } else {
            if( GetVariableIntVal( "IsMultiInstall" ) ) {
                // push current script on stack
                DirParamStack( &inf_name, &src_path, Stack_Push ); // "Push"
            }
            new_inf = GUIMemAlloc( _MAX_PATH );
            drive = GUIMemAlloc( _MAX_DRIVE );
            dir = GUIMemAlloc( _MAX_PATH );
            if( new_inf == NULL || drive == NULL || dir == NULL ) {
                GUIMemFree( new_inf );
                GUIMemFree( drive );
                GUIMemFree( dir );
                break;
            }
            // construct new path relative to previous
            ReplaceVars( new_inf, _MAX_PATH, GetVariableStrVal( "SetupPath" ) );
            _splitpath( current_dir, drive, dir, NULL, NULL );
            _makepath( inf_name, drive, dir, new_inf, NULL );

            _splitpath( inf_name, drive, dir, NULL, NULL );
            _makepath( src_path, drive, dir, NULL, NULL );
            RemoveDirSep( src_path );
//            strcpy( current_dir, src_path );
//            ConcatDirSep( current_dir );

            GUIMemFree( new_inf );
            GUIMemFree( drive );
            GUIMemFree( dir );
        } /* if */

        FreeGlobalVarList( false );
        FreeDefaultDialogs();
        FreeAllStructs();
        ConfigModified = false;
    } /* while */


    FileFini();
    FreeGlobalVarList( true );
    FreeDefaultDialogs();
    FreeAllStructs();
    FreeDirParams( &inf_name, &src_path, &arc_name );
    CloseDownProgram();
}

