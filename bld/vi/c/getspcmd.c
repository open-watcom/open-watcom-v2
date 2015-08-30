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


#include "vi.h"
#include "posix.h"

#include "clibext.h"


extern char  _NEAR * _NEAR ExeExtensions[];
extern int ExeExtensionCount;
extern char _NEAR * _NEAR InternalCommands[];
extern int InternalCommandCount;

void GetSpawnCommandLine( char *path, const char *cmdl, cmd_struct *cmds )
{
    char        orgcmd[MAX_INPUT_LINE];
    char        cmd[MAX_INPUT_LINE];
    char        full[FILENAME_MAX];
    char        drive[_MAX_DRIVE], directory[_MAX_DIR], name[_MAX_FNAME];
    char        ext[_MAX_EXT];
    int         i;
    bool        is_internal;

    strcpy( cmd, cmdl );
    strcpy( orgcmd, cmd );
    NextWord1( cmd, full );
    is_internal = false;

    strcpy( path, full );
    _splitpath( full, drive, directory, name, ext );
    if( ext[0] != 0 ) {
        if( drive[0] == 0 && directory[0] == 0 ) {
            GetFromEnv( full, path );
        }
    } else {
        if( drive[0] == 0 && directory[0] == 0 ) {
            for( i = 0; i < InternalCommandCount; i++ ) {
                if( !stricmp( full, InternalCommands[i] ) ) {
                    is_internal = true;
                    break;
                }
            }
        }
        if( !is_internal ) {
            for( i = 0; i < ExeExtensionCount; i++ ) {
                _makepath( full, drive, directory, name, ExeExtensions[i] );
                GetFromEnv( full, path );
                if( path[0] != 0 ) {
                    break;
                }
            }
        }
    }
    RemoveLeadingSpaces( cmd );
    _splitpath( full, drive, directory, name, ext );
    if( !stricmp( ext, ExeExtensions[0] ) || is_internal ) {
        strcpy( path, Comspec );
        strcpy( cmds->cmd, "/c " );
        strcat( cmds->cmd, orgcmd );
    } else {
        strcpy( cmds->cmd, cmd );
    }
    cmds->len = strlen( cmds->cmd );
    cmds->cmd[cmds->len] = 0x0d;
}
