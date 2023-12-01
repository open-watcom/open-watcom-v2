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
* Description:  Routines to modify registry and configuration files.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include "wio.h"
#include "watcom.h"
#include "setup.h"
#include "guiutil.h"
#include "setupinf.h"
#include "genvbl.h"
#include "gendlg.h"
#include "utils.h"
#include "iopath.h"

#include "clibext.h"


#if defined( __UNIX__ )
    #define SETENV          "export "
    #define SETENV_LEN      7
    #define NAME_PREFIX     "$"
    #define NAME_SUFFIX     ""
    #define IS_SETENVCMD(s) (strncmp(s, SETENV, SETENV_LEN) == 0)
#else
    #define SETENV          "SET "
    #define SETENV_LEN      4
    #define NAME_PREFIX     "%"
    #define NAME_SUFFIX     "%"
    #define IS_SETENVCMD(s) (strnicmp(s, SETENV, SETENV_LEN) == 0)
#endif

typedef enum {
    VAR_SETENV_ASSIGN,  // format is 'SETENV NAME = VALUE' and inf var name is "NAME"
    VAR_ASSIGN_SETENV,  // format is 'SETENV NAME = VALUE' and inf var name is "SETENV NAME"
    VAR_ASSIGN,         // format is 'NAME = VALUE"
    VAR_CMD,            // format is 'NAME VALUE"
    VAR_ERROR,
} var_type;

#if defined( __NT__ )
static struct reg_location {
    HKEY    key;
    bool    key_is_open;
    bool    modify;
} RegLocation[NUM_REG_LOCATIONS];
#endif

#if !defined( __UNIX__ )

#if !defined( __OS2__ )
static char     *WinDotCom = NULL;
#endif

static char GetBootDrive( void )
/*******************************/
{
#ifdef __OS2__
    UCHAR           DataBuf[10];
    APIRET          rc;

    rc = DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, DataBuf, sizeof( DataBuf ) );
    if( rc != 0 ) {
        printf("DosQuerySysInfo error: return code = %ld ",rc);
    }
    return( *(unsigned long *)DataBuf );
#elif defined( __DOS__ )
    union REGS  r;

    r.w.ax = 0x3305;
    intdos( &r, &r );
    return( r.h.dl );
#else
    return( 0 );
#endif
}


static bool GetOldConfigFileDir( VBUF *newauto, const VBUF *drive_path, bool uninstall )
/**************************************************************************************/
{
    VBUF        drive;

    if( uninstall ) {
        VbufInit( &drive );

        VbufSplitpath( drive_path, &drive, NULL, NULL, NULL );
        if( VbufLen( &drive ) == 0 ) {
            VbufFullpath( newauto, drive_path );
            VbufSplitpath( newauto, &drive, NULL, NULL, NULL );
        }
        VbufMakepath( newauto, &drive, NULL, NULL, NULL );

        VbufFree( &drive );
    } else {
        VbufSetStr( newauto, GetVariableStrVal( "DstDir" ) );
    }

    return( true );
}

#endif  // !__UNIX__

static void NoDupPaths( VBUF *old_value, const VBUF *new_value, char list_delim )
/*******************************************************************************/
{
    const char  *dup;
    const char  *look;
    size_t      len;
    const char  *new_start;
    const char  *new_end;
    const char  *value_start;
    VBUF        tmp;

    VbufInit( &tmp );
    new_start = VbufString( new_value );
    for( ;; ) {
        new_end = strchr( new_start, list_delim );
        if( new_end == NULL ) {
            len = strlen( new_start );
        } else {
            len = new_end - new_start;
        }
        value_start = VbufString( old_value );
        for( look = value_start; (dup = stristr( look, new_start, len )) != NULL; look = dup + len ) {
            if( dup[len] == list_delim ) {
                if( dup == value_start ) {
                    // no data to copy
                    value_start = dup + len + 1;
                } else if( dup[-1] == list_delim ) {
                    // copy previous data
                    VbufConcBuffer( &tmp, value_start, dup - value_start );
                    value_start = dup + len + 1;
                }
                // correct "look" pointer by 1 for next lookup after delimiter
                // to synchronize with "value_start" pointer
                look++;
            } else if( dup[len] == '\0' ) {
                if( dup == value_start ) {
                    // no data to copy
                    value_start = dup + len;
                } else if( dup[-1] == list_delim ) {
                    // copy previous data
                    VbufConcBuffer( &tmp, value_start, dup - value_start );
                    value_start = dup + len;
                }
            }
        }
        // set look pointer to the end
        look += strlen( look );
        if( value_start != look ) {
            // copy rest of data not copied to "tmp"
            VbufConcBuffer( &tmp, value_start, look - value_start );
        }
        // copy result into "old_value"
        VbufSetVbuf( old_value, &tmp );
        if( new_end == NULL )
            break;
        new_start = new_end + 1;
    }
    len = VbufLen( old_value );
    if( len > 0 ) {
        // remove delimiter from the end
        if( VbufString( old_value )[len - 1] == list_delim ) {
            VbufSetLen( old_value, len - 1 );
        }
    }
    VbufFree( &tmp );
}

static void modify_value_list( VBUF *value, const VBUF *new_value, char list_delim, append_mode append, bool uninstall )
/**********************************************************************************************************************/
{
    NoDupPaths( value, new_value, list_delim );
    if( !uninstall ) {
        if( append == AM_AFTER ) {
            VbufConcChr( value, list_delim );
            VbufConcVbuf( value, new_value );
        } else if( append == AM_BEFORE ) {
            VbufPrepChr( value, list_delim );
            VbufPrepVbuf( value, new_value );
        } else {
            VbufSetVbuf( value, new_value );
        }
    }
}

static bool output_line( VBUF *vbuf, var_type vt, const VBUF *name, const VBUF *value )
/*************************************************************************************/
{
    VbufRewind( vbuf );
    if( VbufLen( value ) > 0 ) {
        switch( vt ) {
        case VAR_SETENV_ASSIGN:
            VbufConcStr( vbuf, SETENV );
            VbufConcVbuf( vbuf, name );
            VbufConcChr( vbuf, '=' );
            VbufConcVbuf( vbuf, value );
            break;
        case VAR_ASSIGN:
        case VAR_ASSIGN_SETENV:
            VbufConcVbuf( vbuf, name );
            VbufConcChr( vbuf, '=' );
            VbufConcVbuf( vbuf, value );
            break;
        case VAR_CMD:
            VbufConcVbuf( vbuf, name );
            VbufConcChr( vbuf, ' ' );
            VbufConcVbuf( vbuf, value );
            break;
        case VAR_ERROR:
        default:
            break;
        }
    }
    return( VbufLen( vbuf ) > 0 );
}

static void modify_value_list_libpath( VBUF *val_before, VBUF *val_after,
                const VBUF *new_value, char list_delim, append_mode append )
/**************************************************************************/
{
    if( append == AM_AFTER ) {
        NoDupPaths( val_before, new_value, list_delim );
        NoDupPaths( val_after, new_value, list_delim );
        VbufConcChr( val_after, list_delim );
        VbufConcVbuf( val_after, new_value );
    } else if( append == AM_BEFORE ) {
        NoDupPaths( val_before, new_value, list_delim );
        VbufPrepChr( val_before, list_delim );
        VbufPrepVbuf( val_before, new_value );
        NoDupPaths( val_after, new_value, list_delim );
    } else {
        VbufSetVbuf( val_before, new_value );
        VbufRewind( val_after );
    }
}

#if !defined( __UNIX__ )
static var_type parse_line( char *line, VBUF *name, VBUF *value, var_type vt_setenv )
/***********************************************************************************/
{
    char        c;
    var_type    vt;
    char        *s;

    // parsed line formats
    // SETENV NAME = VALUE
    // NAME = VALUE
    // NAME VALUE
    VbufRewind( name );
    SKIP_SPACES( line );
    if( IS_SETENVCMD( line ) ) {
        line += SETENV_LEN;
        SKIP_SPACES( line );
        if( vt_setenv == VAR_ASSIGN_SETENV ) {
            VbufConcStr( name, SETENV );
        }
        vt = vt_setenv;
    } else {
        vt = VAR_ASSIGN;
    }
    s = line;
    for( ; (c = *line) != '\0'; line++ ) {
        if( isspace( c ) || c == '=' ) {
            break;
        }
    }
    VbufConcBuffer( name, s, line - s );
    VbufRewind( value );
    if( *line == '=' ) {
        SKIP_CHAR_SPACES( line );
        VbufConcStr( value, line );
    } else if( vt == vt_setenv ) {
        vt = VAR_ERROR;
    } else {
        SKIP_SPACES( line );
        VbufConcStr( value, line );
        vt = VAR_CMD;
    }
    return( vt );
}
#endif

static var_type getEnvironVarType( const VBUF *env_var )
/******************************************************/
{
    var_type        vt;

#ifdef __UNIX__
    /* unused parameters */ (void)env_var;
#endif

    vt = VAR_SETENV_ASSIGN;
#ifndef __UNIX__
  #ifndef __OS2__
    if( GetVariableBoolVal( "IsOS2DosBox" ) ) {
  #endif
        // OS/2
        if( VbufCompStr( env_var, "LIBPATH", true ) == 0 ) {
            vt = VAR_ASSIGN;
        }
  #ifndef __OS2__
    } else {
        // DOS, WINDOWS, NT
        if( VbufCompStr( env_var, "PATH", true ) == 0 ) {
            vt = VAR_CMD;
        }
    }
  #endif
#endif
    return( vt );
}

#if !defined( __UNIX__ )
static void CheckEnvironmentLine( char *line, int num_env, bool *found_env, bool uninstall )
/******************************************************************************************/
{
    int                 i;
    append_mode         append;
    var_type            vt;
    bool                modified;
    VBUF                line_var;
    VBUF                line_val;
    VBUF                next_var;
    VBUF                next_val;

    VbufInit( &line_var );
    VbufInit( &line_val );
    vt = parse_line( line, &line_var, &line_val, VAR_SETENV_ASSIGN );
    switch( vt ) {
    case VAR_SETENV_ASSIGN:
        break;
    case VAR_ASSIGN:
        if( VbufCompStr( &line_var, "LIBPATH", true ) == 0 )
            break;
        // fall down
    case VAR_CMD:
        if( VbufCompStr( &line_var, "PATH", true ) == 0 )
            break;
        // fall down
    default:
        VbufFree( &line_val );
        VbufFree( &line_var );
        return;
    }
    VbufInit( &next_var );
    VbufInit( &next_val );
    modified = false;
    for( i = 0; i < num_env; ++i ) {
        if( found_env[i] || !uninstall && !SimCheckEnvironmentCondition( i ) )
            continue;
        append = SimGetEnvironmentStrings( i, &next_var, &next_val );
        if( VbufCompVbuf( &line_var, &next_var, true ) == 0 ) {
            // found an environment variable, replace its value
            found_env[i] = true;
            modify_value_list( &line_val, &next_val, PATH_LIST_SEP, append, uninstall );
            modified = true;
        }
    }
    if( modified ) {
        VBUF    vbuf;

        VbufInit( &vbuf );
        output_line( &vbuf, vt, &line_var, &line_val );
        strcpy( line, VbufString( &vbuf ) );
        VbufFree( &vbuf );
    }
    VbufFree( &next_val );
    VbufFree( &next_var );
    VbufFree( &line_val );
    VbufFree( &line_var );
}
#endif

static void FinishEnvironmentLines( FILE *fp, int num_env, bool *found_env, bool batch )
/**************************************************************************************/
{
    int                 i;
    int                 j;
    append_mode         append;
    bool                libpath_batch;
    VBUF                val_before;
    VBUF                val_after;
    VBUF                cur_var;
    VBUF                cur_val;
    VBUF                next_var;
    VBUF                next_val;
    VBUF                vbuf;
    VBUF                tmp;

    VbufInit( &cur_var );
    VbufInit( &cur_val );
    VbufInit( &next_var );
    VbufInit( &next_val );
    VbufInit( &val_before );
    VbufInit( &val_after );
    VbufInit( &vbuf );
    VbufInit( &tmp );
    for( i = 0; i < num_env; ++i ) {
        if( found_env[i] || !SimCheckEnvironmentCondition( i ) )
            continue;
        append = SimGetEnvironmentStrings( i, &cur_var, &cur_val );
        libpath_batch = false;
        if( batch ) {
            if( VbufCompStr( &cur_var, "LIBPATH", true ) == 0 ) {
                libpath_batch = true;
                if( append == AM_AFTER ) {
                    VbufSetVbuf( &val_after, &cur_val );
                } else {
                    VbufSetVbuf( &val_before, &cur_val );
                }
            } else if( append == AM_AFTER ) {
                VbufPrepChr( &cur_val, PATH_LIST_SEP );
                VbufPrepStr( &cur_val, NAME_SUFFIX );
                VbufPrepVbuf( &cur_val, &cur_var );
                VbufPrepStr( &cur_val, NAME_PREFIX );
            } else if( append == AM_BEFORE ) {
                VbufConcChr( &cur_val, PATH_LIST_SEP );
                VbufConcStr( &cur_val, NAME_PREFIX );
                VbufConcVbuf( &cur_val, &cur_var );
                VbufConcStr( &cur_val, NAME_SUFFIX );
//            } else {
//                strcpy( value, cur_val );
            }
//        } else {
//            strcpy( value, cur_val );
        }
        for( j = i + 1; j < num_env; ++j ) {
            if( found_env[j] || !SimCheckEnvironmentCondition( j ) )
                continue;
            append = SimGetEnvironmentStrings( j, &next_var, &next_val );
            if( VbufCompVbuf( &cur_var, &next_var, true ) == 0 ) {
                found_env[j] = true;
                if( libpath_batch ) {
                    modify_value_list_libpath( &val_before, &val_after, &next_val, PATH_LIST_SEP, append );
                } else {
                    modify_value_list( &cur_val, &next_val, PATH_LIST_SEP, append, false );
                }
            }
        }
        if( libpath_batch ) {
            if( VbufLen( &val_before ) > 0 ) {
                VbufSetStr( &tmp, "BEGINLIBPATH" );
                if( output_line( &vbuf, VAR_SETENV_ASSIGN, &tmp, &val_before ) ) {
                    fputs_vbuf( &vbuf, fp );
                    fputc( '\n', fp );
                }
            }
            if( VbufLen( &val_after ) > 0 ) {
                VbufSetStr( &tmp, "ENDLIBPATH" );
                if( output_line( &vbuf, VAR_SETENV_ASSIGN, &tmp, &val_after ) ) {
                    fputs_vbuf( &vbuf, fp );
                    fputc( '\n', fp );
                }
            }
            if( output_line( &vbuf, VAR_SETENV_ASSIGN, &cur_var, &cur_val ) ) {
                fputs_vbuf( &vbuf, fp );
                fputc( '\n', fp );
            }
        } else {
            if( output_line( &vbuf, getEnvironVarType( &cur_var ), &cur_var, &cur_val ) ) {
                fputs_vbuf( &vbuf, fp );
                fputc( '\n', fp );
            }
        }
    }
    VbufFree( &tmp );
    VbufFree( &vbuf );
    VbufFree( &val_after );
    VbufFree( &val_before );
    VbufFree( &next_val );
    VbufFree( &next_var );
    VbufFree( &cur_val );
    VbufFree( &cur_var );
}


#if !defined( __UNIX__ )

static bool ModFile( const VBUF *orig, const VBUF *new,
                     void (*func_xxx)( char *, int, bool *, bool ),
                     void (*finish_xxx)( FILE *, int, bool *, bool ),
                     int num_xxx, int num_env, bool uninstall )
/*****************************************************************/
{
    FILE                *fp1, *fp2;
    char                *line;
    bool                *found_xxx = NULL;
    bool                *found_env = NULL;
    char                envbuf[MAXENVVAR + 1];

    fp1 = fopen_vbuf( orig, "rt" );
    if( fp1 == NULL ) {
        MsgBoxVbuf( NULL, "IDS_ERROR_OPENING", GUI_OK, orig );
        return( false );
    }
    fp2 = fopen_vbuf( new, "wt" );
    if( fp2 == NULL ) {
        MsgBoxVbuf( NULL, "IDS_ERROR_OPENING", GUI_OK, new );
        fclose( fp1 );
        return( false );
    }
    // allocate array to remember variables
    if( num_xxx > 0 ) {
        found_xxx = GUIMemAlloc( num_xxx * sizeof( bool ) );
        if( found_xxx == NULL ) {
            return( false );
        }
        memset( found_xxx, false, num_xxx * sizeof( bool ) );
    }
    if( num_env > 0 ) {
        found_env = GUIMemAlloc( num_env * sizeof( bool ) );
        if( found_env == NULL ) {
            GUIMemFree( found_xxx );
            return( false );
        }
        memset( found_env, false, num_env * sizeof( bool ) );
    }
    while( fgets( envbuf, sizeof( envbuf ) - 1, fp1 ) != NULL ) {
        line = strchr( envbuf, '\n' );
        if( line != NULL ) {
            *line = '\0';
        }
        // don't process empty lines but keep them in new file
        line = envbuf;
        SKIP_SPACES( line );
        if( line[0] != '\0' ) {
            func_xxx( line, num_xxx, found_xxx, uninstall );
            if( num_env > 0 ) {
                CheckEnvironmentLine( line, num_env, found_env, uninstall );
            }
            if( line[0] == '\0' ) {
                // skip removed lines
                continue;
            }
        }
        strcat( envbuf, "\n" );
        if( fputs( envbuf, fp2 ) < 0 ) {
            MsgBoxVbuf( NULL, "IDS_ERROR_WRITING", GUI_OK, new );
            return( false );
        }
    }
    fclose( fp1 );
    if( !uninstall ) {
        // handle any remaining variables
        finish_xxx( fp2, num_xxx, found_xxx, false );
        FinishEnvironmentLines( fp2, num_env, found_env, false );
    }
    if( num_xxx > 0 ) {
        GUIMemFree( found_xxx );
    }
    if( num_env > 0 ) {
        GUIMemFree( found_env );
    }
    if( fclose( fp2 ) != 0 ) {
        MsgBoxVbuf( NULL, "IDS_ERROR_CLOSING", GUI_OK, new );
        return( false );
    }
    return( true );
}


#ifndef __OS2__

static var_type getAutoVarType( const VBUF *auto_var )
/****************************************************/
{
    var_type        vt;

    if( IS_SETENVCMD( auto_var->buf ) ) {
        vt = VAR_ASSIGN_SETENV;
    } else {
        vt = VAR_CMD;
    }
    return( vt );
}

static void CheckAutoLine( char *line, int num_auto, bool *found_auto, bool uninstall )
/*************************************************************************************/
{
    int                 i;
    append_mode         append;
    VBUF                fname;
    VBUF                fext;
    var_type            vt;
    bool                modified;
    VBUF                line_var;
    VBUF                line_val;
    VBUF                next_var;
    VBUF                next_val;

    VbufInit( &line_var );
    VbufInit( &line_val );
    vt = parse_line( line, &line_var, &line_val, VAR_ASSIGN_SETENV );
    switch( vt ) {
    case VAR_ASSIGN_SETENV:
        break;
    case VAR_CMD:
        VbufInit( &fname );
        VbufInit( &fext );
        if( VbufCompStr( &line_var, "PATH", true ) != 0 ) {
            if( uninstall )
                break;
            VbufSplitpath( &line_var, NULL, NULL, &fname, &fext );
            if( VbufCompStr( &fname, "win", true ) != 0 || ( VbufCompExt( &fext, "com", true ) != 0 && VbufLen( &fext ) > 0 ) )
                break;
            WinDotCom = GUIStrDup( line, NULL );
            line[0] = '\0';
        }
        VbufFree( &fext );
        VbufFree( &fname );
        // fall through
    default:
        VbufFree( &line_val );
        VbufFree( &line_var );
        return;
    }
    VbufInit( &next_var );
    VbufInit( &next_val );
    modified = false;
    for( i = 0; i < num_auto; ++i ) {
        if( found_auto[i] || !uninstall && !SimCheckAutoExecCondition( i ) )
            continue;
        append = SimGetAutoExecStrings( i, &next_var, &next_val );
        if( VbufCompVbuf( &line_var, &next_var, true ) == 0 ) {
            // found an command, replace its value
            found_auto[i] = true;
            modify_value_list( &line_val, &next_val, PATH_LIST_SEP, append, uninstall );
            modified = true;
        }
    }
    if( modified ) {
        VBUF    vbuf;

        VbufInit( &vbuf );
        output_line( &vbuf, vt, &line_var, &line_val );
        strcpy( line, VbufString( &vbuf ) );
        VbufFree( &vbuf );
    }
    VbufFree( &next_val );
    VbufFree( &next_var );
    VbufFree( &line_val );
    VbufFree( &line_var );
}

static void FinishAutoLines( FILE *fp, int num_auto, bool *found_auto, bool batch )
/*********************************************************************************/
{
    int                 i;
    int                 j;
    append_mode         append;
    VBUF                cur_var;
    VBUF                cur_val;
    VBUF                next_var;
    VBUF                next_val;
    VBUF                vbuf;

    /* unused parameters */ (void)batch;

    VbufInit( &cur_var );
    VbufInit( &cur_val );
    VbufInit( &next_var );
    VbufInit( &next_val );
    VbufInit( &vbuf );
    for( i = 0; i < num_auto; ++i ) {
        if( found_auto[i] || !SimCheckAutoExecCondition( i ) )
            continue;
        append = SimGetAutoExecStrings( i, &cur_var, &cur_val );
        for( j = i + 1; j < num_auto; ++j ) {
            if( found_auto[j] || !SimCheckAutoExecCondition( j ) )
                continue;
            append = SimGetAutoExecStrings( j, &next_var, &next_val );
            if( VbufCompVbuf( &cur_var, &next_var, true ) == 0 ) {
                found_auto[j] = true;
                modify_value_list( &cur_val, &next_val, PATH_LIST_SEP, append, false );
            }
        }
        if( output_line( &vbuf, getAutoVarType( &cur_var ), &cur_var, &cur_val ) ) {
            fputs_vbuf( &vbuf, fp );
            fputc( '\n', fp );
        }
    }
    if( WinDotCom != NULL ) {
        fputs( WinDotCom, fp );
        fputc( '\n', fp );
        GUIMemFree( WinDotCom );
        WinDotCom = NULL;
    }
    VbufFree( &vbuf );
    VbufFree( &next_val );
    VbufFree( &next_var );
    VbufFree( &cur_val );
    VbufFree( &cur_var );
}


static bool ModAuto( const VBUF *orig, const VBUF *new, bool uninstall )
/**********************************************************************/
{
    int         num_auto;
    int         num_env;
    bool        rc;
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    bool        isOS2DosBox;
#endif

    num_auto = SimNumAutoExec();
    num_env = SimNumEnvironment();
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    isOS2DosBox = GetVariableBoolVal( "IsOS2DosBox" );
    SetBoolVariableByName( "IsOS2DosBox", false );
#endif
    rc = ModFile( orig, new, CheckAutoLine, FinishAutoLines, num_auto, num_env, uninstall );
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    SetBoolVariableByName( "IsOS2DosBox", isOS2DosBox );
#endif
    return( rc );
}

#endif

static var_type getConfigVarType( const VBUF *cfg_var )
/*****************************************************/
{
    var_type        vt;

    if( IS_SETENVCMD( cfg_var->buf ) ) {
        vt = VAR_ASSIGN_SETENV;
    } else {
        vt = VAR_ASSIGN;
    }
    return( vt );
}

static void CheckConfigLine( char *line, int num_cfg, bool *found_cfg, bool uninstall )
/*************************************************************************************/
{
    int                 i;
    append_mode         append;
    var_type            vt;
    bool                run_find;
    bool                run_found;
    bool                modified;
    VBUF                line_var;
    VBUF                line_val;
    VBUF                next_var;
    VBUF                next_val;

    VbufInit( &line_var );
    VbufInit( &line_val );
    vt = parse_line( line, &line_var, &line_val, VAR_ASSIGN_SETENV );
    switch( vt ) {
    case VAR_ASSIGN_SETENV:
    case VAR_ASSIGN:
        break;
    default:
        VbufFree( &line_val );
        VbufFree( &line_var );
        return;
    }
    VbufInit( &next_var );
    VbufInit( &next_val );
    run_find = ( VbufCompStr( &line_var, "RUN", true ) == 0 );
    modified = false;
    run_found = false;
    for( i = 0; i < num_cfg; ++i ) {
        if( found_cfg[i] || !uninstall && !SimCheckConfigCondition( i ) )
            continue;
        append = SimGetConfigStrings( i, &next_var, &next_val );
        if( VbufCompVbuf( &line_var, &next_var, true ) == 0 ) {
            // found an variable
            if( run_find ) {
                // found RUN variable
                if( VbufCompVbuf( &line_val, &next_val, true ) == 0 ) {
                    // if already there, just mark it as found
                    found_cfg[i] = true;
                    run_found = true;
                }
                continue;
            }
            if( isdigit( VbufString( &line_val )[0] ) ) { // for files=20, linefers=30 etc
                if( uninstall || atoi( VbufString( &next_val ) ) <= atoi( VbufString( &line_val ) ) ) {
                    found_cfg[i] = true;
                    continue;
                }
            }
            found_cfg[i] = true;
            // replace its value
            modify_value_list( &line_val, &next_val, PATH_LIST_SEP, append, uninstall );
            modified = true;
        }
    }
    if( run_found && uninstall ) {
        VBUF    vbuf;
        VBUF    tmp;

        VbufInit( &vbuf );
        VbufInit( &tmp );
        VbufConcStr( &tmp, "REM RUN" );
        output_line( &vbuf, vt, &tmp, &line_val );
        strcpy( line, VbufString( &vbuf ) );
        VbufFree( &tmp );
        VbufFree( &vbuf );
    } else if( modified ) {
        VBUF    vbuf;

        VbufInit( &vbuf );
        output_line( &vbuf, vt, &line_var, &line_val );
        strcpy( line, VbufString( &vbuf ) );
        VbufFree( &vbuf );
    }
    VbufFree( &next_val );
    VbufFree( &next_var );
    VbufFree( &line_val );
    VbufFree( &line_var );
}

static void FinishConfigLines( FILE *fp, int num_cfg, bool *found_cfg, bool batch )
/*********************************************************************************/
{
    int                 i;
    int                 j;
    append_mode         append;
    VBUF                cur_var;
    VBUF                cur_val;
    VBUF                next_var;
    VBUF                next_val;
    VBUF                vbuf;

    /* unused parameters */ (void)batch;

    VbufInit( &cur_var );
    VbufInit( &cur_val );
    VbufInit( &next_var );
    VbufInit( &next_val );
    VbufInit( &vbuf );
    for( i = 0; i < num_cfg; ++i ) {
        if( found_cfg[i] || !SimCheckConfigCondition( i ) )
            continue;
        append = SimGetConfigStrings( i, &cur_var, &cur_val );
        for( j = i + 1; j < num_cfg; ++j ) {
            if( found_cfg[j] || !SimCheckConfigCondition( j ) )
                continue;
            append = SimGetConfigStrings( j, &next_var, &next_val );
            if( VbufCompVbuf( &cur_var, &next_var, true ) == 0 ) {
                found_cfg[j] = true;
                modify_value_list( &cur_val, &next_val, PATH_LIST_SEP, append, false );
            }
        }
        if( output_line( &vbuf, getConfigVarType( &cur_var ), &cur_var, &cur_val ) ) {
            fputs_vbuf( &vbuf, fp );
            fputc( '\n', fp );
        }
    }
    VbufFree( &vbuf );
    VbufFree( &next_val );
    VbufFree( &next_var );
    VbufFree( &cur_val );
    VbufFree( &cur_var );
}

static bool ModConfig( const VBUF *orig, const VBUF *new, bool uninstall )
/************************************************************************/
{
    int         num_cfg;
    int         num_env;

    num_cfg = SimNumConfig();
#ifndef __OS2__
    if( GetVariableBoolVal( "IsOS2DosBox" ) ) {
#endif
        num_env = SimNumEnvironment();
#ifndef __OS2__
    } else {
        num_env = 0;
    }
#endif
    if( num_cfg == 0 && num_env == 0 ) {
         return( true );
    }
    return( ModFile( orig, new, CheckConfigLine, FinishConfigLines, num_cfg, num_env, uninstall ) );
}

static void BackupName( VBUF *backupname, const VBUF *filename )
/**************************************************************/
{
    VBUF        temp;
    int         num;

    VbufInit( &temp );

    VbufSetVbuf( backupname, filename );
    for( num = 0; num < 999; num++ ) {
        VbufSetInteger( &temp, num, 3 );
        VbufSetPathExt( backupname, &temp );
        if( access_vbuf( backupname, F_OK ) != 0 ) {
            break;
        }
    }

    VbufFree( &temp );
}

bool ModifyAutoExec( bool uninstall )
/***********************************/
{
    int             num_auto;
    int             num_cfg;
    int             num_env;
    char            boot_drive;
#ifndef __OS2__
    VBUF            newauto;
#endif
    VBUF            newcfg;
    FILE            *fp;
    VBUF            new_ext;
    bool            ok;
    VBUF            OrigAutoExec;
    VBUF            OrigConfig;


    num_auto = SimNumAutoExec();
    num_cfg = SimNumConfig();
    num_env = SimNumEnvironment();
    if( num_auto == 0 && num_cfg == 0 && num_env == 0 ) {
        return( true );
    }
    VbufInit( &OrigAutoExec );
    VbufInit( &OrigConfig );
    VbufInit( &new_ext );
    VbufInit( &newcfg );
#ifndef __OS2__
    VbufInit( &newauto );
#endif

    ok = true;
#ifdef __OS2__
    SetVariableByName( "AutoText", GetVariableStrVal( "IDS_MODIFY_CONFIG" ) );
#else
    SetVariableByName( "AutoText", GetVariableStrVal( "IDS_MODIFY_AUTOEXEC" ) );
#endif
    if( DoDialog( "Modify" ) == DLG_CANCEL ) {
        ok = false;
    }
    if( ok ) {
        if( GetVariableBoolVal( "IsWin95" ) ) {
            boot_drive = '\0';
        } else {
            boot_drive = 'A' + GetBootDrive() - 1;
            if( boot_drive == 'A' - 1 ) {
                boot_drive = '\0';
            }
        }
        if( boot_drive == '\0' ) {
#ifdef __NT__
            if( GetDriveType( "C:\\" ) == DRIVE_FIXED ) {
                boot_drive = 'C';   // assume C if it is a hard drive
            } else {
                // otherwise guess it is the same as the windows system directory
                const char  *sys_drv;
                sys_drv = GetVariableStrVal( "WinSystemDir" );
                boot_drive = toupper( sys_drv[0] );
            }
#else
            boot_drive = 'C';       // assume C
#endif
        }
        VbufSetStr( &OrigAutoExec, "?:\\AUTOEXEC.BAT" );
        VbufSetStr( &OrigConfig, "?:\\CONFIG.SYS" );
        VbufSetPathDrive( &OrigAutoExec, boot_drive );
        VbufSetPathDrive( &OrigConfig, boot_drive );

        SetVariableByName( "FileToFind", "CONFIG.SYS" );
        while( access_vbuf( &OrigConfig, F_OK ) != 0 ) {
            SetVariableByName_vbuf( "CfgDir", &OrigConfig );
            if( DoDialog( "LocCfg" ) == DLG_CANCEL ) {
                MsgBox( NULL, "IDS_CANTFINDCONFIGSYS", GUI_OK );
                ok = false;
                break;
            }
            VbufSetStr( &newcfg, GetVariableStrVal( "CfgDir" ) );
            boot_drive = VbufString( &newcfg )[0];
            VbufSetPathDrive( &OrigConfig, boot_drive );
            VbufSetPathDrive( &OrigAutoExec, boot_drive );
            if( GetVariableBoolVal( "CfgFileCreate" ) ) {
                fp = fopen_vbuf( &OrigConfig, "wt" );
                if( fp == NULL ) {
                    MsgBoxVbuf( NULL, "IDS_CANTCREATEFILE", GUI_OK, &OrigConfig );
                } else {
                    fclose( fp );
                }
            }
        }
    }
#ifndef __OS2__
    if( ok ) {
        SetVariableByName( "FileToFind", "AUTOEXEC.BAT" );
        while( access_vbuf( &OrigAutoExec, F_OK ) != 0 ) {
            SetVariableByName_vbuf( "CfgDir", &OrigAutoExec );
            if( DoDialog( "LocCfg" ) == DLG_CANCEL ) {
                MsgBox( NULL, "IDS_CANTFINDAUTOEXEC", GUI_OK );
                ok = false;
                break;
            }
            VbufSetStr( &newcfg, GetVariableStrVal("CfgDir") );
            boot_drive = VbufString( &newcfg )[0];
            VbufSetPathDrive( &OrigAutoExec, boot_drive );
            if( GetVariableBoolVal( "CfgFileCreate" ) ) {
                fp = fopen_vbuf( &OrigAutoExec, "wt" );
                if( fp == NULL ) {
                    MsgBoxVbuf( NULL, "IDS_CANTCREATEFILE", GUI_OK, &OrigAutoExec );
                } else {
                    fclose( fp );
                }
            }
        }
    }
#endif
    if( ok ) {
        if( GetVariableBoolVal( "ModNow" ) ) {
            // copy current files to AUTOEXEC.BAK and CONFIG.BAK

#ifndef __OS2__
            BackupName( &newauto, &OrigAutoExec );
#endif
            BackupName( &newcfg, &OrigConfig );

#ifndef __OS2__
            MsgBoxVbuf2( NULL, "IDS_COPYAUTOEXEC", GUI_OK, &newauto, &newcfg );
            if( DoCopyFile( &OrigAutoExec, &newauto, false ) != CFE_NOERROR ) {
                MsgBox( NULL, "IDS_ERRORBACKAUTO", GUI_OK );
            } else {
                if( !ModAuto( &newauto, &OrigAutoExec, uninstall ) ) {
                    ok = false;
                }
            }
#else
            MsgBoxVbuf( NULL, "IDS_COPYCONFIGSYS", GUI_OK, &newcfg );
#endif
            if( ok ) {
                if( DoCopyFile( &OrigConfig, &newcfg, false ) != CFE_NOERROR ) {
                    MsgBox( NULL, "IDS_ERRORBACKCONFIG", GUI_OK );
                } else {
                    if( !ModConfig( &newcfg, &OrigConfig, uninstall ) ) {
                        ok = false;
                    }
#ifdef __OS2__
                    MsgBox( NULL, "IDS_OS2CONFIGSYS", GUI_OK );
#endif
                }
                if( ok ) {
                    // indicate config files were modified if and only if we got this far
                    ConfigModified = true;
                }
            }
        } else {    // handle "ModLater" case
            VbufConcStr( &new_ext, BATCH_EXT_SAVED );
            // place modifications in AUTOEXEC.NEW and CONFIG.NEW
#ifndef __OS2__
            GetOldConfigFileDir( &newauto, &OrigAutoExec, uninstall );
            VbufConcVbufPos( &newauto, &OrigAutoExec, 2 );
            VbufSetPathExt( &newauto, &new_ext );
#endif
            GetOldConfigFileDir( &newcfg, &OrigConfig, uninstall );
            VbufConcVbufPos( &newcfg, &OrigConfig, 2 );
            VbufSetPathExt( &newcfg, &new_ext );

#ifndef __OS2__
            MsgBoxVbuf2( NULL, "IDS_NEWAUTOEXEC", GUI_OK, &newauto, &newcfg );
            if( !ModAuto( &OrigAutoExec, &newauto, uninstall ) ) {
                ok = false;
            }
#else
            MsgBoxVbuf( NULL, "IDS_NEWCONFIGSYS", GUI_OK, &newcfg );
#endif
            if( ok ) {
                if( !ModConfig( &OrigConfig, &newcfg, uninstall ) ) {
                    ok = false;
                }
            }
        }
    }
#ifndef __OS2__
    VbufFree( &newauto );
#endif
    VbufFree( &newcfg );
    VbufFree( &new_ext );
    VbufFree( &OrigConfig );
    VbufFree( &OrigAutoExec );
    return( ok );
}

#endif   // !__UNIX__

void ReplaceVars( VBUF *dst, const char *src )
/********************************************/
//  Replace occurrences of %variable% in src with the destination directory,
//  and place the result in buffer.
{
    size_t              len;
    const char          *p;
    const char          *e;
    const char          *varname;
    char                *quest;
    char                *colon;
    const char          *varval;
    VBUF                tmp;
    VBUF                var1;
    VBUF                var2;

    VbufInit( &tmp );
    VbufInit( &var1 );
    VbufInit( &var2 );
    if( src != NULL ) {
        VbufSetStr( dst, src );
    }
    p = VbufString( dst );
    while( *p != '\0' ) {
        if( *p++ != '%' )
            continue;
        if( *p == '%' ) {
            len = p - VbufString( dst );
            VbufSetStr( &tmp, p + 1 );
            VbufSetVbufAt( dst, &tmp, len );
            p = VbufString( dst ) + len;
            continue;
        }
        e = strchr( p, '%' );
        if( e == NULL ) {
            break;
        }
        VbufSetBuffer( &tmp, p, e - p );
        varname = VbufString( &tmp );
        for( ;; ) {     // loop for multiple '?' operators
            quest = strchr( varname, '?' );
            if( quest == NULL ) {
                if( stricmp( varname, "root" ) == 0 ) { // kludge?
                    varval = GetVariableStrVal( "DstDir" );
                } else if( varname[0] == '@' ) {
                    varval = getenv( varname + 1 );
                } else {
                    varval = GetVariableStrVal( varname );
                }
                break;  // no '?' operator
            }
            VbufSetBuffer( &var1, varname, quest - varname );
            quest++;    // skip '?'
            colon = strchr( quest, ':' );
            VbufSetBuffer( &var2, quest, colon - quest );
            colon++;    // skip ':'
            if( GetOptionVarValue( GetVariableByName( VbufString( &var1 ) ) ) ) {
                varval = GetVariableStrVal( VbufString( &var2 ) );
                break;
            }
            varname = colon;
        }
        len = p - 1 - VbufString( dst );
        VbufSetStr( &tmp, e + 1 );
        if( varval != NULL ) {
            VbufPrepStr( &tmp, varval );
        }
        VbufSetVbufAt( dst, &tmp, len );
        p = VbufString( dst ) + len;
    }
    VbufFree( &var2 );
    VbufFree( &var1 );
    VbufFree( &tmp );
}

//***************************************************************************

/* place additional paths to search in here
   NOTE:  trailing \ is necessary as is final NULL
*/

static char *AdditionalPaths[] = { "drive:\\directory\\",
                                   /* insert paths here */
                                   NULL };

static void secondarysearch( const VBUF *filename, VBUF *buffer )
/***************************************************************/
{
    VBUF                path;
    VBUF                drive;
    VBUF                dir;
    VBUF                name;
    VBUF                ext;
    unsigned int        counter;

    VbufInit( &path );
    VbufInit( &drive );
    VbufInit( &dir );
    VbufInit( &name );
    VbufInit( &ext );

    VbufRewind( buffer );
    VbufSplitpath( filename, NULL, NULL, &name, &ext );
    for( counter = 0; AdditionalPaths[counter] != NULL; counter++ ) {
        VbufSetStr( &path, AdditionalPaths[counter] );
        VbufSplitpath( &path, &drive, &dir, NULL, NULL );
        VbufMakepath( &path, &drive, &dir, &name, &ext );
        if( access_vbuf( &path, F_OK ) == 0 ) {
            VbufSetVbuf( buffer, &path );
            break;
        }
    }

    VbufFree( &ext );
    VbufFree( &name );
    VbufFree( &dir );
    VbufFree( &drive );
    VbufFree( &path );
}

static void VersionStr( int fp, char *ver, int verlen, char *verbuf, size_t verbuflen )
/*************************************************************************************/
{
    static char         Buffer[2048];
    int                 len;
    int                 size;
    char                *p;

    verbuf[0] = '\0';
    for( ;; ) {
        len = read( fp, Buffer, sizeof( Buffer ) );
        if( len < sizeof( Buffer ) ) {
            size = len;
            memset( Buffer + size, 0, sizeof( Buffer ) - size );
        } else {
            size = sizeof( Buffer ) - 256;
        }
        for( p = Buffer; p < Buffer + size; ++p ) {
            if( memcmp( p, ver, verlen ) == 0 ) {
                p += verlen;
                strncpy( verbuf, p, verbuflen );
                verbuf[verbuflen - 1] = '\0';
                return;
            }
        }
        if( len < sizeof( Buffer ) )
            break;    // eof
        if( lseek( fp, -256L, SEEK_CUR ) == -1L ) {
            break;
        }
    }
}


static void CheckVersion( VBUF *path, VBUF *drive, VBUF *dir )
/************************************************************/
{
    int                 fp, hours;
    char                am_pm;
    char                buf[100];
    int                 check;
    struct stat         statbuf;
    struct tm           *timeptr;

    fp = open_vbuf( path, O_RDONLY | O_BINARY );
    if( fp == -1 ) {
        return;     // shouldn't happen
    }

    // concat date and time to end of path
    check = fstat( fp, &statbuf );
    if( check == -1 ) {
        close( fp );
        return;         // shouldn't happen
    }

    timeptr = gmtime( &(statbuf.st_mtime) );
    hours   = timeptr->tm_hour;
    if( hours <= 11 ) {
        am_pm = 'a';
        if( hours == 0 ) {
            hours += 12;
        }
    } else {
        am_pm = 'p';
        if( hours != 12 ) {
            hours -= 12;
        }
    }
    VbufSplitpath( path, drive, dir, NULL, NULL );
    VbufMakepath( path, drive, dir, NULL, NULL );
    sprintf( buf, "  (%.2d-%.2d-%.4d %.2d:%.2d%cm)  ",
             timeptr->tm_mon + 1, timeptr->tm_mday, timeptr->tm_year + 1900,
             hours, timeptr->tm_min, am_pm );
    VbufConcStr( path, buf );

    // also concat version number if it exists
    VersionStr( fp, "VeRsIoN=", 8, buf, sizeof( buf ) );
    if( buf[0] != '\0' ) {
        // Novell DLL
        VbufConcStr( path, buf );
    } else {
        lseek( fp, 0, SEEK_SET );
        VersionStr( fp, "FileVersion", 12, buf, sizeof( buf ) ); // includes terminating '\0' of "FileVersion"
        if( buf[0] != '\0' ) {
            // Windows DLL
            VbufConcStr( path, buf );
        }
    }
    close( fp );
}

#ifdef EXTRA_CAUTIOUS_FOR_DLLS
static bool replace_file( const VBUF *name, const VBUF *unpacked_as )
{
    if( access_vbuf( name, F_OK ) != 0 || remove_vbuf( name ) == 0 ) {
        rename_vbuf( unpacked_as, name );
    } else {
        remove_vbuf( unpacked_as );
        if( MsgBoxVbuf( NULL, "IDS_CANTREPLACE", GUI_YES_NO, name ) == GUI_RET_NO ) {
            return( true );
        }
    }
    return( false );
}
#endif


bool CheckInstallDLL( const VBUF *name, vhandle var_handle )
/**********************************************************/
{
    VBUF                dst_dir;
    VBUF                drive;
    VBUF                dir;
    VBUF                fname;
    VBUF                ext;
    VBUF                unpacked_as;
    VBUF                dll_name;
    VBUF                path1;
    VBUF                path2;
    VBUF                prev_path;
#if defined( __WINDOWS__ )
    OFSTRUCT            ofPrev;
#else
    char                buffer[_MAX_PATH];
#endif
    bool                cancel;
    bool                ok;

    VbufInit( &dst_dir );
    VbufInit( &drive );
    VbufInit( &dir );
    VbufInit( &fname );
    VbufInit( &ext );
    VbufInit( &unpacked_as );
    VbufInit( &dll_name );
    VbufInit( &path1 );
    VbufInit( &path2 );
    VbufInit( &prev_path );

    VbufSplitpath( name, &drive, &dir, &fname, &ext );
    VbufMakepath( &dll_name, NULL, NULL, &fname, &ext );
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
    VbufSetStr( &ext, "_D_" );
    VbufMakepath( &unpacked_as, &drive, &dir, &fname, &ext );
#else
    VbufMakepath( &unpacked_as, &drive, &dir, &fname, &ext );
#endif
    cancel = false;
    ok = true;
#if defined( __WINDOWS__ )
    if( OpenFile( VbufString( &dll_name ), &ofPrev, OF_EXIST ) != -1 ) {
        VbufSetStr( &prev_path, ofPrev.szPathName );
#else
    _searchenv( VbufString( &dll_name ), "PATH", buffer );
    if( buffer[0] != '\0' ) {
        VbufSetStr( &prev_path, buffer );
#endif
    } else {
        secondarysearch( &dll_name, &prev_path );
        if( VbufLen( &prev_path ) == 0 ) {
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
            cancel = replace_file( name, &unpacked_as );
#endif
            ok = false;     // did not previously exist
        }
    }
    if( ok ) {
        VbufSplitpath( name, &drive, &dir, NULL, NULL );
        VbufMakepath( &path1, &drive, &dir, NULL, NULL );
//        strupr( path1 );
        VbufSplitpath( &prev_path, &drive, &dir, NULL, NULL );
        VbufMakepath( &path2, &drive, &dir, NULL, NULL );
//        strupr( path2 );
        VbufSetStr( &dst_dir, GetVariableStrVal( "DstDir" ) );
        if( VbufCompVbuf( &path1, &dst_dir, true ) == 0 && VbufCompVbuf( &path2, &dst_dir, true ) == 0 ) {
            /* both files are going into the main installation sub-tree */
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
            cancel = replace_file( name, &unpacked_as );
#endif
            ok = false;
        }
    }
    if( ok && VbufCompVbuf( &path1, &path2, true ) == 0 ) {
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
        /* both files are going into the same directory */
        struct stat         new, old;

        stat_vbuf( &prev_path, &old );
        stat_vbuf( &unpacked_as, &new );
        if( new.st_mtime < old.st_mtime ) {
            remove_vbuf( &unpacked_as );
        } else {
            cancel = replace_file( name, &unpacked_as );
        }
#endif
        /* there is only one file & it's been zapped */
        ok = false;
    }
    if( ok && CheckForceDLLInstall( &dll_name ) ) {
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
        cancel = replace_file( name, &unpacked_as );
#endif
        ok = false;
    }
    if( ok ) {
//        strupr( &dll_name );
        VbufSetVbuf( &path1, &unpacked_as );
        VbufSetVbuf( &path2, &prev_path );
        CheckVersion( &path1, &drive, &dir );
        CheckVersion( &path2, &drive, &dir );
        SetVariableByName_vbuf( "FileDesc", &dll_name );
        SetVariableByName_vbuf( "DLLDir", &path1 );
        SetVariableByName_vbuf( "OtherDLLDir", &path2 );

        // don't display the dialog if the user selected the "Skip dialog" option
        if( !GetVariableBoolVal( "DLL_Skip_Dialog" ) ) {
            if( DoDialog( "DLLInstall" ) == DLG_CANCEL ) {
                remove_vbuf( &unpacked_as );
                cancel = true;
                ok = false;
            }
        }
    }
    if( ok ) {
        if( GetVariableBoolVal( "DLL_Delete_Old" ) ) {
            remove_vbuf( &prev_path );
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
            cancel = replace_file( name, &unpacked_as );
#endif
        } else if( GetVariableBoolVal( "DLL_Keep_Both" ) ) {
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
            cancel = replace_file( name, &unpacked_as );
#endif
        } else if( GetVariableBoolVal( "DLL_Replace_Old" ) ) {
            DoCopyFile( &unpacked_as, &prev_path, false );
            SetVariableByHandle_vbuf( var_handle, &prev_path );
            remove_vbuf( &unpacked_as );
        } else if( GetVariableBoolVal( "DLL_Dont_Install" ) ) {
            SetVariableByHandle_vbuf( var_handle, &prev_path );
            remove_vbuf( &unpacked_as );
        } else if( GetVariableBoolVal( "DLL_Abort_Install" ) ) {
            SetVariableByHandle_vbuf( var_handle, &prev_path );
            remove_vbuf( &unpacked_as );
            cancel = true;
        }
    }

    VbufFree( &prev_path );
    VbufFree( &path2 );
    VbufFree( &path1 );
    VbufFree( &dll_name );
    VbufFree( &unpacked_as );
    VbufFree( &ext );
    VbufFree( &fname );
    VbufFree( &dir );
    VbufFree( &drive );
    VbufFree( &dst_dir );
    return( cancel );
}


//***************************************************************************

#if defined( __NT__ )

static bool ModEnv( int num_env, bool uninstall )
/***********************************************/
{
    bool                ok;
    int                 i;
    int                 j;
    int                 k;
    int                 rc;
    append_mode         append;
    DWORD               reg_var_len;
    DWORD               reg_val_len;
    DWORD               type;
    DWORD               reg_type;
    char                reg_var[MAXBUF];
    char                reg_val[MAXENVVAR];
    VBUF                cur_var;
    VBUF                cur_val;
    VBUF                reg_val_vbuf;

    VbufInit( &cur_var );
    VbufInit( &cur_val );
    VbufInit( &reg_val_vbuf );
    ok = true;
    for( i = 0; i < num_env; i ++ ) {
        if( !uninstall && !SimCheckEnvironmentCondition( i ) )
            continue;

        append = SimGetEnvironmentStrings( i, &cur_var, &cur_val );
        for( j = CURRENT_USER; j < NUM_REG_LOCATIONS; j++ ) {
            if( !RegLocation[j].key_is_open || !RegLocation[j].modify ) {
                continue;
            }
            // Look for current definition
            // we need to do this all the time, since if we are uninstalling
            // we want to get rid of existing settings
            k = 0;
            do {
                reg_var_len = MAXENVVAR;
                reg_val_len = MAXENVVAR;
                rc = RegEnumValue( RegLocation[j].key, k, reg_var, &reg_var_len, NULL, NULL,
                                    (LPBYTE)reg_val, &reg_val_len );
                if( rc != 0 )
                    break;
                ++k;
            } while( VbufCompStr( &cur_var, reg_var, true ) != 0 );
            if( rc == ERROR_NO_MORE_ITEMS ) {
                // No existing value so add it
                if( uninstall ) {
                    rc = 0;
                } else {
                    rc = RegSetValueEx( RegLocation[j].key, VbufString( &cur_var ), 0, REG_SZ,
                        (LPBYTE)VbufString( &cur_val ), (DWORD)( VbufLen( &cur_val ) + 1 ) );
                }
            } else if( rc == 0 ) {
                VbufSetStr( &reg_val_vbuf, reg_val );
                modify_value_list( &reg_val_vbuf, &cur_val, PATH_LIST_SEP, append, uninstall );
                if( VbufLen( &reg_val_vbuf ) == 0 ) {
                    rc = RegDeleteValue( RegLocation[j].key, VbufString( &cur_var ) );
                } else {
                    rc = RegQueryValueEx( RegLocation[j].key, VbufString( &cur_var ), NULL,
                                            &reg_type, NULL, NULL );
                    if( rc == 0 ) {
                        type = reg_type;
                    } else {
                        type = REG_SZ;
                    }
                    rc = RegSetValueEx( RegLocation[j].key, VbufString( &cur_var ), 0, type,
                        (LPBYTE)VbufString( &reg_val_vbuf ), (DWORD)( VbufLen( &reg_val_vbuf ) + 1 ) );
                }
            }
            if( rc != 0 ) {
                ok = false;
            }
        }
    }
    VbufFree( &reg_val_vbuf );
    VbufFree( &cur_val );
    VbufFree( &cur_var );
    return( ok );
}

bool ModifyConfiguration( bool uninstall )
/****************************************/
{
    int                 num_env;
    VBUF                changes;
    VBUF                temp;
    FILE                *fp;
    int                 i, j;
    bool                bRet;
    VBUF                cur_var;
    VBUF                cur_val;
    VBUF                next_var;
    VBUF                next_val;
    bool                *found;
    append_mode         append;
    int                 rc;

    num_env = SimNumEnvironment();
    if( num_env == 0 ) {
        return( true );
    }

    rc = RegOpenKey( HKEY_CURRENT_USER, "Environment", &RegLocation[CURRENT_USER].key );
    RegLocation[CURRENT_USER].key_is_open = ( rc == 0 );

    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
              "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
              0, KEY_ALL_ACCESS, &RegLocation[LOCAL_MACHINE].key );
    RegLocation[LOCAL_MACHINE].key_is_open = ( rc == 0 );

    if( RegLocation[LOCAL_MACHINE].key_is_open && !uninstall ) {
        if( DoDialog( "ModifyEnvironment" ) == DLG_CANCEL ) {
            return( false );
        }
    } else {
        // Note we use the same dialog as for AUTOEXEC changes
        // We set the Variable AUTOTEXT to contain the proper wording
        SetVariableByName( "AutoText", GetVariableStrVal( "IDS_MODIFY_ENVIRONMENT" ) );
        if( DoDialog( "Modify" ) == DLG_CANCEL ) {
            return( false );
        }
    }

    if( GetVariableBoolVal( "ModNow" ) ) {
        if( uninstall ) { //Clean up everywhere
            RegLocation[LOCAL_MACHINE].modify = true;
            RegLocation[CURRENT_USER].modify  = true;
        } else if( GetVariableBoolVal( "ModMachine" ) ) {
            RegLocation[LOCAL_MACHINE].modify = true;
            RegLocation[CURRENT_USER].modify  = false;
        } else { // ModNow == 1 or ModUser == 1
            RegLocation[LOCAL_MACHINE].modify = false;
            RegLocation[CURRENT_USER].modify  = true;
        }
        bRet = ModEnv( num_env, uninstall );
        // indicate config files were modified if and only if we got this far
        ConfigModified = true;
    } else {    // handle "ModLater" case
        VbufInit( &changes );
        VbufInit( &temp );

        found = GUIMemAlloc( num_env * sizeof( bool ) );
        memset( found, false, num_env * sizeof( bool ) );
        VbufConcStr( &temp, GetVariableStrVal( "DstDir" ) );
        GetOldConfigFileDir( &changes, &temp, uninstall );
        VbufConcStr( &changes, "\\CHANGES.ENV" );
        MsgBoxVbuf( NULL, "IDS_CHANGES", GUI_OK, &changes );
        fp = fopen_vbuf( &changes, "wt" );
        if( fp != NULL ) {
            VbufInit( &cur_var );
            VbufInit( &cur_val );
            VbufInit( &next_var );
            VbufInit( &next_val );

            fprintf( fp, "%s\n\n", GetVariableStrVal( "IDS_ENV_CHANGES" ) );
            for( i = 0; i < num_env; i ++ ) {
                if( found[i] || !SimCheckEnvironmentCondition( i ) )
                    continue;
                append = SimGetEnvironmentStrings( i, &cur_var, &cur_val );
                if( append == AM_AFTER ) {
                    fprintf( fp, GetVariableStrVal( "IDS_ADD_TO_VAR" ), VbufString( &cur_var ) );
                } else if( append == AM_BEFORE ) {
                    fprintf( fp, GetVariableStrVal( "IDS_ADD_TO_VAR" ), VbufString( &cur_var ) );
                } else {
                    fprintf( fp, GetVariableStrVal( "IDS_SET_VAR_TO" ), VbufString( &cur_var ) );
                }
                for( j = i + 1; j < num_env; ++j ) {
                    if( found[j] || !SimCheckEnvironmentCondition( j ) )
                        continue;
                    append = SimGetEnvironmentStrings( j, &next_var, &next_val );
                    if( VbufCompVbuf( &cur_var, &next_var, true ) == 0 ) {
                        found[j] = true;
                        modify_value_list( &cur_val, &next_val, PATH_LIST_SEP, append, uninstall );
                    }
                }
                fprintf( fp, "\n    %s\n", VbufString( &cur_val ) );
            }
            fclose( fp );

            VbufFree( &next_val );
            VbufFree( &next_var );
            VbufFree( &cur_val );
            VbufFree( &cur_var );
        }
        GUIMemFree( found );
        bRet = true;

        VbufFree( &temp );
        VbufFree( &changes );
    }

    if( RegLocation[CURRENT_USER].key_is_open ) {
        RegCloseKey( RegLocation[CURRENT_USER].key );
    }
    if( RegLocation[LOCAL_MACHINE].key_is_open ) {
        RegCloseKey( RegLocation[LOCAL_MACHINE].key );
    }

    return( bRet );
}

bool ModifyRegAssoc( bool uninstall )
/***********************************/
{
    HKEY    hkey;
    VBUF    temp;
    VBUF    ext;
    VBUF    keyname;
    int     num;
    int     i;

    if( !uninstall ) {
        if( DoDialog( "ModifyAssociations" ) == DLG_CANCEL ) {
            return( false );
        }
        if( GetVariableBoolVal( "NoModEnv" ) ) {
            return( true );
        }
        VbufInit( &temp );
        VbufInit( &ext );
        VbufInit( &keyname );
        num = SimNumAssociations();
        for( i = 0; i < num; i++ ) {
            if( !SimCheckAssociationCondition( i ) )
                continue;
            SimGetAssociationExt( i, &ext );
            SimGetAssociationKeyName( i, &keyname );
            VbufSetChr( &temp, '.' );
            VbufConcVbuf( &temp, &ext );
            RegCreateKey( HKEY_CLASSES_ROOT, VbufString( &temp ), &hkey );
            RegSetValue( hkey, NULL, REG_SZ, VbufString( &keyname ), (DWORD)VbufLen( &keyname ) );
            RegCloseKey( hkey );
            RegCreateKey( HKEY_CLASSES_ROOT, VbufString( &keyname ), &hkey );
            SimGetAssociationDescription( i, &temp );
            RegSetValue( hkey, NULL, REG_SZ, VbufString( &temp ), (DWORD)VbufLen( &temp ) );
            /* process program definition */
            SimGetAssociationProgram( i, &temp );
            if( VbufLen( &temp ) > 0 ) {
                VbufConcStr( &temp, " %1" );
                ReplaceVars1( &temp );
                RegSetValue( hkey, "shell\\open\\command", REG_SZ, VbufString( &temp ), (DWORD)VbufLen( &temp ) );
            }
            /* process icon definition */
            if( VbufLen( &temp ) > 0 ) {
                SimGetAssociationIconFileName( i, &temp );
                if( VbufLen( &temp ) == 0 ) {
                    /* if icon file not defined then use program name */
                    SimGetAssociationProgram( i, &temp );
                }
            } else {
                SimGetAssociationIconFileName( i, &temp );
            }
            VbufConcChr( &temp, ',' );
            VbufConcInteger( &temp, SimGetAssociationIconIndex( i ), 0 );
            ReplaceVars1( &temp );
            RegSetValue( hkey, "DefaultIcon", REG_SZ, VbufString( &temp ), (DWORD)VbufLen( &temp ) );
            RegCloseKey( hkey );
        }
        VbufFree( &keyname );
        VbufFree( &ext );
        VbufFree( &temp );
    }

    return( true );
}

bool AddToUninstallList( bool uninstall )
/***************************************/
{
    HKEY        hkey;
    const char  *val;
    DWORD       major;
    DWORD       minor;
    DWORD       dw;
    VBUF        buf;

    VbufInit( &buf );
    VbufConcStr( &buf, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" );
    VbufConcStr( &buf, GetVariableStrVal( "UninstallKeyName" ) );
    if( !uninstall ) {
        RegCreateKey( HKEY_LOCAL_MACHINE, VbufString( &buf ), &hkey );
        val = GetVariableStrVal( "UninstallDisplayName" );
        RegSetValueEx( hkey, "DisplayName", 0L, REG_SZ, (LPBYTE)val, (DWORD)( strlen( val ) + 1 ) );
        ReplaceVars( &buf, GetVariableStrVal( "UninstallCommand" ) );
        RegSetValueEx( hkey, "UninstallString", 0L, REG_SZ, (LPBYTE)VbufString( &buf ), (DWORD)( VbufLen( &buf ) + 1 ) );
        ReplaceVars( &buf, GetVariableStrVal( "UninstallIcon" ) );
        RegSetValueEx( hkey, "DisplayIcon", 0L, REG_SZ, (LPBYTE)VbufString( &buf ), (DWORD)( VbufLen( &buf ) + 1 ) );
        val = GetVariableStrVal( "UninstallCompany" );
        RegSetValueEx( hkey, "Publisher", 0L, REG_SZ, (LPBYTE)val, (DWORD)( strlen( val ) + 1 ) );
        val = GetVariableStrVal( "UninstallHelpURL" );
        RegSetValueEx( hkey, "HelpLink", 0L, REG_SZ, (LPBYTE)val, (DWORD)( strlen( val ) + 1 ) );
        major = GetVariableIntVal( "UninstallMajorVersion" );
        RegSetValueEx( hkey, "VersionMajor", 0L, REG_DWORD, (LPBYTE)&major, (DWORD)( sizeof( DWORD ) ) );
        minor = GetVariableIntVal( "UninstallMinorVersion" );
        RegSetValueEx( hkey, "VersionMinor", 0L, REG_DWORD, (LPBYTE)&minor, (DWORD)( sizeof( DWORD ) ) );
        VbufRewind( &buf );
        VbufConcInteger( &buf, major, 0 );
        VbufConcChr( &buf, '.' );
        VbufConcInteger( &buf, minor, 0 );
        RegSetValueEx( hkey, "DisplayVersion", 0L, REG_SZ, (LPBYTE)VbufString( &buf ), (DWORD)( VbufLen( &buf ) + 1 ) );
        val = GetVariableStrVal( "DstDir" );
        RegSetValueEx( hkey, "InstallLocation", 0L, REG_SZ, (LPBYTE)val, (DWORD)( strlen( val ) + 1 ) );
        dw = 1L;
        RegSetValueEx( hkey, "NoModify", 0L, REG_DWORD, (LPBYTE)&dw, (DWORD)( sizeof( DWORD ) ) );
        RegSetValueEx( hkey, "NoRepair", 0L, REG_DWORD, (LPBYTE)&dw, (DWORD)( sizeof( DWORD ) ) );
        RegCloseKey( hkey );
    } else {
        RegDeleteKey( HKEY_LOCAL_MACHINE, VbufString( &buf ) );
    }
    VbufFree( &buf );

    return( true );
}

#endif

bool GenerateBatchFile( bool uninstall )
/**************************************/
{
    int                 num_env;
    FILE                *fp;
    VBUF                batch_file;
    VBUF                drive;
    VBUF                dir;
    VBUF                fname;
    VBUF                ext;
    bool                *found;
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    bool                isOS2DosBox;
#endif

    VbufInit( &batch_file );
    VbufInit( &drive );
    VbufInit( &dir );
    VbufInit( &fname );
    VbufInit( &ext );

    ReplaceVars( &batch_file, GetVariableStrVal( "BatchFileName" ) );
    VbufSplitpath( &batch_file, &drive, &dir, &fname, &ext );
    if( VbufLen( &ext ) == 0 )
        VbufConcStr( &ext, BATCHEXT );
    VbufMakepath( &batch_file, &drive, &dir, &fname, &ext );
    if( uninstall ) {
        remove_vbuf( &batch_file );
    } else {
        fp = fopen_vbuf( &batch_file, "wt" );
        if( fp != NULL ) {
#ifdef __UNIX__
            fprintf( fp, "#!/bin/sh\n" );
#else
            fprintf( fp, "@echo off\n" );
#endif
            fprintf( fp, "echo %s\n", GetVariableStrVal( "BatchFileCaption" ) );
#if defined( __DOS__ ) || defined( __WINDOWS__ )
            isOS2DosBox = GetVariableBoolVal( "IsOS2DosBox" );
            SetBoolVariableByName( "IsOS2DosBox", false );
#endif
            num_env = SimNumEnvironment();
            if( num_env > 0 ) {
                found = GUIMemAlloc( num_env * sizeof( bool ) );
                memset( found, false, num_env * sizeof( bool ) );
                FinishEnvironmentLines( fp, num_env, found, true );
                GUIMemFree( found );
            }
#if defined( __DOS__ ) || defined( __WINDOWS__ )
            SetBoolVariableByName( "IsOS2DosBox", isOS2DosBox );
#endif
            fclose( fp );
#ifdef __UNIX__
            chmod_vbuf( &batch_file, PMODE_RX_USR_W );
#endif
        }
    }
    VbufInit( &ext );
    VbufInit( &fname );
    VbufInit( &dir );
    VbufInit( &drive );
    VbufFree( &batch_file );
    return( true );
}
