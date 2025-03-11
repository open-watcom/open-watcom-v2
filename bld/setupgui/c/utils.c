/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Setup utility functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <limits.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#ifdef __UNIX__
    #include <sys/utsname.h>
#endif
#include <errno.h>
#include "wdirent.h"
#include "wio.h"
#include "watcom.h"
#include "walloca.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"
#include "gendlg.h"
#include "guiutil.h"
#include "guiutils.h"
#include "utils.h"
#include "setupio.h"
#include "iopath.h"
#include "guistats.h"
#include "pathgrp2.h"

#include "clibext.h"


#if defined( __UNIX__ )
#define MAX_DRIVES      1
#else
#define DRIVE_NUM(x)    ((x) - 'A' + 1)
#define MAX_DRIVES      (1 + DRIVE_NUM('Z'))
#endif

#define TMPFILENAME "_watcom_.tmp"

#if defined( __NT__ ) || defined( __WINDOWS__ )
    #define UNC_SUPPORT
#endif

#define TEST_UNC(x)     (x[0] == '\\' && x[1] == '\\')
#define TEST_DRIVE(x)   (isalpha( x[0] ) && x[1] == ':')

typedef struct def_var {
    char                *variable;
    char                *value;
    struct def_var      *link;
} DEF_VAR;

bool            ConfigModified = false;
bool            SkipDialogs;
char            *VariablesFile;
DEF_VAR         *ExtraVariables;
bool            Invisible;
bool            ProgramGroups;
bool            StartupChange;
char            InstallerFile[_MAX_PATH] = { 0 };

void ConcatDirSep( char *dir )
/****************************/
{
    size_t  len;
    char    c;

    if( *dir == '\0' ) {
        dir[0] = '.';
        dir[1] = DIR_SEP;
        dir[2] = '\0';
    } else {
        len = strlen( dir );
        c = dir[len - 1];
        if( !IS_PATH_SEP( c ) ) {
            if( len > 1
              && IS_PATH_SEP( dir[len - 2] )
              && dir[len - 1] == '.' ) {
                dir[len - 1] = '\0';
            } else if( len == 2
              && dir[1] == ':' ) {
                dir[2] = '.';
                dir[3] = DIR_SEP;
                dir[4] = '\0';
            } else {
                dir[len++] = DIR_SEP;
                dir[len] = '\0';
            }
        }
    }
}

void RemoveDirSep( char *dir )
/****************************/
{
    size_t  len;
    char    c;

    if( *dir == '\0' ) {
        dir[0] = '.';
        dir[1] = '\0';
    } else {
        len = strlen( dir );
        c = dir[len - 1];
        if( IS_PATH_SEP( c ) ) {
            if( len == 1
              || len == 3
              && dir[1] == ':' ) {
                dir[len++] = '.';
                dir[len] = '\0';
            } else {
                dir[len - 1] = '\0';
            }
        } else {
            if( len == 2
              && dir[1] == ':' ) {
                dir[len++] = '.';
                dir[len] = '\0';
            }
        }
    }
}

bool ModifyEnvironment( bool uninstall )
/**************************************/
{
    bool        ret = true;

    if( ProgramGroups ) {
#if !defined( GUI_IS_GUI ) || defined( __UNIX__ )
        /* unused parameters */ (void)uninstall;
#else
        ret = CreatePMInfo( uninstall );
        if( !ret ) {                   /* create folder and icons */
            gui_message_return  gui_ret;

            if( uninstall != 0 ) {
                gui_ret = MsgBox( NULL, "IDS_PMREMNOGOOD", GUI_YES_NO );
            } else {
                gui_ret = MsgBox( NULL, "IDS_PMADDNOGOOD", GUI_YES_NO );
            }
            if( gui_ret == GUI_RET_YES ) {
                ret = true;
            }
        }
#endif
    }
    return( ret );
}

bool ModifyStartup( bool uninstall )
/**********************************/
{
    bool        ret = true;

#if defined( __UNIX__ ) && !defined( GUI_IS_GUI )
    /* unused parameters */ (void)uninstall;
#endif

    if( StartupChange ) {
#if defined( GUI_IS_GUI )
        /*
         * will write to the win.ini file.
         */
        WriteProfileStrings( uninstall );
#endif

//        if( !uninstall ) {
#if defined( __NT__ )
  #ifdef __AXP__
        ret = ModifyConfiguration( uninstall );
  #else
        if( GetVariableBoolVal( "IsWin95" ) ) {
            ret = ModifyAutoExec( uninstall );
        } else {
            ret = ModifyConfiguration( uninstall );
        }
  #endif
#elif defined( __UNIX__ )
//        ret = ModifyAutoExec( uninstall );
#else
        ret = ModifyAutoExec( uninstall );
#endif
    }
    return( ret );
}

bool ModifyAssociations( bool uninstall )
/***************************************/
{
    bool        ret = true;

#ifdef __NT__
    ret = ModifyRegAssoc( uninstall );
#else
    /* unused parameters */ (void)uninstall;
#endif
    return( ret );
}

bool ModifyUninstall( bool uninstall )
/************************************/
{
    bool        ret = true;

#ifdef __NT__
    ret = AddToUninstallList( uninstall );
#else
    /* unused parameters */ (void)uninstall;
#endif
    return( ret );
}

#if !defined( __UNIX__ )
typedef struct {
    disk_size           free_space;
    unsigned long       block_size;
    bool                use_target_for_tmp_file;
    bool                fixed;
    bool                removable;
} drive_info;

static drive_info       Drives[MAX_DRIVES];

#if defined( __DOS__ )

static int __far critical_error_handler( unsigned deverr, unsigned errcode, unsigned __far *devhdr )
/**************************************************************************************************/
{
    (void)deverr;
    (void)errcode;
    (void)devhdr;
    return( _HARDERR_FAIL );
}

typedef __far (HANDLER)( unsigned deverr, unsigned errcode, unsigned __far *devhdr );

#endif

static void NoHardErrors( void )
/******************************/
{
#if defined( __OS2__ )
    DosError( FERR_DISABLEHARDERR );
#elif defined( __DOS__ )
    _harderr( (HANDLER *)critical_error_handler );
#elif defined( __WINDOWS__ ) || defined( __NT__ )
    SetErrorMode( SEM_FAILCRITICALERRORS );
#endif
}

#ifdef __NT__
static bool NTSpawnWait( const char *cmd, DWORD *exit_code, HANDLE in, HANDLE out, HANDLE err )
/*********************************************************************************************/
{
    STARTUPINFO             start;
    PROCESS_INFORMATION     info;

    memset( &start, 0, sizeof( start ) );
    start.cb = sizeof( start );
    if( cmd[0] == '-' ) {
        cmd++;
        /*
         * set ShowWindow default value for nCmdShow parameter
         */
        start.dwFlags |= STARTF_USESHOWWINDOW;
        start.wShowWindow = SW_HIDE;
    }
    if( in
      || out
      || err ) {
        start.dwFlags |= STARTF_USESTDHANDLES;
        start.hStdInput = in;
        start.hStdOutput = GetStdHandle( STD_OUTPUT_HANDLE );
        start.hStdError = GetStdHandle( STD_ERROR_HANDLE );
    }
    if( !CreateProcess( NULL, (char *)cmd, NULL, NULL, TRUE,
                        CREATE_NEW_PROCESS_GROUP + CREATE_NEW_CONSOLE,
                        NULL, NULL, &start, &info ) ) {
        return( false );
    } else {
        WaitForSingleObject( info.hProcess, INFINITE );
        while( !GetExitCodeProcess( info.hProcess, exit_code ) ||
               *exit_code == STILL_ACTIVE ) {
            if( StatusCancelled() ) {
                return( false );
            }
        }
    }
    CloseHandle( info.hProcess );
    CloseHandle( info.hThread );
    return( true );
}
#endif

#ifdef __WINDOWS__
#include "toolhelp.h"

typedef BOOL WINAPI (task_func)( TASKENTRY FAR* );

static bool WinSpawnWait( const char *cmd )
/*****************************************/
{
    struct {
        WORD        segEnv;
        LPCSTR      lpzCmdLine;
        UINT FAR    *lpShow;
        UINT FAR    *lpReserved;
    } parm;
    UINT        show[2];
    HINSTANCE   inst;
    HINSTANCE   toolhelp;
    task_func   *taskfirst;
    task_func   *tasknext;
    TASKENTRY   task;
    bool        stillthere;
    bool        ok;
    VBUF        buff;

    show[0] = 2;
    show[1] = SW_SHOW;
    memset( &parm, 0, sizeof( parm ) );
    parm.lpzCmdLine = "";
    parm.lpShow = show;
    inst = LoadModule( cmd, &parm );
    if( inst < HINSTANCE_ERROR )
        return( false );
    VbufInit( &buff );
    ReplaceVars( &buff, "%ToolHelp%" );
    ok = ( VbufLen( &buff ) > 0 );
    if( ok ) {
        toolhelp = LoadModule( VbufString( &buff ), &parm );
        ok = ( toolhelp < HINSTANCE_ERROR );
    }
    VbufFree( &buff );
    if( !ok )
        return( false );
    taskfirst = (task_func *)GetProcAddress( toolhelp, "TaskFirst" );
    if( taskfirst == NULL )
        return( false );
    tasknext = (task_func *)GetProcAddress( toolhelp, "TaskNext" );
    if( tasknext == NULL )
        return( false );
    task.dwSize = sizeof( task );
    do {
        stillthere = false;
        ok = ( taskfirst( &task ) != 0 );
        while( ok ) {
            if( task.hInst == inst )
                stillthere = true;
            ok = ( tasknext( &task ) != 0 );
            StatusCancelled(); /* yield */
        }
    } while( stillthere );
    return( true );
}
#endif

#ifdef __OS2__
static bool OS2SpawnWait( const char *cmd, int *rc )
/**************************************************/
{
    RESULTCODES res;
    PID         dummy;
    char        *p;

    p = strchr( cmd, ' ' );
    if( p != NULL )
        *p = '\0';
    if( DosExecPgm( NULL, 0, EXEC_ASYNCRESULT, cmd, NULL, &res, cmd ) != 0 )
        return( false );

    if( p != NULL )
        *p = ' ';
    for( ;; ) {
        *rc = DosWaitChild( DCWA_PROCESS, DCWW_NOWAIT, &res, &dummy, res.codeTerminate );
        if( *rc != ERROR_CHILD_NOT_COMPLETE ) {
            *rc = res.codeResult;
            break;
        }
        StatusCancelled();
    }
    return( true );
}
#endif

#endif /* !defined( __UNIX__ ) */

static bool DoSpawnCmd( const char *cmd )
/***************************************/
{
    bool        rc;

#if defined( __WINDOWS__ )
    rc = WinSpawnWait( cmd );
#elif defined( __NT__ )
    {
        DWORD exit_code;

        if( NTSpawnWait( cmd, &exit_code, 0, 0, 0 ) ) {
            rc = (exit_code == 0);
        } else {
            rc = false;
        }
    }
#elif defined( __OS2__ )
    {
        int     code;

        OS2SpawnWait( cmd, &code );
        rc = (code == 0);
    }
#else
    system( cmd );
    rc = true;
#endif
    GUIWndDirty( NULL );
    return( rc );
}


void DoSpawn( when_time when )
/****************************/
{
    VBUF    buff;
    int     i;
    int     num_spawns;

    VbufInit( &buff );
    num_spawns = SimNumSpawns();
    for( i = 0; i < num_spawns; ++i ) {
        if( when != SimWhen( i ) )
            continue;
        if( !SimEvalSpawnCondition( i ) )
            continue;
        if( SimGetSpawnCommand( i, &buff ) )
            continue;
        DoSpawnCmd( VbufString( &buff ) );
    }
    VbufFree( &buff );
}

#if !defined( __UNIX__ )

#if defined( UNC_SUPPORT )
static void GetTmpFileNameUNC( const char *fs_path, VBUF *buff )
/**************************************************************/
{
    if( fs_path == NULL
      || buff == NULL ) {
        return;
    }
    VbufRewind( buff );
    if( TEST_UNC( fs_path ) ) {
        VbufConcStr( buff, fs_path );
        VbufAddDirSep( buff );
        VbufConcStr( buff, TMPFILENAME );
    } else {
        VbufConcChr( buff, fs_path[0] );
        VbufConcStr( buff, ":\\" TMPFILENAME );
    }
}
#endif

static void GetTmpFileName( const char *fs_path, VBUF *buff )
/***********************************************************/
{
    VbufSetChr( buff, fs_path[0] );
    VbufConcChr( buff, DRIVE_SEP );
    VbufConcChr( buff, DIR_SEP );
    VbufConcStr( buff, TMPFILENAME );
}

static void GetTmpFileNameInTarget( const char *fs_path, VBUF *buff )
/*******************************************************************/
{
    int         i;
    int         max_targets = SimNumTargets();
    char        drive;

    drive = tolower( fs_path[0] );
    for( i = 0; i < max_targets; ++i ) {
        SimTargetDir( i, buff );
        if( tolower( VbufString( buff )[0] ) == drive
          && VbufString( buff )[1] == ':' ) {
            VbufAddDirSep( buff );
            VbufConcStr( buff, TMPFILENAME );
            return;
        }
    }
    GetTmpFileName( fs_path, buff );
}

#endif /* !defined( __UNIX__ ) */

void ResetDriveInfo( void )
/*************************/
{
#if !defined( __UNIX__ )
    int         i;

    for( i = 0; i < MAX_DRIVES; ++i ) {
        Drives[i].block_size = 0;
    }
#endif
}

#if !defined( __UNIX__ )
static int GetDriveNum( const char *fs_path )
/*******************************************/
{
    int     drive_num;

    drive_num = DRIVE_NUM( toupper( fs_path[0] ) );
    if( drive_num < DRIVE_NUM( 'A' )
      || drive_num > DRIVE_NUM( 'Z' ) )
        drive_num = 0;
    return( drive_num );
}

static int GetDriveInfo( const char *fs_path, bool removable )
/************************************************************/
{
    int         drive_num = 0;
#if !defined( __UNIX__ )
    drive_info  *info;
#endif

#if defined( __UNIX__ )
    /* unused parameters */ (void)fs_path; (void)removable;
#else
    drive_num = GetDriveNum( fs_path );
    if( drive_num > 0 ) {
        info = &Drives[drive_num];
        if( (info->block_size == 0
          || removable /* recheck - could have been replaced */) ) {
            memset( info, 0, sizeof( *info ) );
            NoHardErrors();
    #if defined( __OS2__ )
            {
                typedef struct {
                    BYTE    cmdinfo;
                    BYTE    drive_num;
                } parm;

                typedef struct {
                    BYTE    bpb[31];
                    short   cyl;
                    BYTE    type;
                    short   attrs;
                } ret;

                #define BUF_SIZE    40

                UCHAR           fsinfobuf[BUF_SIZE];
                APIRET          rc;

                ULONG           sectors_per_cluster;    /* sectors per allocation unit */
                ULONG           free_clusters;          /* available units */
                ULONG           bytes_per_sector;       /* bytes per sector */
                parm            p;
                ret             r;
                ULONG           parmLengthInOut;
                ULONG           dataLengthInOut;
                char            dev[3];
                struct {
                    FSQBUFFER2  b;
                    char        stuff[100];
                } dataBuffer;
                ULONG           dataBufferLen;

                rc = DosQueryFSInfo( (ULONG)drive_num, FSIL_ALLOC, fsinfobuf, BUF_SIZE );

                if( rc == 0 ) {
                    /*
                     * This is a bit strange: the respective values are not
                     * returned in a structure but in an array BLAH! from
                     * which one must extract info as below. See OS/2 Manual
                     * for clarification.
                     */
                    sectors_per_cluster = *(ULONG *)(fsinfobuf + (1 * sizeof( ULONG )));
                    free_clusters = *(ULONG *)(fsinfobuf + (3 * sizeof( ULONG )));
                    bytes_per_sector = *(USHORT *)(fsinfobuf + (4 * sizeof( ULONG )));
                    info->block_size = sectors_per_cluster * bytes_per_sector;
                    info->free_space = (disk_size)free_clusters * (ULONG)info->block_size;
                } else {
                    info->free_space = (disk_size)-1;
                }
                info->fixed = false;
                info->removable = false;
                p.cmdinfo = 0;
                p.drive_num = drive_num - 1;
                parmLengthInOut = sizeof( p );
                dataLengthInOut = sizeof( r );
                if( DosDevIOCtl( -1, 8, 0x63, &p, sizeof( p ), &parmLengthInOut, &r,
                                 sizeof( r ), &dataLengthInOut ) == 0 ) {
                    dev[0] = fs_path[0];
                    dev[1] = ':';
                    dev[2] = '\0';
                    dataBufferLen = sizeof( dataBuffer );
                    rc = DosQueryFSAttach( dev, 0, FSAIL_QUERYNAME, (PFSQBUFFER2)&dataBuffer,
                                           &dataBufferLen );
                    if( rc == 0 ) {
                        if( dataBuffer.b.iType == FSAT_LOCALDRV ) {
                            switch( r.type ) {
                            case 5: /* fixed disk */
                                info->fixed = true;
                                break;
                            case 6: /* tape */
                                break;
                            default: /* removable media */
                                info->removable = true;
                                break;
                            }
                        }
                    } else if( rc == ERROR_NOT_READY ) {
                        /*
                         * removable media not in drive
                         */
                        info->removable = true;
                    }
                }
            }
    #elif defined( __NT__ )
            {
                char        root[4];
                DWORD       sectors_per_cluster;
                DWORD       bytes_per_sector;
                DWORD       free_clusters;
                DWORD       total_clusters;
                UINT        drive_type;

                root[0] = fs_path[0];
                strcpy( &root[1], ":\\" );
                if( GetDiskFreeSpace( root, &sectors_per_cluster, &bytes_per_sector,
                                      &free_clusters, &total_clusters ) ) {
                    info->block_size = bytes_per_sector * sectors_per_cluster;
                    info->free_space = (disk_size)free_clusters * (disk_size)info->block_size;
                } else {
                    info->free_space = (disk_size)-1;
                }
                drive_type = GetDriveType( root );
                info->removable = ( drive_type == DRIVE_REMOVABLE );
                info->fixed = ( drive_type == DRIVE_FIXED );
            }
    #else
            {
                struct diskfree_t   FreeSpace;
                union REGS          r;

                info->removable = false;
                info->fixed = false;
                info->block_size = 0;
                info->free_space = (disk_size)-1;
                r.w.ax = 0x440E;    /* get logical drive */
                r.w.bx = drive_num;
                intdos( &r, &r );
                if( r.w.cflag
                  || (r.h.al
                  && (r.h.al != drive_num)) ) {
                    return( drive_num );
                }
                info->fixed = true;
                r.w.ax = 0x4409;    /* query device local/remote */
                r.w.bx = drive_num;
                intdos( &r, &r );
                if( r.w.cflag == 0
                  && (r.w.dx & 0x1000) ) {
                    info->fixed = false;
                }
                r.w.ax = 0x4408;    /* query device removable */
                r.w.bx = drive_num;
                intdos( &r, &r );
                if( r.w.cflag == 0 ) {
                    info->removable = ( r.w.ax == 0 );
                    if( info->removable ) {
                        info->fixed = false;
                    }
                } else {
                    info->fixed = false;
                }
                if( _getdiskfree( drive_num, &FreeSpace ) == 0 ) {
                    info->block_size = (unsigned long)FreeSpace.sectors_per_cluster *
                                         FreeSpace.bytes_per_sector;
                    info->free_space = FreeSpace.avail_clusters *
                                       (disk_size)info->block_size;
                    /*
                     * If reported cluster size is ridiculously large,
                     * it's likely faked; assume the real cluster size is
                     * much smaller - 4096 should be a conservative estimate.
                     */
                    if( info->block_size > 64 * 1024UL ) {
                        info->block_size = 4096;
                    }
                } else if( removable ) {
                    /*
                     * removable media not present
                     */
                    info->block_size = 0;
                    info->free_space = (disk_size)-1;
                } else {
                    /*
                     * doesn't work on network drive - assume 4K cluster,
                     * max free
                     */
                    info->block_size = 4096;
                    info->free_space = (disk_size)-1;
                }
            }
    #endif
            if( !removable ) {
                int         io;
                VBUF        path;

                VbufInit( &path );
                GetTmpFileNameInTarget( fs_path, &path );
                io = open_vbuf( &path, O_RDWR | O_CREAT | O_TRUNC, PMODE_RW );
                if( io == -1 ) {
                    GetTmpFileName( fs_path, &path );
                    io = open_vbuf( &path, O_RDWR | O_CREAT | O_TRUNC, PMODE_RW );
                    info->use_target_for_tmp_file = false;
                } else {
                    info->use_target_for_tmp_file = true;
                }
                if( io != -1 ) {
                    close( io );
                    remove_vbuf( &path );
#if 0
    // FIXME it doesn't work correctly if target directory doesn't exist
    // (new installation) and you have insufficient rights to drive root
                } else {
                    info->block_size = 1;
                    info->free_space = (disk_size)-1;
#endif
                }
                VbufFree( &path );
            }
        }
    }
#endif
    return( drive_num );
}
#endif

static disk_size GetDriveInfoFreeSpace( const char *fs_path, bool removable )
/***************************************************************************/
{
#if defined( __UNIX__ )
    (void)fs_path;
    (void)removable;
    return( -1 );
#else
    return( Drives[GetDriveInfo( fs_path, removable )].free_space );
#endif
}

void ResetDiskInfo( void )
/************************/
{
#if !defined( __UNIX__ )
    memset( Drives, 0, sizeof( Drives ) );
#endif
}

unsigned GetBlockSize( const char *fs_path )
/******************************************/
{
    unsigned    ret = 1;

#if defined( __UNIX__ )
    /* unused parameters */ (void)fs_path;
#else
    if( fs_path != NULL ) {
        ret = Drives[GetDriveInfo( fs_path, false )].block_size;
    }
#endif
    return( ret );
}

#if defined( UNC_SUPPORT )
static bool GetRootFromPathUNC( VBUF *root, const char *fs_path )
/***************************************************************/
{
    char        curr_dir[_MAX_PATH];
    VBUF        temp_vbuf;

    if( fs_path == NULL
      || root == NULL ) {
        return( false );
    }
    VbufRewind( root );
    if( TEST_DRIVE( fs_path ) ) {
        /*
         * turn a path like "c:\dir" into "c:\"
         */
        VbufConcChr( root, fs_path[0] );
        VbufConcChr( root, fs_path[1] );
        VbufConcChr( root, DIR_SEP );
        return( true );
    }
    if( TEST_UNC( fs_path ) ) {
        const char  *index;
        int         i;
        char        c;

        /*
         * turn a UNC name like "\\root\share\dir\subdir" into "\\root\share\"
         */
        index = fs_path;
        i = 0;
        while( (c = *index++) != '\0' ) {
            if( IS_DIR_SEP( c ) ) {
                i++;
            }
            if( i == 4 ) {
                /*
                 * cut off string at character after 4th backslash
                 */
                VbufConcBuffer( root, fs_path, index - fs_path );
                return( true );
            }
        }
        /*
         * invalid UNC name such as: "\\missingshare\"
         */
        return( false );
    }
    /*
     * for relative paths like "\dir" use the current drive.
     */
    if( getcwd( curr_dir, sizeof( curr_dir ) ) == NULL ) {
        return( false );
    }
    VbufInit( &temp_vbuf );
    VbufConcStr( &temp_vbuf, curr_dir );
    VbufSplitpath( &temp_vbuf, root, NULL, NULL, NULL );
    VbufAddDirSep( root );
    VbufFree( &temp_vbuf );
    return( true );
}

static disk_size GetDiskFreeSpaceUNC( const char *fs_path )
/*********************************************************/
{
    disk_size   size = 0;
    struct diskfree_t info;
#ifdef __NT__
    VBUF        root;
#endif

#ifdef __NT__
    VbufInit( &root );
    if( GetRootFromPathUNC( &root, fs_path ) ) {
        if( GetDiskFreeSpace( VbufString( &root ), &info.sectors_per_cluster,
                &info.bytes_per_sector, &info.avail_clusters,
                &info.total_clusters ) ) {
            size = (disk_size)info.sectors_per_cluster * (disk_size)info.bytes_per_sector * (disk_size)info.avail_clusters;
        }
    }
    VbufFree( &root );
#else
    if( TEST_DRIVE( fs_path ) ) {
        if( _getdiskfree( GetDriveNum( fs_path ), &info ) == 0 ) {
            size = (disk_size)info.sectors_per_cluster * (disk_size)info.bytes_per_sector * (disk_size)info.avail_clusters;
        }
    }
#endif
    return( size );
}

#if 0
static long GetBlockSizeUNC( const char *fs_path )
/************************************************/
{
    long        size = 0;
    struct diskfree_t info;
#ifdef __NT__
    VBUF        root;
#endif

#ifdef __NT__
    VbufInit( &root );
    if( GetRootFromPathUNC( &root, fs_path ) ) {
        if( GetDiskFreeSpace( VbufString( &root ), &info.sectors_per_cluster,
                &info.bytes_per_sector, &info.avail_clusters,
                &info.total_clusters ) ) {
            size = (long)info.sectors_per_cluster * info.bytes_per_sector;
        }
    }
    VbufFree( &root );
#else
    if( TEST_DRIVE( fs_path ) ) {
        if( _getdiskfree( GetDriveNum( fs_path ), &info ) == 0 ) {
            size = (long)info.sectors_per_cluster * info.bytes_per_sector;
        }
    }
#endif
    return( size );
}
#endif

static bool IsDriveWritableUNC( const char *fs_path )
/***************************************************/
{
    int         io;
    VBUF        temp_vbuf;
    VBUF        root;
    bool        ok;

    if( fs_path == NULL ) {
        return( false );
    }

    VbufInit( &root );
    VbufInit( &temp_vbuf );

    ok = GetRootFromPathUNC( &root, fs_path ) != 0;
    if( ok ) {
        GetTmpFileNameUNC( VbufString( &root ), &temp_vbuf );
        io = open_vbuf( &temp_vbuf, O_RDWR | O_CREAT | O_TRUNC, PMODE_RW );
        ok = io != -1;
        if( ok ) {
            close( io );
            remove_vbuf( &temp_vbuf );
        }
    }

    VbufFree( &temp_vbuf );
    VbufFree( &root );
    return( ok );
}
#endif

#ifdef UNC_SUPPORT
static bool DriveInfoIsAvailableUNC( const char *fs_path )
/********************************************************/
{
    bool        ok = false;
    struct diskfree_t info;
#ifdef __NT__
    VBUF        root;
#endif

#ifdef __NT__
    VbufInit( &root );
    if( GetRootFromPathUNC( &root, fs_path ) ) {
        if( GetDiskFreeSpace( VbufString( &root ), &info.sectors_per_cluster,
                &info.bytes_per_sector, &info.avail_clusters,
                &info.total_clusters ) ) {
            ok = true;
        }
    }
    VbufFree( &root );
#else
    if( TEST_DRIVE( fs_path ) ) {
        if( _getdiskfree( GetDriveNum( fs_path ), &info ) == 0 ) {
            ok = true;
        }
    }
#endif
    return( ok );
}
#endif

/********************************************************************
 * Functions for Creating the destination directory tree
 */

static void RemoveDstDir( int dir_index, VBUF *buff )
/***************************************************/
{
    int         child;
    int         max_dirs;

    SimDirNoEndSlash( dir_index, buff );
    if( access_vbuf( buff, F_OK ) != 0 )
        return;
    max_dirs = SimNumDirs();
    for( child = 0; child < max_dirs; ++child ) {
        if( SimDirParent( child ) == dir_index ) {
            RemoveDstDir( child, buff );
        }
    }
    if( SimDirParent( dir_index ) == -1 ) {
//        return; /* leave root dir (for config.new, etc) */
    }
    SimDirNoEndSlash( dir_index, buff );
    rmdir_vbuf( buff );
}


static void MakeParentDir( const VBUF *dir, pgroup2 *pg )
/*******************************************************/
{
    char                *end;
    size_t              dir_len;
    VBUF                parent;

    _splitpath2( VbufString( dir ), pg->buffer, &pg->drive, &pg->dir, NULL, NULL );
    if( pg->dir[0] == '\0' )
        return;
    dir_len = strlen( pg->dir );
    end = pg->dir + dir_len - 1;
    if( IS_DIR_SEP( *end ) )
        *end = '\0';
    if( pg->dir[0] == '\0' )
        return;
    VbufInit( &parent );
    VbufConcStr( &parent, pg->drive );
    VbufConcStr( &parent, pg->dir );
    MakeParentDir( &parent, pg );
#if defined( __UNIX__ )
    mkdir_vbuf( &parent, PMODE_RWX );
#else
    mkdir_vbuf( &parent );
#endif
    VbufFree( &parent );
}


static bool CreateDstDir( int i, VBUF *buff )
/********************************************
 * check for directory existence
 * if dir exists return true
 * else try and create directory
 */
{
    bool                ok;
    int                 parent;
    pgroup2             pg;

    parent = SimDirParent( i );
    if( parent != -1 ) {
        ok = CreateDstDir( parent, buff );
        if( !ok ) {
            return( false );
        }
    }
    SimDirNoEndSlash( i, buff );
    /*
     * check for existence
     */
    if( access_vbuf( buff, F_OK ) == 0 )
        return( true );
    MakeParentDir( buff, &pg );
#if defined( __UNIX__ )
    if( mkdir_vbuf( buff, PMODE_RWX ) == 0 )
#else
    if( mkdir_vbuf( buff ) == 0 )
#endif
        return( true );
    MsgBoxVbuf( NULL, "IDS_CANTMAKEDIR", GUI_OK, buff );
    return( false );
}

#define KB  1024UL
#define MB  (KB * KB)

static void catnum( char *buff, long long num )
/*********************************************/
{

    char        num_buff[MAXBUF];
    char        ch;

    ch = ' ';
    if( num < 0 ) {
        num = -num;
        ch = '-';
    }
    if( num < KB ) {
        sprintf( num_buff, "%c%d bytes", ch, (int)num );
    } else {
        num /= KB;
        if( num > MB ) {
            num /= KB;
            sprintf( num_buff, "%c%d,%3.3dMB", ch, (int)(num / 1000),
                     (int)(num % 1000) );
        } else if( num > KB ) {
            sprintf( num_buff, "%c%d,%3.3dKB", ch, (int)(num / 1000),
                     (int)(num % 1000) );
        } else {
            sprintf( num_buff, "%c%dKB", ch, (int)num );
        }
    }
    strcat( buff, num_buff );
}

static void ucatnum( char *buff, unsigned long long num )
/*******************************************************/
{

    char        num_buff[MAXBUF];
    char        ch;

    ch = ' ';
    if( num < KB ) {
        sprintf( num_buff, "%c%u bytes", ch, (int)num );
    } else {
        num /= KB;
        if( num > MB ) {
            num /= KB;
            sprintf( num_buff, "%c%u,%3.3uMB", ch, (int)(num / 1000),
                     (int)(num % 1000) );
        } else if( num > KB ) {
            sprintf( num_buff, "%c%u,%3.3uKB", ch, (int)(num / 1000),
                     (int)(num % 1000) );
        } else {
            sprintf( num_buff, "%c%uKB", ch, (int)num );
        }
    }
    strcat( buff, num_buff );
}

static void free_disks( char **unc_disks, int max_targets )
{
    int         i;

    for( i = 0; i < max_targets; i++ ) {
        GUIMemFree( unc_disks[i] );
    }
}

bool CheckDrive( bool issue_message )
/************************************
 * check if there is enough disk space
 */
{
    bool                ok;
    disk_size           free_disk_space;
    disk_ssize          disk_space_needed;
    int                 max_targets;
    int                 i, j, targ_num;
    char                **unc_disks;
    bool                *disk_counted;
    VBUF                temp_vbuf;
#if !defined( __UNIX__ )
    gui_message_return  reply;
#endif
    char                buff[_MAX_PATH];
    char                drive_freesp[20];
    struct drive_space {
        char        *unc_drive;
        disk_ssize  needed;
        disk_size   free;
        int         num_files;
    }                   *space;
#ifdef UNC_SUPPORT
    VBUF                unc_root1;
    VBUF                unc_root2;
#endif

#if defined( UNC_SUPPORT ) || defined( __UNIX__ )
    /* unused parameters */ (void)issue_message;
#endif

    if( !SimCalcTargetSpaceNeeded() )
        return( false );
    max_targets = SimNumTargets();
    unc_disks = GUIMemAlloc( max_targets * sizeof( *unc_disks ) );
    disk_counted = GUIMemAlloc( max_targets * sizeof( *disk_counted ) );
    space = GUIMemAlloc( max_targets * sizeof( *space ) );
    ok = true;
    VbufInit( &temp_vbuf );
    for( i = 0; i < max_targets; i++ ) {
        /*
         * get drive letter for each target (actually the path including
         * the drive letter)
         */
        if( SimGetTargetFullPath( i, &temp_vbuf ) == NULL ) {
            ok = false;
            max_targets = i;
            break;
        }
        VbufAddDirSep( &temp_vbuf );
        unc_disks[i] = GUIStrDup( VbufString( &temp_vbuf ), NULL );
        disk_counted[i] = false;
    }
    VbufFree( &temp_vbuf );
    if( ok ) {
#ifdef UNC_SUPPORT
        VbufInit( &unc_root1 );
        VbufInit( &unc_root2 );
#endif
        /*
         * check for enough disk space, combine drives that are the same
         */
        for( i = 0; i < max_targets; i++ ) {
            if( !disk_counted[i] ) {
                targ_num = i;
                disk_space_needed = SimTargetSpaceNeeded( i );
                for( j = i + 1; j < max_targets; ++j ) {
#ifdef UNC_SUPPORT
                    GetRootFromPathUNC( &unc_root1, unc_disks[i] );
                    GetRootFromPathUNC( &unc_root2, unc_disks[j] );
                    /*
                     * identical drives are combined, and so are UNC paths
                     * pointing to the same share
                     * BUT: drives and UNC paths that happen to be the same
                     * are NOT combined. (I am lazy)
                     */
                    if( ( tolower( *unc_disks[j] ) == tolower( *unc_disks[i] )
                      && isalpha( *unc_disks[i] ) )
                      || VbufCompVbuf( &unc_root1, &unc_root2, true ) == 0 ) {
#else
                    if( tolower( *unc_disks[j] ) == tolower( *unc_disks[i] )
                      && isalpha( *unc_disks[i] ) ) {
#endif
                        targ_num = j;
                        disk_space_needed += SimTargetSpaceNeeded( j );
                        disk_counted[j] = true;
                    }
                }
#ifdef UNC_SUPPORT
                if( TEST_UNC( unc_disks[i] ) ) {
                    if( !IsDriveWritableUNC( unc_disks[i] ) ) {
                        if( issue_message ) {
                            GetRootFromPathUNC( &unc_root1, unc_disks[i] );
                            if( access_vbuf( &unc_root1, F_OK ) == 0 ) {
                                MsgBoxVbuf( NULL, "IDS_UNCPATH_NOTWRITABLE", GUI_OK, &unc_root1 );
                            } else {
                                MsgBox( NULL, "IDS_UNCPATH_NOTEXIST", GUI_OK, &unc_root1 );
                            }
                            ok = false;
                            break;
                        }
                    }
                    free_disk_space = GetDiskFreeSpaceUNC( unc_disks[i] );
                } else {
#endif
                    free_disk_space = GetDriveInfoFreeSpace( unc_disks[i], false );
#ifdef UNC_SUPPORT
                }
#endif
                if( free_disk_space == (unsigned long long)-1 )
                    free_disk_space = 0;
                space[i].unc_drive = unc_disks[i];
                space[i].free = free_disk_space;
                space[i].needed = disk_space_needed;
                space[i].num_files = SimGetTargetNumFiles( targ_num );
#if !defined( __UNIX__ )
                if( issue_message ) {
                    if( disk_space_needed > 0
                      && free_disk_space < (disk_size)disk_space_needed ) {
    #ifdef UNC_SUPPORT
                        if( TEST_UNC( unc_disks[i] ) ) {
                            if( DriveInfoIsAvailableUNC( unc_disks[i] ) ) {
                                reply = MsgBox( NULL, "IDS_NODISKSPACE_UNC", GUI_YES_NO,
                                                unc_disks[i], free_disk_space / 1000,
                                                disk_space_needed / 1000 );
                            } else {
                                GetRootFromPathUNC( &unc_root1, unc_disks[i] );
                                reply = MsgBoxVbuf( NULL, "IDS_ASSUME_ENOUGHSPACE", GUI_YES_NO, &unc_root1 );
                            }
                        } else {
    #endif
                            reply = MsgBox( NULL, "IDS_NODISKSPACE", GUI_YES_NO, *unc_disks[i],
                                            free_disk_space / 1000,
                                            disk_space_needed / 1000 );
    #ifdef UNC_SUPPORT
                        }
    #endif
                        if( reply == GUI_RET_NO ) {
                            ok = false;
                            break;
                        }
                    }
                }
#endif
            }
        }
#ifdef UNC_SUPPORT
        VbufFree( &unc_root2 );
        VbufFree( &unc_root1 );
#endif
    }
    if( ok ) {
#ifdef UNC_SUPPORT
        VbufInit( &unc_root1 );
#endif
        for( i = 0; i < max_targets; ++i ) {
            strcpy( drive_freesp, "DriveFreeN" );
            if( *space[i].unc_drive != '\0'
              && SimTargetNeedsUpdate( i ) ) {
#ifdef UNC_SUPPORT
                if( TEST_UNC( space[i].unc_drive ) ) {
                    GetRootFromPathUNC( &unc_root1, space[i].unc_drive );
                    sprintf( buff, GetVariableStrVal( "IDS_DRIVE_SPEC_UNC" ), &unc_root1 );
                } else {
#endif
                    sprintf( buff, GetVariableStrVal( "IDS_DRIVE_SPEC" ), toupper( *space[i].unc_drive ) );
#ifdef UNC_SUPPORT
                }
#endif
                if( space[i].needed < 0 ) {
                    catnum( buff, -space[i].needed );
                    strcat( buff, GetVariableStrVal( "IDS_DRIVE_FREED" ) );
                } else {
                    catnum( buff, space[i].needed );
                    strcat( buff, GetVariableStrVal( "IDS_DRIVE_REQUIRED" ) );
                    ucatnum( buff, space[i].free );
                    strcat( buff, GetVariableStrVal( "IDS_DRIVE_AVAILABLE" ) );
                }
            } else {
                buff[0] = '\0';
            }
            drive_freesp[strlen( drive_freesp ) - 1] = i + 1 + '0';
#ifdef UNC_SUPPORT
            if( TEST_UNC( space[i].unc_drive )
              && (!DriveInfoIsAvailableUNC( space[i].unc_drive )
              || !IsDriveWritableUNC( space[i].unc_drive )) ) {
                strcpy( buff, "" );
            }
#endif
            SetVariableByName( drive_freesp, buff );
        }
#ifdef UNC_SUPPORT
        VbufFree( &unc_root1 );
#endif
    }
    free_disks( unc_disks, max_targets );
    GUIMemFree( unc_disks );
    GUIMemFree( disk_counted );
    GUIMemFree( space );
    return( ok );
}

static void SetFileDate( const VBUF *dst_path, time_t date )
/**********************************************************/
{
    struct utimbuf      timebuf;

    timebuf.modtime = date;
    timebuf.actime = date;
    utime_vbuf( dst_path, &timebuf );
}

static void SameFileDate( const VBUF *src_path, const VBUF *dst_path )
/********************************************************************/
{
    struct stat         statblk;

    FileStat( src_path, &statblk );
    SetFileDate( dst_path, statblk.st_mtime );
}

/********************************************************************
 * Functions for Deleting Files
 */

bool DoDeleteFile( const VBUF *path )
/***********************************/
{
    return( remove_vbuf( path ) == 0 );
}

/********************************************************************
 * Functions for Copying Files
 */

COPYFILE_ERROR DoCopyFile( const VBUF *src_path, const VBUF *dst_path, copy_mode copymode )
/*****************************************************************************************/
{
    static char         lastchance[1024];
    size_t              buffer_size = 16 * 1024;
    file_handle         src_files;
    int                 dst_files;
    int                 bytes_read, bytes_written, style;
    char                *pbuff;
    COPYFILE_ERROR      ret;

    src_files = FileOpen( src_path, DATA_BIN );
    if( src_files == NULL ) {
        return( CFE_CANTOPENSRC );
    }

    for( ;; ) {
        pbuff = GUIMemAlloc( buffer_size );
        if( pbuff != NULL )
            break;
        buffer_size >>= 1;
        if( buffer_size < sizeof( lastchance ) ) {
            pbuff = lastchance;
            buffer_size = sizeof( lastchance );
            break;
        }
    }

    if( copymode & COPY_APPEND ) {
        style = O_RDWR | O_BINARY;
    } else {
        style = O_CREAT | O_TRUNC | O_WRONLY | O_BINARY;
    }
    dst_files = open_vbuf( dst_path, style, PMODE_R_USR_W );
    if( dst_files == -1 ) {
        FileClose( src_files );
        if( pbuff != lastchance )
            GUIMemFree( pbuff );
        dst_files = open_vbuf( dst_path, O_RDONLY );
        if( dst_files != -1 ) {
            /*
             * read only file
             */
            close( dst_files );
            return( CFE_DSTREADONLY );
        }
        return( CFE_CANTOPENDST );
    }
    if( copymode & COPY_APPEND ) {
        lseek( dst_files, 0, SEEK_END );
    }

    ret = CFE_NOERROR;
    do {
        bytes_read = FileRead( src_files, pbuff, buffer_size );
        if( bytes_read < 0 ) {
            SetupError( "IDS_READERROR" );
            ret = CFE_ERROR;
            break;
        }
        bytes_written = write( dst_files, pbuff, bytes_read );
        BumpStatus( bytes_written );
        if( bytes_written != bytes_read
          || StatusCancelled() ) {
            if( bytes_written == bytes_read ) {
                /*
                 * copy was aborted, delete destination file
                 */
                ret = CFE_ABORT;
                break;
            }
            /*
             * error writing file - probably disk full
             */
            SetupError( "IDS_WRITEERROR" );
            ret = CFE_ERROR;
            break;
        }
    } while( bytes_read == buffer_size );
    close( dst_files );
    FileClose( src_files );
    if( pbuff != lastchance )
        GUIMemFree( pbuff );
    if( ret == CFE_NOERROR ) {
        /*
         * Make the destination file have the same time stamp
         * as the source file.
         */
        SameFileDate( src_path, dst_path );
    } else if( ret == CFE_ABORT ) {
        DoDeleteFile( dst_path );
    } else if( ret == CFE_ERROR ) {
        DoDeleteFile( dst_path );
    }
    return( ret );
}


#define DIR_THRESHOLD   10

typedef struct split_file {
    struct split_file   *next;
    char                *src_path;
} split_file;

#define OVERHEAD_SIZE 10000
/*
 * removing a file is about like copying a small file
 */
static bool CreateDirectoryTree( void )
/*************************************/
{
    long                num_total_install;
    long                num_installed;
    int                 i;
    VBUF                dst_path;
    int                 max_dirs;
    bool                ok;

    max_dirs = SimNumDirs();
    num_total_install = 0;
    for( i = 0; i < max_dirs; i++ ) {
        if( SimDirUsed( i ) ) {
            ++num_total_install;
        }
    }
    if( num_total_install != 0 ) {
        StatusLines( STAT_CREATEDIRECTORY, "" );
        StatusAmount( 0, num_total_install );
    }
    VbufInit( &dst_path );
    num_installed = 0;
    ok = true;
    for( i = 0; i < max_dirs; i++ ) {
        if( SimDirUsed( i ) ) {
            if( !CreateDstDir( i, &dst_path ) ) {
                ok = false;
                break;
            }
            StatusLinesVbuf( STAT_SAME, &dst_path );
            StatusAmount( ++num_installed, num_total_install );
            if( StatusCancelled() ) {
                ok = false;
                break;
            }
        }
    }
    VbufFree( &dst_path );
    if( num_total_install != 0 ) {
        StatusLines( STAT_SAME, "" );
        StatusAmount( num_total_install, num_total_install );
    }
    return( ok );
}

static copy_mode getCopyMode( int parm, int subfile )
/***************************************************/
{
    if( SimSubFileTextCRLF( parm, subfile ) ) {
        if( GetVariableBoolVal( "IsDos" )
          || GetVariableBoolVal( "IsWin16" )
          || GetVariableBoolVal( "IsWin95" ) ) {
            return( COPY_TEXT_CRLF );
        }
    }
    return( COPY_NORMAL );
}

static bool RelocateFiles( void )
/*******************************/
{
    int                 filenum;
    int                 subfilenum, max_subfiles;
    long                num_total_install;
    long                num_installed;
    VBUF                dst_path;
    VBUF                src_path;
    VBUF                dir;
    VBUF                file_desc;
    int                 max_files = SimNumFiles();
    bool                ok;

    num_total_install = 0;
    for( filenum = 0; filenum < max_files; filenum++ ) {
        if( SimFileRemove( filenum ) )
            continue;
        max_subfiles = SimNumSubFiles( filenum );
        for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
            if( SimSubFileInOldDir( filenum, subfilenum ) ) {
                num_total_install += SimSubFileSize( filenum, subfilenum );
            }
        }
    }

    if( num_total_install != 0 ) {
        StatusLines( STAT_RELOCATING, "" );
        StatusAmount( 0, num_total_install );
    }
    VbufInit( &dir );
    VbufInit( &file_desc );
    VbufInit( &dst_path );
    VbufInit( &src_path );
    num_installed = 0;
    ok = true;
    for( filenum = 0; ok && filenum < max_files; filenum++ ) {
        if( SimFileRemove( filenum ) )
            continue;
        max_subfiles = SimNumSubFiles( filenum );
        for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
            if( SimSubFileInOldDir( filenum, subfilenum ) ) {
                SimFileOldDir( filenum, &dir );
                SimSubFileName( filenum, subfilenum, &file_desc );
                VbufMakepath( &src_path, NULL, &dir, &file_desc, NULL );
                SimFileDir( filenum, &dir );
                VbufMakepath( &dst_path, NULL, &dir, &file_desc, NULL );
                StatusLinesVbuf( STAT_SAME, &src_path );
                if( SimSubFileInNewDir( filenum, subfilenum ) ) {
                    remove_vbuf( &dst_path );
                }
                if( DoCopyFile( &src_path, &dst_path, getCopyMode( filenum, subfilenum ) ) != CFE_NOERROR ) {
                    ok = false;
                    break;
                }
                if( SimSubFileExecutable( filenum, subfilenum ) ) {
                    chmod_vbuf( &dst_path, PMODE_RX_USR_W );
                }
                remove_vbuf( &src_path );
                num_installed += SimSubFileSize( filenum, subfilenum );
                StatusAmount( num_installed, num_total_install );
            }
        }
    }
    VbufFree( &src_path );
    VbufFree( &dst_path );
    VbufFree( &file_desc );
    VbufFree( &dir );
    if( num_total_install != 0 ) {
        StatusLines( STAT_RELOCATING, "" );
        StatusAmount( num_total_install, num_total_install );
    }
    return( ok );
}


typedef struct file_check {
    struct file_check   *next;
    VBUF                name;
    vhandle             var_handle;
    bool                is_dll;
} file_check;

static file_check *fileCheck = NULL;
static file_check *fileCheckThisPack = NULL;

static void NewFileToCheck( const VBUF *name, bool is_dll )
/*********************************************************/
{
    file_check  *new;

    new = GUIMemAlloc( sizeof( *new ) );
    new->next = fileCheckThisPack;
    fileCheckThisPack = new;
    VbufInit( &new->name );
    VbufConcVbuf( &new->name, name );
    new->var_handle = NO_VAR;
    new->is_dll = is_dll;
}

static void UpdateCheckList( const VBUF *name, vhandle var_handle )
/*****************************************************************/
{
    file_check  *check;

    for( check = fileCheckThisPack; check != NULL; check = check->next ) {
#if defined( __UNIX__ )
        if( VbufCompVbuf( name, &check->name, false ) == 0 ) {
#else
        if( VbufCompVbuf( name, &check->name, true ) == 0 ) {
#endif
            check->var_handle = var_handle;
        }
    }
}

static void TransferCheckList( void )
/***********************************/
{
    file_check  *check;
    file_check  *next;

    for( check = fileCheckThisPack; check != NULL; check = next ) {
        next = check->next;
        check->next = fileCheck;
        fileCheck = check;
    }
    fileCheckThisPack = NULL;
}

static bool CheckPendingFiles( void )
/***********************************/
{
    file_check  *curr;
    file_check  *next;
    bool        cancel;

    for( curr = fileCheck; curr != NULL; curr = next ) {
        next = curr->next;
        if( curr->is_dll ) {
            cancel = CheckInstallDLL( &curr->name, curr->var_handle );
        } else {
            cancel = CheckInstallNLM( &curr->name, curr->var_handle );
        }
        if( cancel )
            return( false );
        VbufFree( &curr->name );
        GUIMemFree( curr );
    }
    return( true );
}

static void CopySetupInfFile( void )
/**********************************/
{
    VBUF                dst_path;
    VBUF                temp_vbuf;
    VBUF                fname;

    VbufInit( &dst_path );
    VbufInit( &temp_vbuf );
    VbufInit( &fname );
    /*
     * if DoCopyInf variable is set, copy/delete setup.inf
     */
    VbufSetStr( &temp_vbuf, GetVariableStrVal( "DoCopyInf" ) );
    if( VbufLen( &temp_vbuf ) > 0 ) {
        ReplaceVars1( &temp_vbuf );
        VbufSetStr( &fname, "setup.inf" );
        VbufMakepath( &dst_path, NULL, &temp_vbuf, &fname, NULL );
        if( VarGetBoolVal( UnInstall ) ) {
            remove_vbuf( &dst_path );
        } else {
            VbufSetStr( &temp_vbuf, GetVariableStrVal( "SetupInfFile" ) );
            DoCopyFile( &temp_vbuf, &dst_path, COPY_NORMAL );
        }
    }

    VbufFree( &fname );
    VbufFree( &temp_vbuf );
    VbufFree( &dst_path );
}

static bool checkForNewName( int filenum, int subfilenum, VBUF *name )
/********************************************************************/
{
    VBUF        ext;
    bool        rc;

    VbufInit( &ext );

    if( SimSubFileIsNLM( filenum, subfilenum ) ) {
        NewFileToCheck( name, false );
        VbufSetStr( &ext, "_N_" );
        VbufSetPathExt( name, &ext );
        rc = true;
    } else if( SimSubFileIsDLL( filenum, subfilenum ) ) {
        NewFileToCheck( name, true );
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
        VbufSetStr( &ext, "_D_" );
        VbufSetPathExt( name, &ext );
#endif
        rc = true;
    } else {
        rc = false;
    }

    VbufFree( &ext );
    return( rc );
}

static bool DoCopyFiles( void )
/*****************************/
{
    int                 filenum;
    int                 subfilenum, max_subfiles;
    COPYFILE_ERROR      copy_error;
//    VBUF                dst_path;
    VBUF                temp_path;
    VBUF                src_path;
//    VBUF                file_name;
    VBUF                file_desc;
    VBUF                dir;
    VBUF                temp_vbuf;
    VBUF                old_dir;
    long                num_total_install;
    long                num_installed;
    split_file          *split = NULL;
    split_file          **owner_split = &split;
    split_file          *junk;
    bool                resp_replace;
    vhandle             var_handle;
    gui_message_return  ret = GUI_RET_OK;
    int                 max_files = SimNumFiles();
    size_t              len;
    size_t              src_path_pos1;
    size_t              src_path_pos2;
    const char          *dst_dir;
    bool                ok;

    VbufInit( &dir );
    VbufInit( &old_dir );
    VbufInit( &temp_vbuf );
    VbufInit( &file_desc );
    VbufInit( &temp_path );
    VbufInit( &src_path );

    /*
     * Check files for processing
     * Calculate "num_total_install" overall value for progress status
     */
    num_total_install = 0;
    ok = true;
    for( filenum = 0; ok && filenum < max_files; filenum++ ) {
        SimFileDir( filenum, &dir );
        if( SimFileAdd( filenum )
          && !SimFileUpToDate( filenum ) ) {
            num_total_install += SimFileSize( filenum );
            max_subfiles = SimNumSubFiles( filenum );
            for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
                if( SimSubFileReadOnly( filenum, subfilenum ) ) {
                    SimSubFileName( filenum, subfilenum, &file_desc );
                    VbufMakepath( &temp_path, NULL, &dir, &file_desc, NULL );
                    if( !PromptUser( &temp_path, "ReadOnlyFile", "RO_Skip_Dialog", "RO_Replace_Old", &resp_replace ) ) {
                        ok = false;
                        break;
                    }
                    if( resp_replace ) {
                        chmod_vbuf( &temp_path, PMODE_USR_W );
                    }
                }
                if( SimSubFileNewer( filenum, subfilenum ) ) {
                    SimSubFileName( filenum, subfilenum, &file_desc );
//                    _splitpath( file_desc, NULL, NULL, NULL, file_ext );
                    VbufMakepath( &temp_path, NULL, &dir, &file_desc, NULL );
                    if( !PromptUser( &temp_path, "NewerFile", "Newer_Skip_Dialog", "Newer_Replace_Old", &resp_replace ) ) {
                        ok = false;
                        break;
                    }
                    if( resp_replace ) {
                        SetFileDate( &temp_path, SimSubFileDate( filenum, subfilenum ) - 1 );
                    }
                }
            }
        } else if( SimFileRemove( filenum ) ) {
            max_subfiles = SimNumSubFiles( filenum );
            for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
                if( !SimSubFileExists( filenum, subfilenum ) )
                    continue;
                if( SimSubFileReadOnly( filenum, subfilenum ) ) {
                    SimSubFileName( filenum, subfilenum, &file_desc );
                    VbufMakepath( &temp_path, NULL, &dir, &file_desc, NULL );
                    if( !PromptUser( &temp_path, "DeleteReadOnlyFile", "RO_Skip_Remove", "RO_Remove_Old", &resp_replace ) ) {
                        ok = false;
                        break;
                    }
                    if( resp_replace ) {
                        chmod_vbuf( &temp_path, PMODE_USR_W );
                        num_total_install += OVERHEAD_SIZE;
                    }
                } else {
                    num_total_install += OVERHEAD_SIZE;
                }
            }
        }
    }
    if( ok ) {
        num_installed = 0;
        StatusLines( STAT_COPYINGFILE, "" );
        StatusAmount( 0, num_total_install );
        /*
         * remove files first so we don't go over disk space estimate
         */
        for( filenum = 0; ok && filenum < max_files; filenum++ ) {
            if( SimFileRemove( filenum ) ) {
                SimFileDir( filenum, &dir );
                max_subfiles = SimNumSubFiles( filenum );
                for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
                    if( !SimSubFileExists( filenum, subfilenum ) )
                        continue;
                    num_installed += OVERHEAD_SIZE;
                    SimSubFileName( filenum, subfilenum, &file_desc );
                    VbufMakepath( &temp_path, NULL, &dir, &file_desc, NULL );
                    StatusLinesVbuf( STAT_REMOVING, &temp_path );
                    remove_vbuf( &temp_path );
                    if( SimSubFileInOldDir( filenum, subfilenum ) ) {
                        SimFileOldDir( filenum, &old_dir );
                        VbufMakepath( &temp_path, NULL, &old_dir, &file_desc, NULL );
                        StatusLinesVbuf( STAT_REMOVING, &temp_path );
                        remove_vbuf( &temp_path );
                    }
                    StatusAmount( num_installed, num_total_install );
                    if( StatusCancelled() ) {
                        ok = false;
                        break;
                    }
                }
            }
        }
        if( ok ) {
            /*
             * now go ahead and add files
             */
            if( FileIsPlainFS() ) {
                VbufSetStr( &src_path, GetVariableStrVal( "SrcDir" ) );
                VbufAddDirSep( &src_path );
                src_path_pos1 = VbufLen( &src_path );
            } else {
                VbufRewind( &src_path );
                src_path_pos1 = 0;
            }
            for( filenum = 0; ok && filenum < max_files; filenum++ ) {
                if( !SimFileAdd( filenum )
                  || SimFileUpToDate( filenum ) ) {
                    while( split != NULL ) {
                        junk = split;
                        split = split->next;
                        GUIMemFree( junk->src_path );
                        GUIMemFree( junk );
                    }
                    owner_split = &split;
                    *owner_split = NULL;
                    continue;
                }
                SimFileDir( filenum, &dir );
                SimGetFileDesc( filenum, &file_desc );

//                _splitpath( file_desc, NULL, NULL, NULL, file_ext );
//                VbufMakepath( &dst_path, NULL, &dir, &file_desc, NULL );

                /*
                 * nuke sub-dir and name from end of src_path
                 */
                VbufSetLen( &src_path, src_path_pos1 );
                dst_dir = GetVariableStrVal( "DstDir" );
                len = strlen( dst_dir );
                if( strncmp( dir.buf, dst_dir, len ) == 0 ) {
                    /*
                     * if 1st char to concat is a backslash, skip it
                     */
                    if( VbufString( &dir )[len] == DIR_SEP ) {
                        len++;
                    }
                } else {
                    /*
                     * use the macro as the directory name
                     * eg: cd_drive:\winsys\filename
                     */
                    SimTargetDirName( SimDirTargetNum( SimFileDirNum( filenum ) ), &temp_vbuf );
                    len = strlen( GetVariableStrVal_vbuf( &temp_vbuf ) );
                    VbufConcVbuf( &src_path, &temp_vbuf );
                }
                /*
                 * get rid of the dest directory, just keep the subdir
                 */
                VbufConcVbufPos( &src_path, &dir, len );

                src_path_pos2 = VbufLen( &src_path );
                VbufConcVbuf( &src_path, &file_desc );

                if( StatusCancelled() ) {
                    ok = false;
                    break;
                }

                max_subfiles = SimNumSubFiles( filenum );
                for( subfilenum = 0; ok && subfilenum < max_subfiles; ++subfilenum ) {
                    do {
                        SimSubFileName( filenum, subfilenum, &file_desc );
                        var_handle = SimSubFileVar( filenum, subfilenum );
                        VbufMakepath( &temp_path, NULL, &dir, &file_desc, NULL );
                        /*
                         * add name to end of src_path
                         */
                        VbufSetVbufAt( &src_path, &file_desc, src_path_pos2 );
                        StatusLinesVbuf( STAT_COPYINGFILE, &temp_path );
                        checkForNewName( filenum, subfilenum, &temp_path );
                        copy_error = DoCopyFile( &src_path, &temp_path, getCopyMode( filenum, subfilenum ) );

                        switch( copy_error ) {
                        case CFE_ABORT:
                        case CFE_ERROR:
                            ret = GUI_RET_CANCEL;
                            break;
                        case CFE_BAD_CRC:
                            MsgBoxVbuf( NULL, "IDS_BADCRC", GUI_OK, &src_path );
                            ret = GUI_RET_CANCEL;
                            break;
                        case CFE_NOERROR:
                        case CFE_DSTREADONLY:
                            copy_error = CFE_NOERROR;
                            break;
                        case CFE_NOMEMORY:
                            ret = MsgBox( NULL, "IDS_NOMEMORYCOPY", GUI_RETRY_CANCEL );
                            break;
                        case CFE_CANTOPENSRC:
                            ret = MsgBoxVbuf( NULL, "IDS_CANTOPENSRC", GUI_RETRY_CANCEL, &src_path );
                            break;
                        case CFE_CANTOPENDST:
                            ret = MsgBoxVbuf( NULL, "IDS_CANTOPENDST", GUI_RETRY_CANCEL, &temp_path );
                            break;
                        }
                        if( ret == GUI_RET_CANCEL ) {
                            ok = false;
                            break;
                        }
                    } while( copy_error != CFE_NOERROR );
                    if( ok ) {
                        if( SimSubFileExecutable( filenum, subfilenum ) ) {
                            chmod_vbuf( &temp_path, PMODE_RX_USR_W );
                        }
                        SetVariableByHandle_vbuf( var_handle, &temp_path );
                        UpdateCheckList( &temp_path, var_handle );
                    }
                }
                if( ok ) {
                    TransferCheckList();

                    num_installed += SimFileSize( filenum );
                    if( num_installed > num_total_install )
                        num_installed = num_total_install;
                    StatusAmount( num_installed, num_total_install );
                    if( StatusCancelled() ) {
                        ok = false;
                    }
                }
            }
            if( !CheckPendingFiles() ) {
                ok = false;
            }
        }
        StatusAmount( num_total_install, num_total_install );
    }

    VbufFree( &src_path );
    VbufFree( &temp_path );
    VbufFree( &file_desc );
    VbufFree( &temp_vbuf );
    VbufFree( &old_dir );
    VbufFree( &dir );
    return( ok );
}


static void RemoveUnusedDirs( void )
/**********************************/
{
    VBUF        dst_path;
    int         i;
    int         max_dirs;

    VbufInit( &dst_path );
    max_dirs = SimNumDirs();
    for( i = 0; i < max_dirs; i++ ) {
        if( !SimDirUsed( i ) ) {
            RemoveDstDir( i, &dst_path );
        }
    }
    VbufFree( &dst_path );
}

/********************************************************************
 * TODO: Figure out what supplemental files will be applicable for
 *  the UNIX environment
 */

static void RemoveExtraFiles( void )
/***********************************
 * remove supplemental files
 */
{
    bool                uninstall;

    uninstall = VarGetBoolVal( UnInstall );
    if( uninstall ) {
#if !defined( __UNIX__ )
        const char      *p;
        char            dst_path[_MAX_PATH];

        /*
         * delete saved autoexec's and config's
         */
        p = GetVariableStrVal( "DstDir" );
    #if defined( __NT__ )
        /*
         * Windows NT
         */
        strcpy( dst_path, p );
        strcat( dst_path, "\\CHANGES.ENV" );
        remove( dst_path );
    #endif
    #if defined( __NT__ ) || defined( __WINDOWS__ ) || defined( __DOS__ )
        /*
         * Windows 95, Windows 3.x, DOS
         */
        strcpy( dst_path, p );
        strcat( dst_path, "\\AUTOEXEC." BATCH_EXT_SAVED );
        remove( dst_path );
    #endif
        strcpy( dst_path, p );
        strcat( dst_path, "\\CONFIG." BATCH_EXT_SAVED );
        remove( dst_path );
#endif
        /*
         * delete saved environment setup batch script file
         */
        if( GetVariableBoolVal( "GenerateBatchFile" ) ) {
            GenerateBatchFile( uninstall );
        }
    }
}


bool CopyAllFiles( void )
/***********************/
{
    fileCheck = NULL;
    fileCheckThisPack = NULL;

    if( !CreateDirectoryTree() )
        return( false );
    if( !RelocateFiles() )
        return( false );
#if defined( __OS2__ ) && defined( GUI_IS_GUI )
    /*
     * add labels (long names) to directories
     */
    LabelDirs();
#endif
    if( !DoCopyFiles() )
        return( false );
    CopySetupInfFile();
    RemoveExtraFiles();
    RemoveUnusedDirs();
    StatusCancelled(); /* make sure display gets updated */
    return( true );
}


static bool NukePath( VBUF *path, int status )
/********************************************/
{
    DIR                 *dirp;
    struct dirent       *dire;
    bool                ok;
    size_t              path_len;
#if defined( __UNIX__ )
    struct stat         statbuf;
#endif

    ok = true;
    dirp = opendir_vbuf( path );
    if( dirp != NULL ) {
        VbufAddDirSep( path );
        path_len = VbufLen( path );
        while( (dire = readdir( dirp )) != NULL ) {
            VbufSetStrAt( path, dire->d_name, path_len );
#if defined( __UNIX__ )
            stat_vbuf( path, &statbuf );
            if( S_ISDIR( statbuf.st_mode ) ) {
#else
            if( dire->d_attr & _A_SUBDIR ) {
#endif
                if( IS_VALID_DIR( dire ) ) {
                    if( !NukePath( path, status ) ) {
                        ok = false;
                        break;
                    }
                    rmdir_vbuf( path );
                }
            } else {
#if defined( __UNIX__ )
                if( (statbuf.st_mode & S_IWUSR) == 0
                  || !S_ISREG( statbuf.st_mode ) ) {
#else
                if( dire->d_attr & (_A_RDONLY | _A_SYSTEM | _A_HIDDEN) ) {
#endif
                    chmod_vbuf( path, PMODE_USR_W );
                }
                if( remove_vbuf( path ) != 0 ) {
                    ok = false;
                    break;
                }
            }
            StatusLinesVbuf( status, path );
        }
        VbufSetLen( path, path_len );
        closedir( dirp );
    }
    return( ok );
}


/********************************************************************
 * Miscellaneous Function
 */

void DeleteObsoleteFiles( void )
/******************************/
{
    int         i, max_deletes;
    int         group;
    dlg_state   state = DLG_NEXT;
    VBUF        temp_vbuf;
    bool        *found;
    bool        found_any;

    max_deletes = SimNumDeletes();
    group = 1;
    for( i = 0; i < max_deletes; ++i ) {
        if( SimDeleteIsDialog( i ) ) {
            ++group;
        }
    }
    VbufInit( &temp_vbuf );
    found = GUIMemAlloc( sizeof( bool ) * group );
    memset( found, false, sizeof( bool ) * group );
    found_any = false;
    group = 0;
    for( i = 0; i < max_deletes; ++i ) {
        if( SimDeleteIsDialog( i ) ) {
            ++group;
        } else {
            ReplaceVars( &temp_vbuf, SimDeleteName( i ) );
            if( access_vbuf( &temp_vbuf, F_OK ) == 0 ) {
                found[group] = true;
                found_any = true;
            }
        }
    }
    group = 0;
    if( found_any ) {
        StatusLines( STAT_REMOVING, "" );
        StatusAmount( 0, 1 );
        for( i = 0; i < max_deletes; ++i ) {
            if( SimDeleteIsDialog( i ) ) {
                ++group;
            }
            if( found[group] ) {
                ReplaceVars( &temp_vbuf, SimDeleteName( i ) );
                if( SimDeleteIsDialog( i ) ) {
                    state = DoDialog( VbufString( &temp_vbuf ) );
                } else if( state == DLG_NEXT ) {
                    if( SimDeleteIsDir( i ) ) {
                        NukePath( &temp_vbuf, STAT_REMOVING );
                        rmdir_vbuf( &temp_vbuf );
                    } else {
                        StatusLinesVbuf( STAT_REMOVING, &temp_vbuf );
                        remove_vbuf( &temp_vbuf );
                    }
                }
            }
        }
    }
    VbufFree( &temp_vbuf );
    GUIMemFree( found );
}

void GetInstallName( VBUF *name )
/*******************************/
{
    VBUF        argv0;

    if( GetVariableByName( "InstallerName" ) != NO_VAR ) {
        VbufSetStr( name, GetVariableStrVal( "InstallerName" ) );
    } else {
        VbufInit( &argv0 );

        VbufSetStr( &argv0, InstallerFile );
        VbufSplitpath( &argv0, NULL, NULL, name, NULL );
//        strupr( name );

        VbufFree( &argv0 );
    }
}

void AddInstallName( VBUF *str )
/******************************/
{
    const char          *p;
    VBUF                temp_vbuf;
    VBUF                inst_name;
    size_t              len;

    VbufInit( &temp_vbuf );
    VbufInit( &inst_name );

    /*
     * DBCS should be handled on more places
     * code need rework
     */
    GetInstallName( &inst_name );
//    p = strchr( text, '@' ); no good for dbcs!!!
    p = VbufString( str );
    while( *p != '\0' ) {
        if( *p != '@' ) {
            p += GUICharLen( UCHAR_VALUE( *p ) );
            continue;
        }
        len = p - VbufString( str );
        VbufSetStr( &temp_vbuf, p + 1 );
        VbufSetVbufAt( str, &inst_name, len );
        VbufConcVbuf( str, &temp_vbuf );
        p = VbufString( str ) + len;
    }

    VbufFree( &inst_name );
    VbufFree( &temp_vbuf );
}

static void remove_ampersand( VBUF *str )
/***************************************/
{
    const char      *s;
    VBUF            temp_vbuf;
    size_t          len;

    VbufInit( &temp_vbuf );
    s = VbufString( str );
    while( *s != '\0' ) {
        if( *s != '&' ) {
            s++;
            continue;
        }
        len = s - VbufString( str );
        VbufSetStr( &temp_vbuf, s + 1 );
        VbufSetVbufAt( str, &temp_vbuf, len );
        s = VbufString( str ) + len;
    }
    VbufFree( &temp_vbuf );
}

gui_message_return MsgBox( gui_window *gui, const char *msg_id,
                                  gui_message_type wType, ... )
/*************************************************************/
{
    gui_message_return  result;
    char                msg_buf[1024];
    const char          *errormessage;
    va_list             args;
    VBUF                msg_text;
    VBUF                inst_name;

    VbufInit( &msg_text );

    if( !SkipDialogs ) {
        if( stricmp( msg_id, "IDS_NOSETUPINFOFILE" ) == 0 ) {
            /*
             * If the message is "can't find the setup.inf file", then
             * don't look up the string, because it is in the file
             * we can't find
             */
            errormessage = "The file %s cannot be found.";
        } else {
            errormessage = GetVariableStrVal( msg_id );
        }
        if( errormessage == NULL ) {
            VbufConcStr( &msg_text, GetVariableStrVal( "IDS_UNKNOWNERROR" ) );
        } else {
            va_start( args, wType );
            vsprintf( msg_buf, errormessage, args );
            va_end( args );
            VbufConcStr( &msg_text, msg_buf );
        }
    }
//    if( gui == NULL ) {
//        wType |= GUI_SYSTEMMODAL;
//    }

    if( GUIIsGUI() ) {
        if( wType & GUI_YES_NO ) {
            wType |= GUI_QUESTION;
        } else {
            wType |= GUI_INFORMATION;
        }
    }

    if( SkipDialogs ) {
        switch( wType ) {
        case GUI_ABORT_RETRY_IGNORE:
            result = GUI_RET_ABORT;
            break;
        case GUI_RETRY_CANCEL:
            result = GUI_RET_CANCEL;
            break;
        case GUI_YES_NO:
        case GUI_YES_NO + GUI_QUESTION:
        case GUI_YES_NO + GUI_INFORMATION:
            result = GUI_RET_NO;
            break;
        case GUI_YES_NO_CANCEL:
        case GUI_YES_NO_CANCEL + GUI_QUESTION:
        case GUI_YES_NO_CANCEL + GUI_INFORMATION:
            result = GUI_RET_CANCEL;
            break;
        default:
            result = GUI_RET_OK;
        }
    } else {
        VbufInit( &inst_name );

        AddInstallName( &msg_text );
        remove_ampersand( &msg_text );
        GetInstallName( &inst_name );
        result = GUIDisplayMessage( gui == NULL ? MainWnd : gui, VbufString( &msg_text ), VbufString( &inst_name ), wType );

        VbufFree( &inst_name );
    }

    VbufFree( &msg_text );
    return( result );
}


bool PromptUser( const VBUF *name, const char *dlg, const char *skip, const char *replace, bool *resp_replace )
/*************************************************************************************************************/
{
    dlg_state       return_state;

    SetVariableByName_vbuf( "FileDesc", name );
    /*
     * don't display the dialog if the user selected the "Skip dialog" option
     */
    if( !GetVariableBoolVal( skip ) ) {
        for( ;; ) {
            return_state = DoDialog( dlg );
            if( return_state != DLG_DONE
              && return_state != DLG_CANCEL )
                break;
            if( MsgBox( NULL, "IDS_QUERYABORT", GUI_YES_NO ) == GUI_RET_YES ) {
                CancelSetup = true;
                return( false );
            }
        }
    }
    *resp_replace = GetVariableBoolVal( replace );
    return( true );
}

static void AddDefine( char *def )
/*********************************
 * Process command line switch to set a variable
 *     -dvariable=value
 */
{
    char                *p;
    DEF_VAR             *var;

    p = strchr( def, '=' );
    if( p != NULL ) {
        *p = '\0';
        ++p;
        var = GUIMemAlloc( sizeof( DEF_VAR ) );
        var->variable = GUIStrDup( def, NULL );
        var->value = GUIStrDup( p, NULL );
        var->link = ExtraVariables;
        ExtraVariables = var;
    }
}

static void DefineVars( void )
/*****************************
 * Create variables specified on command line
 */
{
    DEF_VAR             *var;

    for( var = ExtraVariables; var != NULL; var = var->link ) {
        SetVariableByName( var->variable, var->value );
    }
}

static void FreeDefinedVars( void )
/**********************************
 * Create variables specified on command line
 */
{
    DEF_VAR             *var;

    while( (var = ExtraVariables) != NULL ) {
        ExtraVariables = var->link;
        GUIMemFree( var->variable );
        GUIMemFree( var->value );
        GUIMemFree( var );
    }
}

#if !defined( _M_X64 )

#if defined( __NT__ )
static bool isOS64bit( void )
{
    DWORD   version = GetVersion();

    return( version < 0x80000000 && LOBYTE( LOWORD( version ) ) >= 5 && IsWOW64() );
}
#endif

#if defined( __UNIX__ )
static bool isOS64bit( void )
{
    struct utsname name;

    return( uname( &name ) != -1 && ( stricmp( name.machine, "x86_64" ) == 0 || stricmp( name.machine, "amd64" ) == 0 ) );
}
#endif

#if defined( __NT__ ) || defined( __UNIX__ )
static bool check32bitOn64bit( void )
{
    if( isOS64bit() ) {
        char *msg =
            "You are using the 32-bit installer on a 64-bit host.\n"
            "It is recommended to use the 64-bit installer on a 64-bit host.\n"
            "Press OK button to continue with installation or Cancel button to abort it.\n";
        SetVariableByName( "IDS_USEINST64BIT", "%s");
        if( MsgBox( NULL, "IDS_USEINST64BIT", GUI_OK_CANCEL, msg ) != GUI_RET_OK ) {
            /*
             * return true to terminate installer
             */
            return( true );
        }
    }
    return( false );
}
#endif

#endif

static void dispUsage( void )
{
    char * msg =
        "Usage: @ [-options] [<arc_name> [<inf_name> [<src_path>]]]\n\n" \
        "Supported options (case insensitive):\n\n" \
        "-f=script\t\tspecify script file to override setup.inf\n" \
        "-d<name=val>\tdefine a variable for the installer\n" \
        "-i\t\tinvisible: shows no dialogs; infers -s\n" \
        "-s\t\tskips dialogs but shows install progress\n" \
        "-np\t\tdoes not create Program Manager entries\n" \
        "-ns\t\tdoes not register startup information (paths, environment)\n" \
        "\n" \
        "Optional parameters:\n\n" \
        "<arc_name>\tarchive file name" \
        "<inf_name>\tscript file name" \
        "<src_path>\tpath for " \
        ;

    SetVariableByName( "IDS_USAGE", "%s");
    MsgBox( NULL, "IDS_USAGE", GUI_OK, msg );
}

static void setInstallerFile( const char *arg )
{
    if( _cmdname( InstallerFile ) == NULL ) {
        strncpy( InstallerFile, arg, sizeof( InstallerFile ) - 1);
    }
    InstallerFile[sizeof( InstallerFile ) - 1] = '\0';
}

bool GetDirParams( int argc, char **argv, VBUF *inf_name, VBUF *src_path, VBUF *arc_name )
/****************************************************************************************/
{
    VBUF                dir;
    VBUF                drive;
    int                 i;

#if !defined( _M_X64 )
  #if defined( __NT__ ) || defined( __UNIX__ )
    if( check32bitOn64bit() ) {
        /*
         * return false to terminate installer
         */
        return( false );
    }
  #endif
#endif

    Invisible           = false;
    SkipDialogs         = false;
    VariablesFile       = NULL;
    ExtraVariables      = NULL;
    ProgramGroups       = true;
    StartupChange       = true;
    i                   = 1;

    while( i < argc ) {
#if defined( __UNIX__ )
        if( argv[i][0] == '-' ) {
#else
        if( argv[i][0] == '-'
          || argv[i][0] == '/' ) {
#endif
            switch( argv[i][1] ) {
            case '?':
                dispUsage();
                /*
                 * return false to terminate installer
                 */
                return( false );
            case 'f':
            case 'F':
                /*
                 * Process "script" file to override variables in setup.inf
                 */
                if( argv[i][2] == '='
                  && argv[i][3] != '\0'
                  && access( &argv[i][3], R_OK ) == 0 ) {
                    GUIMemFree( VariablesFile );
                    VariablesFile = GUIStrDup( &argv[i][3], NULL );
                }
                break;
            case 'd':
            case 'D':
                AddDefine( &argv[i][2] );
                break;
            case 'i':
            case 'I':
                /*
                 * No screen output (requires SkipDialogs below as well)
                 */
                Invisible = true;
                /* fall through */
            case 's':
            case 'S':
                /*
                 * Skip dialogs
                 */
                SkipDialogs = true;
                break;
            case 'n':
            case 'N':
                if( argv[i][2] == 's'
                  || argv[i][2] == 'S' ) {
                    StartupChange = false;
                } else if( argv[i][2] == 'p'
                  || argv[i][2] == 'P' ) {
                    ProgramGroups = false;
                }
                break;
            }
            i++;
        } else {
            break;
        }
    }

    setInstallerFile( argv[0] );

    if( i < argc ) {
        VbufSetStr( arc_name, argv[i++] );
    } else {
        VbufSetStr( arc_name, InstallerFile );
    }

    VbufInit( &drive );
    VbufInit( &dir );
    if( i < argc ) {
        VbufSetStr( inf_name, argv[i++] );
    } else {
        /*
         * If archive exists, expect setup.inf inside. Otherwise assume
         * it's right next to the setup executable.
         */
        if( access_vbuf( arc_name, R_OK ) == 0 ) {
            VbufSetStr( inf_name, "setup.inf" );
        } else {
            VBUF    temp_vbuf;

            VbufInit( &temp_vbuf );

            VbufSetStr( inf_name, InstallerFile );
            VbufSplitpath( inf_name, &drive, &dir, NULL, NULL );
            VbufSetStr( inf_name, "setup.inf" );
            VbufMakepath( &temp_vbuf, &drive, &dir, inf_name, NULL );
            VbufFullpath( inf_name, &temp_vbuf );

            VbufFree( &temp_vbuf );
        }
    }

    if( i < argc ) {
        VbufSetStr( src_path, argv[i] );
    } else {
        VbufSplitpath( inf_name, &drive, &dir, NULL, NULL );
        VbufMakepath( src_path, &drive, &dir, NULL, NULL );
    }
    VbufRemEndDirSep( src_path );
    VbufFree( &dir );
    VbufFree( &drive );
    return( true );
}

bool FreeDirParams( void )
/************************/
{
    FreeDefinedVars();
    GUIMemFree( VariablesFile );

    return( true );
}

static void removeTrailingSpaces( char *s )
/*****************************************/
{
    size_t  len;

    len = strlen( s );
    while( len-- > 0 && isspace( s[len] ) ) {
        s[len] = '\0';
    }
}

void ReadVariablesFile( const char *name )
/****************************************/
{
    FILE   *fp;
    char   buf[256];
    char   *line;
    char   *variable;
    char   *value;

    fp = fopen( VariablesFile, "rt" );
    if( fp == NULL ) {
        return;
    }

    while( (line = fgets( buf, sizeof( buf ), fp )) != NULL ) {
        SKIP_SPACES( line );
        if( *line == '#'
          || *line == '\0' ) {
            continue;
        }
        removeTrailingSpaces( line );
        variable = strtok( line, " =\t" );
        if( variable != NULL ) {
            value = strtok( NULL, "=\t\0" );
            if( value != NULL ) {
                SKIP_SPACES( value );
                removeTrailingSpaces( value );
                if( name == NULL
                  || stricmp( name, variable ) == 0 ) {
                    if( stricmp( value, "true" ) == 0 ) {
                        SetBoolVariableByName( variable, true );
                    } else if( stricmp( value, "false" ) == 0 ) {
                        SetBoolVariableByName( variable, false );
                    } else {
                        SetVariableByName( variable, value );
                    }
                }
            }
        }
    }
    fclose( fp );
}

bool InitInfo( const VBUF *inf_name, const VBUF *src_path )
/**********************************************************
 * initialize global vbls. and read setup.inf into memory.
 */
{
//    char                dir[_MAX_DIR];
//    char                drive[_MAX_DRIVE];
    int                 ret;

    SetVariableByName_vbuf( "SrcDir", src_path );
//    VbufSplitpath( inf_name, drive, dir, NULL, NULL );

    ret = SimInit( inf_name );
    if( ret == SIM_INIT_NOERROR ) {
        DefineVars();
        if( VariablesFile != NULL ) {
            ReadVariablesFile( NULL );
        }
        return( true );
    }
    if( ret == SIM_INIT_NOMEM ) {
        MsgBoxVbuf( NULL, "IDS_NOMEMORY", GUI_OK, inf_name );
    } else { /* SIM_INIT_NOFILE */
        MsgBoxVbuf( NULL, "IDS_NOSETUPINFOFILE", GUI_OK, inf_name );
    }
    return( false );
}

void CloseDownMessage( bool installed_ok )
/****************************************/
{
    if( installed_ok ) {
        if( VarGetBoolVal( UnInstall ) ) {
            MsgBox( NULL, "IDS_UNSETUPCOMPLETE", GUI_OK );
        } else {
            if( ConfigModified ) {
#if defined( __NT__ )
                MsgBox( NULL, "IDS_NT_REBOOT", GUI_OK );
#elif defined( __OS2__ )
                MsgBox( NULL, "IDS_OS2_REBOOT", GUI_OK );
#elif defined( __DOS__ )
                MsgBox( NULL, "IDS_DOS_REBOOT", GUI_OK );
#elif defined( __WINDOWS__ )
                MsgBox( NULL, "IDS_WINDOWS_REBOOT", GUI_OK );
#elif defined( __UNIX__ )
#endif
            } else {
                MsgBox( NULL, "IDS_COMPLETE", GUI_OK );
            }
        }
    } else if( !CancelSetup ) {
        if( VarGetBoolVal( UnInstall ) ) {
            MsgBox( NULL, "IDS_UNSETUPNOGOOD", GUI_OK );
        } else {
            MsgBox( NULL, "IDS_SETUPNOGOOD", GUI_OK );
        }
    }
}


#ifdef DO_DEBUGGING
void CheckHeap( void )
/********************/
{
#ifdef __WATCOMC__
    switch( _heapchk() ) {
    case _HEAPOK:
        SetupError( "IDS_HEAPOK" );
        break;
    case _HEAPEND:
        SetupError( "IDS_HEAPEND" );
        break;
    case _HEAPEMPTY:
        SetupError( "IDS_HEAPEMPTY" );
        break;
    case _HEAPBADBEGIN:
        SetupError( "IDS_HEAPDAMAGE" );
        break;
    case _HEAPBADPTR:
        SetupError( "IDS_HEAPBADPOINTER" );
        break;
    case _HEAPBADNODE:
        SetupError( "IDS_HEAPBADNODE" );
        break;
    }
#endif
}
#endif

char *stristr( const char *str, const char *substr, size_t substr_len )
/*********************************************************************/
{
    size_t  str_len;

    str_len = strlen( str );
    while( str_len-- >= substr_len ) {
        if( strnicmp( str, substr, substr_len ) == 0 )
            return( (char *)str );
        ++str;
    }
    return( NULL );
}


void SetupError( const char *msg )
/********************************/
{
//    MsgBox( NULL, "IDS_ERROR", GUI_OK, msg );
    MsgBox( NULL, msg, GUI_OK );
}
