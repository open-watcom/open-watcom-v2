/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "pathgrp2.h"

#include "clibext.h"


#define CMPFEXT(e,c)    (e[0] == '.' && stricmp(e + 1, c) == 0)

#define STR(x)  #x

#if defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( __OS2__ ) || defined( __NT__ )
#define INTERNAL_COMMANDS() \
    INTERNAL_COMMAND( BREAK ) \
    INTERNAL_COMMAND( CALL ) \
    INTERNAL_COMMAND( CD ) \
    INTERNAL_COMMAND( CHDIR ) \
    INTERNAL_COMMAND( CLS ) \
    INTERNAL_COMMAND( COMMAND ) \
    INTERNAL_COMMAND( COPY ) \
    INTERNAL_COMMAND( CTTY ) \
    INTERNAL_COMMAND( DATE ) \
    INTERNAL_COMMAND( DEL ) \
    INTERNAL_COMMAND( DIR ) \
    INTERNAL_COMMAND( ECHO ) \
    INTERNAL_COMMAND( ERASE ) \
    INTERNAL_COMMAND( FOR ) \
    INTERNAL_COMMAND( IF ) \
    INTERNAL_COMMAND( MD ) \
    INTERNAL_COMMAND( MKDIR ) \
    INTERNAL_COMMAND( PATH ) \
    INTERNAL_COMMAND( PAUSE ) \
    INTERNAL_COMMAND( PROMPT ) \
    INTERNAL_COMMAND( RD ) \
    INTERNAL_COMMAND( REM ) \
    INTERNAL_COMMAND( REN ) \
    INTERNAL_COMMAND( RENAME ) \
    INTERNAL_COMMAND( RMDIR ) \
    INTERNAL_COMMAND( SET ) \
    INTERNAL_COMMAND( TIME ) \
    INTERNAL_COMMAND( TYPE ) \
    INTERNAL_COMMAND( VER ) \
    INTERNAL_COMMAND( VERIFY ) \
    INTERNAL_COMMAND( VOL )
#else
#define NO_INTERNAL_COMMANDS
#endif

#ifndef NO_INTERNAL_COMMANDS
enum {
    #define INTERNAL_COMMAND(a) DUMMY_INTERNAL_COMMAND_##a,
        INTERNAL_COMMANDS()
    #undef INTERNAL_COMMAND
    MAX_INTERNAL_COMMANDS
};

static const char * const _NEAR InternalCommands[MAX_INTERNAL_COMMANDS] = {
    #define INTERNAL_COMMAND(a) (const char *)STR( a ),
        INTERNAL_COMMANDS()
    #undef INTERNAL_COMMAND
};
#endif

#if defined( __DOS__ ) || defined( __WINDOWS__ )
#define EXE_EXTENSIONS() \
    EXE_EXTENSION( bat ) \
    EXE_EXTENSION( com ) \
    EXE_EXTENSION( exe )
#elif defined( __NT__ )
#define EXE_EXTENSIONS() \
    EXE_EXTENSION( bat ) \
    EXE_EXTENSION( exe )
#elif defined( __OS2__ )
#define EXE_EXTENSIONS() \
    EXE_EXTENSION( cmd ) \
    EXE_EXTENSION( exe )
#else
#define NO_EXE_EXTENSIONS
#endif

#ifndef NO_EXE_EXTENSIONS
enum {
    #define EXE_EXTENSION(a) DUMMY_EXE_EXTENSION_##a,
        EXE_EXTENSIONS()
    #undef EXE_EXTENSION
    MAX_EXE_EXTENSIONS
};

static const char * const _NEAR ExeExtensions[MAX_EXE_EXTENSIONS] = {
    #define EXE_EXTENSION(a) (const char *)STR( a ),
        EXE_EXTENSIONS()
    #undef EXE_EXTENSION
};
#endif

void GetSpawnCommandLine( char *path, const char *cmdl, cmd_struct *cmds )
{
    const char  *cmd;
    char        full[FILENAME_MAX];
    pgroup2     pg;
#if !defined( NO_INTERNAL_COMMANDS ) || !defined( NO_EXE_EXTENSIONS )
    int         i;
    bool        is_internal = false;
#endif

    cmd = GetNextWord1( cmdl, full );
    strcpy( path, full );
    _splitpath2( full, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] != '\0' ) {
        if( pg.drive[0] == '\0' && pg.dir[0] == '\0' ) {
            GetFromEnv( full, path );
        }
#if !defined( NO_INTERNAL_COMMANDS ) || !defined( NO_EXE_EXTENSIONS )
    } else {
  #if !defined( NO_INTERNAL_COMMANDS )
        if( pg.drive[0] == '\0' && pg.dir[0] == '\0' ) {
            for( i = 0; i < MAX_INTERNAL_COMMANDS; i++ ) {
                if( stricmp( full, InternalCommands[i] ) == 0 ) {
                    is_internal = true;
                    break;
                }
            }
        }
  #endif
  #if !defined( NO_EXE_EXTENSIONS )
        if( !is_internal ) {
            for( i = 0; i < MAX_EXE_EXTENSIONS; i++ ) {
                _makepath( full, pg.drive, pg.dir, pg.fname, ExeExtensions[i] );
                GetFromEnv( full, path );
                if( path[0] != '\0' ) {
                    break;
                }
            }
        }
  #endif
#endif
    }
#if !defined( NO_EXE_EXTENSIONS )
    _splitpath2( full, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
#endif
#if !defined( NO_EXE_EXTENSIONS ) || !defined( NO_INTERNAL_COMMANDS )
  #if defined( NO_EXE_EXTENSIONS )
    if( is_internal ) {
  #else
    if( CMPFEXT( pg.ext, ExeExtensions[0] ) || is_internal ) {
  #endif
        strcpy( path, Comspec );
        strcpy( cmds->cmd, "/c " );
        strcat( cmds->cmd, cmdl );
    } else {
#endif
        strcpy( cmds->cmd, cmd );
#if !defined( NO_EXE_EXTENSIONS ) || !defined( NO_INTERNAL_COMMANDS )
    }
#endif
    cmds->len = strlen( cmds->cmd );
    cmds->cmd[cmds->len] = '\r';
}
