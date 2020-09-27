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
#include "setup.h"
#include "setupinf.h"
#include "setupio.h"
#include "gendlg.h"
#include "genvbl.h"
#include "utils.h"
#include "guiutil.h"
#include "guistats.h"
#include "guiutils.h"

#include "clibext.h"


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
    uninstall = VarGetBoolVal( UnInstall );

    if( GetVariableBoolVal( "IsUpgrade" ) ) {
        if( !CheckUpgrade() ) {
            return( false );
        }
    }

    DoSpawn( WHEN_BEFORE );

    DeleteObsoleteFiles();

    // Copy the files
    if( GetVariableBoolVal( "DoCopyFiles" ) ) {
        if( !CopyAllFiles() ) {
            return( false );
        }
    }
    DoSpawn( WHEN_AFTER );

    // Modify AUTOEXEC.BAT and CONFIG.SYS
    if( GetVariableBoolVal( "DoModifyAuto" ) ) {
        if( !ModifyStartup( uninstall ) ) {
            return( false );
        }
    }

    // Perform file associations
    if( GetVariableBoolVal( "DoFileAssociations" ) ) {
        if( !ModifyAssociations( uninstall ) ) {
            return( false );
        }
    }

    // Generate batch file
    if( GetVariableBoolVal( "GenerateBatchFile" ) ) {
        if( !GenerateBatchFile( uninstall ) ) {
            return( false );
        }
    }

    // Create program group (folder)
    if( GetVariableBoolVal( "DoCreateIcons" ) || GetVariableBoolVal( "DoCreateHelpIcons" ) ) {
        if( !ModifyEnvironment( uninstall ) ) {
            return( false );
        }
    }

    // Add uninstaller to Add/Remove Programs
    if( GetVariableBoolVal( "DoUninstall" ) ) {
        if( !ModifyUninstall( uninstall ) ) {
            return( false );
        }
    }

    DoSpawn( WHEN_END );

    return( true );
}

#define MAX_DIAGS 20

static bool DirParamStack( VBUF *inf_name, VBUF *src_path, DIR_PARAM_STACK_OPS function )
/***************************************************************************************/
{
    // Not really a stack; stores only one "node"

    static VBUF old_inf_name = VBUF_INIT_STRUCT;
    static VBUF old_src_path = VBUF_INIT_STRUCT;
    static bool isempty = true;

    if( function == Stack_Push ) {
        // Push values on "stack"
        if( !isempty ) {
            isempty = false;
            VbufFree( &old_inf_name );
            VbufFree( &old_src_path );
        }
        VbufSetVbuf( &old_inf_name, inf_name );
        VbufSetVbuf( &old_src_path, src_path );
        return( true );
    } else if( function == Stack_Pop ) {
        // Pop
        VbufSetVbuf( inf_name, &old_inf_name );
        VbufSetVbuf( src_path, &old_src_path );
        if( !isempty ) {
            isempty = true;
            VbufFree( &old_inf_name );
            VbufFree( &old_src_path );
        }
        return( true );
    } else {
        // IsEmpty
        return( isempty );
    }
}

static bool DoMainLoop( dlg_state *state )
/****************************************/
{
    char                *diag_list[MAX_DIAGS + 1];
    const char          *p;
    char                *list;
    char                *diags;
    bool                got_disk_sizes = false;
    int                 i;
    VBUF                temp;
    char                *next;
    bool                ret = false;

    SetupTitle();

    // display initial dialog
    p = GetVariableStrVal( "DialogOrder" );
    if( p[0] == '\0' ) {
        p = "Welcome";
    }
    i = 0;
    diags = list = GUIStrDup( p, NULL );
    for( ;; ) {
        diag_list[i] = diags;
        next = strchr( diags, ',' );
        if( next == NULL )
            break;
        *next = '\0';
        diags = next + 1;
        ++i;
    }
    diag_list[i + 1] = NULL;
    /* process installation dialogs */

    VbufInit( &temp );
    *state = DLG_NEXT;
    i = 0;
    for( ;; ) {
        if( i < 0 )
            break;
        if( diag_list[i] == NULL ) {
            if( GetVariableBoolVal( "DoCopyFiles" ) ) {
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
        if( *state == DLG_CANCEL ) {
            if( MsgBox( NULL, "IDS_QUERYABORT", GUI_YES_NO ) == GUI_RET_YES ) {
                CancelSetup = true;
                break;
            }
        } else if( *state == DLG_DONE ) {
            CancelSetup = true;
            break;
        } else if( *state == DLG_NEXT && stricmp( diag_list[i], "DstDir" ) == 0 ) {
            VbufSetStr( &temp, GetVariableStrVal( "DstDir" ) );
            VbufRemEndDirSep( &temp );
            SetVariableByName_vbuf( "DstDir", &temp );
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
                    if( diag_list[i] == NULL )
                        break;
                    if( CheckDialog( diag_list[i] ) ) {
                        break;
                    }
                }
            }
        } else if( *state == DLG_PREV ) {
            for( ;; ) {
                --i;
                if( i < 0 )
                    break;
                if( CheckDialog( diag_list[i] ) ) {
                    break;
                }
            }
        } else if( *state == DLG_START ) {
            i = 0;
        }
    } /* for */
    VbufFree( &temp );
    GUIMemFree( list );

    return( ret );
}

void GUImain( void )
/******************/
{
    int                 argc = 0;
    char                **argv = NULL;
    VBUF                dir;
    VBUF                drive;
    VBUF                inf_name;
    VBUF                src_path;
    VBUF                arc_name;
    VBUF                new_inf;
    VBUF                current_dir;
    bool                ok;
    dlg_state           state;

    GUIGetArgs( &argv, &argc );

    // initialize paths and env. vbls.

    if( SetupPreInit( argc, argv ) ) {
        SetupInit();
        InitGlobalVarList();
        VbufInit( &inf_name );
        VbufInit( &src_path );
        VbufInit( &arc_name );
        if( GetDirParams( argc, argv, &inf_name, &src_path, &arc_name ) ) {
            StatusInit();
            GUIDrainEvents();   // push things along
            FileInit( &arc_name );
            VbufInit( &new_inf );
            VbufInit( &current_dir );
            VbufInit( &drive );
            VbufInit( &dir );
            VbufConcVbuf( &current_dir, &src_path );
            VbufAddDirSep( &current_dir );
            ok = false;
            while( !ok && InitInfo( &inf_name, &src_path ) ) {

                ok = DoMainLoop( &state );

                if( state == DLG_DONE ) {
                    ok = true;
                } else if( CancelSetup ) {
                    ok = true;
                } else if( GetVariableByName( "SetupPath" ) == NO_VAR ) {
                    // look for another SETUP.INF
                    if( !DirParamStack( &inf_name, &src_path, Stack_IsEmpty ) ) {   // "IsEmpty"?
                        // pop script from stack
                        DirParamStack( &inf_name, &src_path, Stack_Pop );           // "Pop"
                        CloseDownMessage( ok );
                        // process next script
                        CancelSetup = false;
                        ok = false;
                    } else {
                        CloseDownMessage( ok );
                        // finish
                        ok = true;
                    }
                } else {
                    if( GetVariableBoolVal( "IsMultiInstall" ) ) {
                        // push current script on stack
                        DirParamStack( &inf_name, &src_path, Stack_Push );          // "Push"
                    }
                    // construct new path relative to previous
                    ReplaceVars( &new_inf, GetVariableStrVal( "SetupPath" ) );
                    VbufSplitpath( &current_dir, &drive, &dir, NULL, NULL );
                    VbufMakepath( &inf_name, &drive, &dir, &new_inf, NULL );
                    VbufSplitpath( &inf_name, &drive, &dir, NULL, NULL );
                    VbufMakepath( &src_path, &drive, &dir, NULL, NULL );
                    VbufRemEndDirSep( &src_path );
                }
                FreeDefaultDialogs();
                FreeAllStructs();
                FreeGlobalVarList( false );
                ConfigModified = false;
            } /* while */
            VbufFree( &dir );
            VbufFree( &drive );
            VbufFree( &current_dir );
            VbufFree( &new_inf );
            FreeDefaultDialogs();
            FreeAllStructs();
            FileFini();
            StatusFini();
        }
        FreeDirParams();
        VbufFree( &arc_name );
        VbufFree( &src_path );
        VbufFree( &inf_name );
        FreeGlobalVarList( true );
        SetupFini();
    }
}
