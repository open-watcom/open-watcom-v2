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
* Description:  Setup utility functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <setjmp.h>
#include <limits.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#ifdef __UNIX__
  #include <utime.h>
  #include <dirent.h>
#else
  #include <sys/utime.h>
  #include <dos.h>
  #include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "gui.h"
#include "setupinf.h"
#include "setup.h"
#include "genvbl.h"
#include "gendlg.h"
#include "guiutil.h"
#include "utils.h"
#include "setupio.h"
#include "iopath.h"
#ifdef PATCH
#include "bdiff.h"
#include "wpack.h"
#endif
#include "errno.h"

#include "clibext.h"


#define TMPFILENAME "_watcom_.tmp"

#if defined( __NT__ ) || defined( __WINDOWS__ )
    #define UNC_SUPPORT
#endif

#define TEST_UNC(x) (x[0] == '\\' && x[1] == '\\')

#ifdef __UNIX__
    #define PMODE_W_USR (S_IWUSR)
#else
    #define PMODE_W_USR (S_IWRITE)
#endif
#define DEF_ACCESS      (PMODE_R | PMODE_W_USR)
#define DEF_EXEC        (DEF_ACCESS | PMODE_X)

typedef struct def_var {
    char                *variable;
    char                *value;
    struct def_var      *link;
} DEF_VAR;

extern void     BumpStatus( long );

bool            ConfigModified = false;
static enum { SRC_UNKNOWN, SRC_CD, SRC_DISK } SrcInstState;

extern int      IsPatch;
extern bool     CancelSetup;
extern vhandle  UnInstall;
extern vhandle  PreviousInstall;
bool            SkipDialogs;
char            *VariablesFile;
DEF_VAR         *ExtraVariables;
bool            Invisible;
bool            ProgramGroups;
bool            StartupChange;

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
            if( len > 1 && IS_PATH_SEP( dir[len - 2] ) && dir[len - 1] == '.' ) {
                dir[len - 1] = '\0';
            } else if( len == 2 && dir[1] == ':' ) {
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
            if( len == 1 || len == 3 && dir[1] == ':' ) {
                dir[len++] = '.';
                dir[len] = '\0';
            } else {
                dir[len - 1] = '\0';
            }
        } else {
            if( len == 2 && dir[1] == ':' ) {
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
#if defined( _UI ) || defined( __UNIX__ )
        uninstall = uninstall;
#else
        ret = CreatePMInfo( uninstall );
        if( !ret ) {                   // create folder and icons
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

    if( StartupChange ) {
#if !defined( _UI )
        WriteProfileStrings( uninstall );  // will write to the win.ini file.
#endif

//        if( !uninstall ) {
#if defined( __NT__ )
#ifdef __AXP__
        ret = ModifyConfiguration( uninstall );
#else
        if( GetVariableIntVal( "IsWin95" ) != 0 ) {
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
#ifdef __NT__
    return( ModifyRegAssoc( uninstall ) );
#else
    return( true );
#endif
}

bool ModifyUninstall( bool uninstall )
/************************************/
{
#ifdef __NT__
    return( AddToUninstallList( uninstall ) );
#else
    return( true );
#endif
}

typedef struct {
    disk_size           free_space;
    unsigned long       cluster_size;
    bool                use_target_for_tmp_file;
    bool                fixed;
    bool                diskette;
} drive_info;

static drive_info       Drives[32];

#if defined( __DOS__ )

int __far critical_error_handler( unsigned deverr, unsigned errcode, unsigned __far *devhdr )
/*****************************************************************************************/
{
    deverr = deverr;
    errcode = errcode;
    devhdr = devhdr;
    return( _HARDERR_FAIL );
}

typedef __far (HANDLER)( unsigned deverr, unsigned errcode, unsigned __far *devhdr );

#endif

#if !defined( __UNIX__ )
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
#endif

#ifdef __NT__
bool NTSpawnWait( char *cmd, DWORD *exit_code, HANDLE in, HANDLE out, HANDLE err )
/********************************************************************************/
{
    STARTUPINFO             start;
    PROCESS_INFORMATION     info;

    memset( &start, 0, sizeof( start ) );
    start.cb = sizeof( start );
    if( cmd[0] == '-' ) {
        cmd++;
        // set ShowWindow default value for nCmdShow parameter
        start.dwFlags |= STARTF_USESHOWWINDOW;
        start.wShowWindow = SW_HIDE;
    }
    if( in || out || err ) {
        start.dwFlags |= STARTF_USESTDHANDLES;
        start.hStdInput = in;
        start.hStdOutput = GetStdHandle( STD_OUTPUT_HANDLE );
        start.hStdError = GetStdHandle( STD_ERROR_HANDLE );
    }
    if( !CreateProcess( NULL, cmd, NULL, NULL, TRUE,
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
    BOOL        ok;
    char        buff[_MAX_PATH];

    show[0] = 2;
    show[1] = SW_SHOW;
    memset( &parm, 0, sizeof( parm ) );
    parm.lpzCmdLine = "";
    parm.lpShow = show;
    inst = LoadModule( cmd, &parm );
    if( inst < HINSTANCE_ERROR )
        return( false );
    ReplaceVars( buff, sizeof( buff ), "%ToolHelp%" );
    if( buff[0] == '\0' )
        return( false );
    toolhelp = LoadModule( buff, &parm );
    if( toolhelp < HINSTANCE_ERROR )
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
        ok = taskfirst( &task );
        while( ok ) {
            if( task.hInst == inst )
                stillthere = true;
            ok = tasknext( &task );
            StatusCancelled(); // yield
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
        *rc = DosWaitChild( DCWA_PROCESS, DCWW_NOWAIT, &res,
                            &dummy, res.codeTerminate );
        if( *rc != ERROR_CHILD_NOT_COMPLETE ) {
            *rc = res.codeResult;
            break;
        }
        StatusCancelled();
    }
    return( true );
}
#endif

static bool DoSpawnCmd( char *cmd )
/*********************************/
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
    char    buff[2 * _MAX_PATH];
    int     i, num_spawns;

    num_spawns = SimNumSpawns();
    for( i = 0; i < num_spawns; ++i ) {
        if( when != SimWhen( i ) )
            continue;
        if( !SimEvalSpawnCondition( i ) )
            continue;
        SimGetSpawnCommand( buff, sizeof( buff ), i );
        if( buff[0] == '\0' )
            continue;
        DoSpawnCmd( buff );
    }
}


#if defined( UNC_SUPPORT )
static void GetTmpFileNameUNC( const char *path, char *buff )
/***********************************************************/
{
    if( path == NULL || buff == NULL ) {
        return;
    }
    if( TEST_UNC( path ) ) {
        strcpy( buff, path );
        ConcatDirSep( buff );
        strcat( buff, TMPFILENAME );
    } else {
        buff[0] = *path;
        strcpy( buff + 1, ":\\" TMPFILENAME );
    }
}
#endif

#if !defined( __UNIX__ )

static void GetTmpFileName( char drive, char *buff )
/**************************************************/
{
    *buff++ = drive;
    *buff++ = DRIVE_SEP;
    *buff++ = DIR_SEP;
    strcpy( buff, TMPFILENAME );
}

static void GetTmpFileNameInTarget( char drive, char *buff, size_t buff_len )
/***************************************************************************/
{
    int         i;
    int         max_targets = SimNumTargets();

    for( i = 0; i < max_targets; ++i ) {
        SimTargetDir( i, buff, buff_len );
        if( tolower( buff[0] ) == tolower( drive ) && buff[1] == ':' ) {
            ConcatDirSep( buff );
            strcat( buff, TMPFILENAME );
            return;
        }
    }
    GetTmpFileName( drive, buff );
}
#endif

void ResetDriveInfo( void )
/*************************/
{
    int         i;

    for( i = 0; i < sizeof( Drives ) / sizeof( Drives[0] ); ++i ) {
        Drives[i].cluster_size = 0;
    }
}

static int GetDriveNum( char drive )
/**********************************/
{
    int     drive_num;

    drive_num = toupper( drive ) - 'A' + 1;
    if( drive_num < 1 || drive_num > 26 )
        drive_num = 0;
    return( drive_num );
}

static int GetDriveInfo( char drive, bool removable )
/***************************************************/
{
#if defined( __UNIX__ )
    int         drive_num = 0;
#else
    drive_info  *info;
    int         drive_num;

    drive_num = GetDriveNum( drive );
    info = &Drives[drive_num];
    if( (info->cluster_size == 0 || removable /* recheck - could have been replaced */) ) {
        memset( info, 0, sizeof( *info ) );
        if( drive_num == 0 )
            return( 0 );
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

            #define         BUF_SIZE 40

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
                /* This is a bit strange: the respective values are not */
                /* returned in a structure but in an array BLAH! from   */
                /* which one must extract info as below. See OS/2 Manual*/
                /* for clarification.                                   */
                sectors_per_cluster = *(ULONG *)(fsinfobuf + (1 * sizeof( ULONG )));
                free_clusters = *(ULONG *)(fsinfobuf + (3 * sizeof( ULONG )));
                bytes_per_sector = *(USHORT *)(fsinfobuf + (4 * sizeof( ULONG )));
                info->cluster_size = sectors_per_cluster * bytes_per_sector;
                info->free_space = (disk_size)free_clusters * (ULONG)info->cluster_size;
            } else {
                info->free_space = (disk_size)-1;
            }
            info->fixed = false;
            info->diskette = false;
            p.cmdinfo = 0;
            p.drive_num = drive_num - 1;
            parmLengthInOut = sizeof( p );
            dataLengthInOut = sizeof( r );
            if( DosDevIOCtl( -1, 8, 0x63, &p, sizeof( p ), &parmLengthInOut, &r,
                             sizeof( r ), &dataLengthInOut ) == 0 ) {
                dev[0] = drive;
                dev[1] = ':';
                dev[2] = '\0';
                dataBufferLen = sizeof( dataBuffer );
                rc = DosQueryFSAttach( dev, 0, FSAIL_QUERYNAME, (PFSQBUFFER2)&dataBuffer,
                                       &dataBufferLen );
                if( rc == 0 ) {
                    if( dataBuffer.b.iType == FSAT_LOCALDRV ) {
                        switch( r.type ) {
                        case 5: // fixed disk
                            info->fixed = true;
                            break;
                        case 6: // tape
                            break;
                        default: // diskette
                            info->diskette = true;
                            break;
                        }
                    }
                } else if( rc == ERROR_NOT_READY ) {
                    info->diskette = true; // diskette not in drive
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

            root[0] = drive;
            strcpy( &root[1], ":\\" );
            if( GetDiskFreeSpace( root, &sectors_per_cluster, &bytes_per_sector,
                                  &free_clusters, &total_clusters ) ) {
                info->cluster_size = bytes_per_sector * sectors_per_cluster;
                info->free_space = (disk_size)free_clusters * (disk_size)info->cluster_size;
            } else {
                info->free_space = (disk_size)-1;
            }
            drive_type = GetDriveType( root );
            info->diskette = ( drive_type == DRIVE_REMOVABLE );
            info->fixed = ( drive_type == DRIVE_FIXED );
        }
#else
        {
            struct diskfree_t   FreeSpace;
            union REGS          r;

            info->diskette = false;
            info->fixed = false;
            info->cluster_size = 0;
            info->free_space = (disk_size)-1;
            r.w.ax = 0x440E;    // get logical drive
            r.w.bx = drive_num;
            intdos( &r, &r );
            if( r.w.cflag || (r.h.al && (r.h.al != drive_num)) ) {
                return( drive_num );
            }
            info->fixed = true;
            r.w.ax = 0x4409;    // query device local/remote
            r.w.bx = drive_num;
            intdos( &r, &r );
            if( !r.w.cflag && (r.w.dx & 0x1000) ) {
                info->fixed = false;
            }
            r.w.ax = 0x4408;    // query device removable
            r.w.bx = drive_num;
            intdos( &r, &r );
            if( !r.w.cflag ) {
                info->diskette = ( r.w.ax == 0 );
                if( info->diskette ) {
                    info->fixed = false;
                }
            } else {
                info->fixed = false;
            }
            if( _dos_getdiskfree( drive_num, &FreeSpace ) == 0 ) {
                info->cluster_size = (unsigned long)FreeSpace.sectors_per_cluster *
                                     FreeSpace.bytes_per_sector;
                info->free_space = FreeSpace.avail_clusters *
                                   (disk_size)info->cluster_size;
                /* If reported cluster size is ridiculously large, it's likely faked; assume the
                 * real cluster size is much smaller - 4096 should be a conservative estimate.
                 */
                if( info->cluster_size > 64 * 1024UL ) {
                    info->cluster_size = 4096;
                }
            } else if( removable ) { // diskette not present
                info->cluster_size = 0;
                info->free_space = (disk_size)-1;
            } else {
                /* doesn't work on network drive - assume 4K cluster, max free */
                info->cluster_size = 4096;
                info->free_space = (disk_size)-1;
            }
        }
#endif
        if( !removable ) {
            int         io;
            char        path[_MAX_PATH];

            GetTmpFileNameInTarget( drive, path, sizeof( path ) );
            io = open( path, O_RDWR | O_CREAT | O_TRUNC, PMODE_RW );
            if( io == -1 ) {
                GetTmpFileName( drive, path );
                io = open( path, O_RDWR | O_CREAT | O_TRUNC, PMODE_RW );
                info->use_target_for_tmp_file = false;
            } else {
                info->use_target_for_tmp_file = true;
            }
            if( io != -1 ) {
                close( io );
                remove( path );
#if 0  // FIXME it doesn't work correctly if target directory doesn't exist
       // (new installation) and you have insufficient rights to drive root
            } else {
                info->cluster_size = 1;
                info->free_space = (disk_size)-1;
#endif
            }
        }
    }
#endif
    return( drive_num );
}

disk_size GetFreeDiskSpace( char drive, bool removable )
/****************************************************/
{
    return( Drives[GetDriveInfo( drive, removable )].free_space );
}

void ResetDiskInfo( void )
/************************/
{
    memset( Drives, 0, sizeof( Drives ) );
}

bool IsFixedDisk( char drive )
/****************************/
{
    if( drive == '\0' )
        return( false );
    // don't bang diskette every time!
    if( Drives[GetDriveNum( drive )].diskette )
        return( false );
    return( Drives[GetDriveInfo( drive, false )].fixed != 0 );
}

unsigned GetClusterSize( char drive )
/***********************************/
{
#if defined( __UNIX__ )
    return( 1 );
#else
    if( drive == '\0' )
        return( 1 );
    return( Drives[GetDriveInfo( drive, false )].cluster_size );
#endif
}

#if defined( UNC_SUPPORT )
// The 3 functions: GetRootFromPath(), FreeSpace() and ClusterSize() were originally in
// bkoffice.c, but now they are being used by utils.c and setupinf.c to support UNC
// naming for regular installs.
bool GetRootFromPath( char *root, const char *path )
/**************************************************/
{
    char        *index;
    char        curr_dir[_MAX_PATH];
    int         i;

    if( path == NULL || root == NULL ) {
        return( false );
    }

    if( isalpha( path[0] ) && path[1] == ':' ) {
        // turn a path like "c:\dir" into "c:\"
        root[0] = path[0];
        root[1] = path[1];
        root[2] = DIR_SEP;
        root[3] = '\0';
        return( true );
    } else if( TEST_UNC( path ) ) {
        // turn a UNC name like "\\root\share\dir\subdir" into "\\root\share\"
        strcpy( root, path );
        ConcatDirSep( root );
        index = root;
        i = 0;
        while( *index != '\0' ) {
            if( IS_DIR_SEP( *index ) ) {
                i++;
            }
            index++;
            if( i == 4 ) { // cut off string at character after 4th backslash
                *index = '\0';
                return( true );
            }
        }
        return( false );  // invalid UNC name such as: "\\missingshare\"
    } else {
        // for relative paths like "\dir" use the current drive.
        if( getcwd( curr_dir, sizeof( curr_dir ) ) == NULL ) {
            return( false );
        }
        _splitpath( curr_dir, root, NULL, NULL, NULL );
        ConcatDirSep( root );
        return( true );
    }
}

disk_size FreeSpace( const char *path )
/***********************************/
{
#ifdef __NT__
    DWORD       sectors_per_cluster;
    DWORD       bytes_per_sector;
    DWORD       avail_clusters;
    DWORD       total_clusters;
    char        root[_MAX_PATH];

    if( GetRootFromPath( root, path ) ) {
        if( GetDiskFreeSpace( root, &sectors_per_cluster, &bytes_per_sector,
                              &avail_clusters, &total_clusters ) ) {
            return( (disk_size)sectors_per_cluster * (disk_size)bytes_per_sector * (disk_size)avail_clusters );
        }
    }
#else
    struct diskfree_t info;
    if( isalpha( *path ) ) {
        if( _getdiskfree( toupper( *path ) - 'A' + 1, &info ) == 0 ) {
            return( (disk_size)info.sectors_per_cluster * (disk_size)info.bytes_per_sector * (disk_size)info.avail_clusters );
        }
    }
#endif
    return( 0 );
}

long ClusterSize( const char *path )
/**********************************/
{
#ifdef __NT__
    DWORD       sectors_per_cluster;
    DWORD       bytes_per_sector;
    DWORD       avail_clusters;
    DWORD       total_of_clusters;
    char        root[_MAX_PATH];

    if( GetRootFromPath( root, path ) ) {
        if( GetDiskFreeSpace( root, &sectors_per_cluster, &bytes_per_sector,
                              &avail_clusters, &total_of_clusters ) ) {
            return( sectors_per_cluster * bytes_per_sector );
        }
    }

#else
    struct diskfree_t info;
    if( isalpha( *path ) ) {
        if( _getdiskfree( toupper( *path ) - 'A' + 1, &info ) == 0 ) {
            return( (long)info.sectors_per_cluster * info.bytes_per_sector );
        }
    }
#endif
    return( 0 );
}

bool IsDriveWritable( const char *path )
/**************************************/
{
    int         io;
    char        tempfile[_MAX_PATH];
    char        root[_MAX_PATH];

    if( path == NULL ) {
        return( false );
    }

    if( !GetRootFromPath( root, path ) ) {
        return( false );
    }

    GetTmpFileNameUNC( root, tempfile );

    io = open( tempfile, O_RDWR | O_CREAT | O_TRUNC, PMODE_RW );
    if( io == -1 ) {
        return( false );
    } else {
        close( io );
        remove( tempfile );
        return( true );
    }
}
#endif

#ifdef UNC_SUPPORT
bool DriveInfoIsAvailable( const char *path )
/*******************************************/
{
#ifdef __NT__
    DWORD       sectors_per_cluster;
    DWORD       bytes_per_sector;
    DWORD       avail_clusters;
    DWORD       total_clusters;
    char        root[_MAX_PATH];

    if( GetRootFromPath( root, path ) ) {
        if( GetDiskFreeSpace( root, &sectors_per_cluster, &bytes_per_sector,
                              &avail_clusters, &total_clusters ) ) {
            return( true );
        }
    }
#else
    struct diskfree_t info;

    if( isalpha( *path ) ) {
        if( _getdiskfree( toupper( *path ) - 'A' + 1, &info ) == 0 ) {
            return( true );
        }
    }
#endif
    return( false );
}
#endif

// ********** Functions for Creating the destination directory tree **********


static void RemoveDstDir( int dir_index, char *buff, size_t buff_len )
/********************************************************************/
{
    int         child;
    int         max_dirs = SimNumDirs();

    SimDirNoSlash( dir_index, buff, buff_len );
    if( access( buff, F_OK ) != 0 )
        return;
    for( child = 0; child < max_dirs; ++child ) {
        if( SimDirParent( child ) == dir_index ) {
            RemoveDstDir( child, buff, buff_len );
        }
    }
    if( SimDirParent( dir_index ) == -1 ) {
//        return; // leave root dir (for config.new, etc)
    }
    SimDirNoSlash( dir_index, buff, buff_len );
    rmdir( buff );
}


void MakeParentDir( const char *dir, char *drive, char *path )
/************************************************************/
{
    char                *parent, *end;
    size_t              path_len;

    _splitpath( dir, drive, path, NULL, NULL );
    if( *path == '\0' )
        return;
    path_len = strlen( path );
    end = path + path_len - 1;
    if( IS_DIR_SEP( *end ) )
        *end = '\0';
    if( *path == '\0' )
        return;
    parent = alloca( strlen( drive ) + path_len + 10 ); // lotsa room
    strcpy( parent, drive );
    strcat( parent, path );
    MakeParentDir( parent, drive, path );
#if defined( __UNIX__ )
    mkdir( parent, 0777 );
#else
    mkdir( parent );
#endif
}


bool CreateDstDir( int i, char *buff, size_t buff_len )
/*****************************************************/
// check for directory exitstance.  If dir exists return true.
// Else try and create directory.
{
    bool                ok;
    int                 parent;
    char                drive[_MAX_DRIVE];
    char                path[_MAX_PATH];

    parent = SimDirParent( i );
    if( parent != -1 ) {
        ok = CreateDstDir( parent, buff, buff_len );
        if( !ok ) {
            return( false );
        }
    }
    SimDirNoSlash( i, buff, buff_len );
    if( access( buff, F_OK ) == 0 )          // check for existance
        return( true );
    MakeParentDir( buff, drive, path );
#if defined( __UNIX__ )
    if( mkdir( buff, 0777 ) == 0 )
#else
    if( mkdir( buff ) == 0 )
#endif
        return( true );
    MsgBox( NULL, "IDS_CANTMAKEDIR", GUI_OK, buff );
    return( false );
}

#define KB  1024UL
#define MB  (KB * KB)

static void catnum( char *buff, long long num )
/*********************************************/
{

    char        num_buff[128];
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

    char        num_buff[128];
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

static bool MatchEnd( const char *path, const char *end )
/*******************************************************/
{
    size_t  plen;
    size_t  endlen;

    plen = strlen( path );
    endlen = strlen( end );
    if( endlen > plen )
        return( false );
#if defined( __UNIX__ )
    return( strcmp( path + plen - endlen, end ) == 0 );
#else
    return( stricmp( path + plen - endlen, end ) == 0 );
#endif
}

static bool FindUpgradeFile( char *path )
/***************************************/
{
    DIR                 *d;
    struct dirent       *info;
    char                *path_end;
    int                 upgrades = SimNumUpgrades();
    int                 i;
#if defined( __UNIX__ )
    struct stat         statbuf;
#endif

    StatusAmount( 0, 1 );
    StatusLines( STAT_CHECKING, path );
    if( StatusCancelled() )
        return( false );
    d = opendir( path );
    ConcatDirSep( path );
    path_end = path + strlen( path );
    while( (info = readdir( d )) != NULL ) {
        strcpy( path_end, info->d_name );
#if defined( __UNIX__ )
        stat( path, &statbuf );
        if( S_ISDIR( statbuf.st_mode ) ) {
#else
        if( info->d_attr & _A_SUBDIR ) {
#endif
            if( info->d_name[0] != '.' ) {
                if( FindUpgradeFile( path ) ) {
                    return( true );
                }
            }
        } else {
            for( i = 0; i < upgrades; ++i ) {
                if( MatchEnd( path, SimGetUpgradeName( i ) ) ) {
                    return( true );
                }
            }
        }
    }
    *path_end = '\0';
    closedir( d );
    return( false );
}

bool CheckUpgrade( void )
/***********************/
{
    char                disk[_MAX_PATH];
    dlg_state           return_state;

    if( GetVariableIntVal( "PreviousInstall" ) )
        return( true );
    DoDialog( "UpgradeStart" );
#if defined( __UNIX__ )
    disk[0] = '/';
    disk[1] = '\0';
    StatusCancelled();
    if( FindUpgradeFile( disk ) ) {
        return( true );
    }
#else
    for( disk[0] = 'c'; disk[0] <= 'z'; disk[0]++ ) {
        if( !IsFixedDisk( disk[0] ) )
            continue;
        strcpy( disk + 1, ":\\" );
        StatusCancelled();
        if( FindUpgradeFile( disk ) ) {
            return( true );
        }
    }
#endif
    return_state = DoDialog( "UpgradeNotQualified" );
    return( return_state != DLG_CAN && return_state != DLG_DONE );
}

static void free_disks( char **disks, int max_targs )
{
    int         i;

    for( i = 0; i < max_targs; i++ ) {
        GUIMemFree( disks[i] );
    }
}

bool CheckDrive( bool issue_message )
/***********************************/
//check if there is enough disk space
{
    bool                ret;
    disk_size           free_disk_space;
    disk_size           disk_space_needed;
    disk_size           max_tmp_file;
    int                 max_targs;
    int                 i, j, targ_num;
    char                *disks[MAX_DRIVES];
    bool                disk_counted[MAX_DRIVES];
#if !defined( __UNIX__ )
    char                drive;
    gui_message_return  reply;
#endif
    char                buff[_MAX_PATH];
    char                drive_freesp[20];
    struct {
        char                *drive;
        disk_size   needed;
        disk_size   max_tmp;
        disk_size   free;
        int         num_files;
    }                   space[MAX_DRIVES];
#ifdef UNC_SUPPORT
    char                root[2][_MAX_PATH];
    char                UNC_root[_MAX_PATH];
#endif

    ret = true;
    if( !SimCalcTargetSpaceNeeded() )
        return( false );
    max_targs = SimNumTargets();
    for( i = 0; i < max_targs; i++ ) {
        // get drive letter for each target (actually the path including the drive letter)
        disks[i] = SimGetTargetDriveLetter( i );
        ConcatDirSep( disks[i] );
        if( disks[i] == NULL ) {
            return( false );
        } else {
            disk_counted[i] = false;
        }
    }
    // check for enough disk space, combine drives that are the same
    for( i = 0; i < max_targs; i++ ) {
        if( !disk_counted[i] ) {
            targ_num = i;
            disk_space_needed = SimTargetSpaceNeeded( i );
            max_tmp_file = SimMaxTmpFile( i );
            for( j = i + 1; j < max_targs; ++j ) {
#ifdef UNC_SUPPORT
                GetRootFromPath( root[0], disks[i] );
                GetRootFromPath( root[1], disks[j] );
                // identical drives are combined, and so are UNC paths pointing to the same share
                // BUT:  drives and UNC paths that happen to be the same are NOT combined. (I am lazy)

                if( ( tolower( *disks[j] ) == tolower( *disks[i] ) &&
                    isalpha( *disks[i] ) ) || stricmp( root[0], root[1] ) == 0 ) {
#else
                if( tolower( *disks[j] ) == tolower( *disks[i] ) &&
                    isalpha( *disks[i] ) ) {
#endif
                    targ_num = j;
                    disk_space_needed += SimTargetSpaceNeeded( j );
                    if( SimMaxTmpFile( j ) > max_tmp_file ) {
                        max_tmp_file = SimMaxTmpFile( j );
                    }
                    disk_counted[j] = true;
                }
            }
#ifdef UNC_SUPPORT
            if( TEST_UNC( disks[i] ) ) {
                if( !IsDriveWritable( disks[i] ) ) {
                    if( issue_message ) {
                        GetRootFromPath( UNC_root, disks[i] );
                        if( access( UNC_root, F_OK ) == 0 ) {
                            MsgBox( NULL, "IDS_UNCPATH_NOTWRITABLE", GUI_OK, UNC_root );
                        } else {
                            MsgBox( NULL, "IDS_UNCPATH_NOTEXIST", GUI_OK, UNC_root );
                        }
                        free_disks( disks, max_targs );
                        return( false );
                    }
                }
                free_disk_space = FreeSpace( disks[i] );
            } else
#endif
            {
                free_disk_space = GetFreeDiskSpace( *disks[i], false );
            }
            if( free_disk_space == (unsigned long long)-1 )
                free_disk_space = 0;
            space[i].drive = disks[i];
            space[i].free = free_disk_space;
            space[i].needed = disk_space_needed;
            space[i].max_tmp = max_tmp_file;
            space[i].num_files = SimGetTargNumFiles( targ_num );
#if !defined( __UNIX__ )
            if( (disk_ssize)disk_space_needed > 0 && free_disk_space < disk_space_needed + max_tmp_file ) {
                for( drive = 'c'; drive <= 'z'; ++drive ) {
                    if( drive == tolower( *disks[i] ) )
                        continue;
                    if( !IsFixedDisk( drive ) )
                        continue;
                    if( GetFreeDiskSpace( drive, false ) > max_tmp_file ) {
                        SimSetTargTempDisk( i, drive );
                        for( j = i + 1; j < max_targs; ++j ) {
                            if( tolower( *disks[j] ) == tolower( *disks[i] ) ) {
                                SimSetTargTempDisk( j, drive );
                            }
                        }
                        break;
                    }
                    if( drive == 'z' && issue_message ) {
                        MsgBox( NULL, "IDS_NOTEMPSPACE", GUI_OK, max_tmp_file / 1000 );
                        free_disks( disks, max_targs );
                        return( false );
                    }
                }
            }
            if( issue_message ) {
                if( (disk_ssize)disk_space_needed > 0 && free_disk_space < disk_space_needed ) {
    #ifdef UNC_SUPPORT
                    if( TEST_UNC( disks[i] ) ) {
                        if( DriveInfoIsAvailable( disks[i] ) ) {
                            reply = MsgBox( NULL, "IDS_NODISKSPACE_UNC", GUI_YES_NO,
                                            disks[i], free_disk_space / 1000,
                                            disk_space_needed / 1000 );
                        } else {
                            GetRootFromPath( root, disks[i] );
                            reply = MsgBox( NULL, "IDS_ASSUME_ENOUGHSPACE", GUI_YES_NO,
                                            root );
                        }
                    } else {
    #endif
                        reply = MsgBox( NULL, "IDS_NODISKSPACE", GUI_YES_NO, *disks[i],
                                        free_disk_space / 1000,
                                        disk_space_needed / 1000 );
    #ifdef UNC_SUPPORT
                    }
    #endif
                    if( reply == GUI_RET_NO ) {
                        free_disks( disks, max_targs );
                        return( false );
                    }
                }
            }
#endif
        }
    }
    for( i = 0; i < max_targs; ++i ) {
        strcpy( drive_freesp, "DriveFreeN" );
        if( *space[i].drive != '\0' && SimTargetNeedsUpdate( i ) ) {
#ifdef UNC_SUPPORT
            if( TEST_UNC( space[i].drive ) ) {
                GetRootFromPath( UNC_root, space[i].drive );
                sprintf( buff, GetVariableStrVal( "IDS_DRIVE_SPEC_UNC" ), UNC_root );
            } else
#endif
            {
                sprintf( buff, GetVariableStrVal( "IDS_DRIVE_SPEC" ),
                         toupper( *space[i].drive ) );
            }
            if( (disk_ssize)space[i].needed < 0 ) {
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
        if( TEST_UNC( space[i].drive ) && (!DriveInfoIsAvailable( space[i].drive ) ||
                                           !IsDriveWritable( space[i].drive )) ) {
            strcpy( buff, "" );
        }
#endif
        SetVariableByName( drive_freesp, buff );
    }
    free_disks( disks, max_targs );
    return( ret );
}

static void SetFileDate( const char *dst_path, time_t date )
/**********************************************************/
{
    struct utimbuf      timebuf;

    timebuf.modtime = date;
    timebuf.actime = date;
    utime( dst_path, &timebuf );
}

static void SameFileDate( const char *src_path, const char *dst_path )
/********************************************************************/
{
    struct stat         statblk;

    FileStat( src_path, &statblk );
    SetFileDate( dst_path, statblk.st_mtime );
}

// ********************Functions for Deleting Files***********************

bool DoDeleteFile( const char *path )
/***********************************/
{
    return( remove( path ) == 0 );
}

// ******************* Functions for Copying Files ***************************

COPYFILE_ERROR DoCopyFile( const char *src_path, const char *dst_path, bool append )
/*********************************************************************************/
{
    static char         lastchance[1024];
    size_t              buffer_size = 16 * 1024;
    void                *src_files;
    int                 dst_files;
    int                 bytes_read, bytes_written, style;
    char                *pbuff;

    src_files = FileOpen( src_path, O_RDONLY | O_BINARY );
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

    if( append ) {
        style = O_RDWR | O_BINARY;
    } else {
        style = O_CREAT | O_TRUNC | O_WRONLY | O_BINARY;
    }
    dst_files = open( dst_path, style, DEF_ACCESS );
    if( dst_files == -1 ) {
        FileClose( src_files );
        if( pbuff != lastchance )
            GUIMemFree( pbuff );
        dst_files = open( dst_path, O_RDONLY );
        if( dst_files != -1 ) {
            // read only file
            close( dst_files );
            return( CFE_DSTREADONLY );
        }
        return( CFE_CANTOPENDST );
    }
    if( append ) {
        lseek( dst_files, 0, SEEK_END );
    }

    do {
        bytes_read = FileRead( src_files, pbuff, buffer_size );
        bytes_written = write( dst_files, pbuff, bytes_read );
        BumpStatus( bytes_written );
        if( bytes_written != bytes_read || StatusCancelled() ) {
            close( dst_files );
            FileClose( src_files );
            if( bytes_written == bytes_read ) {
                // copy was aborted, delete destination file
                DoDeleteFile( dst_path );
                return( CFE_ABORT );
            }
            // error writing file - probably disk full
            if( pbuff != lastchance )
                GUIMemFree( pbuff );
            SetupError( "IDS_WRITEERROR" );
            return( CFE_ERROR );
        }
    } while( bytes_read == buffer_size );

    // Make the destination file have the same time stamp as the source file.
    close( dst_files );

    SameFileDate( src_path, dst_path );
    if( pbuff != lastchance )
        GUIMemFree( pbuff );
    FileClose( src_files );
    return( CFE_NOERROR );
}


#define DIR_THRESHOLD   10

typedef struct split_file {
    struct split_file   *next;
    char                *src_path;
    char                *disk_desc;     // a file may span multiple disks
} split_file;

#define OVERHEAD_SIZE 10000 // removing a file is about like copying a small file

static bool CreateDirectoryTree( void )
/*************************************/
{
    long                num_total_install;
    long                num_installed;
    int                 i;
    char                dst_path[_MAX_PATH];
    int                 max_dirs = SimNumDirs();

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
    num_installed = 0;
    for( i = 0; i < max_dirs; i++ ) {
        if( SimDirUsed( i ) ) {
            if( !CreateDstDir( i, dst_path, sizeof( dst_path ) ) )
                return( false );
            StatusLines( STAT_SAME, dst_path );
            StatusAmount( ++num_installed, num_total_install );
            if( StatusCancelled() ) {
                return( false );
            }
        }
    }
    if( num_total_install != 0 ) {
        StatusLines( STAT_SAME, "" );
        StatusAmount( num_total_install, num_total_install );
    }
    return( true );
}

static bool RelocateFiles( void )
/*******************************/
{
    int                 filenum;
    int                 subfilenum, max_subfiles;
    long                num_total_install;
    long                num_installed;
    char                dst_path[_MAX_PATH];
    char                src_path[_MAX_PATH];
    char                dir[_MAX_PATH];
    char                file_desc[MAXBUF];
    int                 max_files = SimNumFiles();

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
    num_installed = 0;
    for( filenum = 0; filenum < max_files; filenum++ ) {
        if( SimFileRemove( filenum ) )
            continue;
        max_subfiles = SimNumSubFiles( filenum );
        for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
            if( SimSubFileInOldDir( filenum, subfilenum ) ) {
                SimFileOldDir( filenum, dir, sizeof( dir ) );
                SimSubFileName( filenum, subfilenum, file_desc );
                _makepath( src_path, NULL, dir, file_desc, NULL );
                SimFileDir( filenum, dir, sizeof( dir ) );
                _makepath( dst_path, NULL, dir, file_desc, NULL );
                StatusLines( STAT_SAME, src_path );
                if( SimSubFileInNewDir( filenum, subfilenum ) ) {
                    remove( dst_path );
                }
                if( DoCopyFile( src_path, dst_path, false ) != CFE_NOERROR ) {
                    return( false );
                }
                if( SimSubFileExecutable( filenum, subfilenum ) ) {
                    chmod( dst_path, DEF_EXEC );
                }
                remove( src_path );
                num_installed += SimSubFileSize( filenum, subfilenum );
                StatusAmount( num_installed, num_total_install );
            }
        }
    }
    if( num_total_install != 0 ) {
        StatusLines( STAT_RELOCATING, "" );
        StatusAmount( num_total_install, num_total_install );
    }
    return( true );
}


typedef struct file_check {
    struct file_check   *next;
    char                *name;
    vhandle             var_handle;
    bool                is_dll;
} file_check;

static file_check *fileCheck = NULL;
static file_check *fileCheckThisPack = NULL;

static void NewFileToCheck( const char *name, bool is_dll )
/*********************************************************/
{
    file_check  *new;

    new = GUIMemAlloc( sizeof( *new ) );
    new->next = fileCheckThisPack;
    fileCheckThisPack = new;
    new->name = GUIStrDup( name, NULL );
    new->var_handle = NO_VAR;
    new->is_dll = is_dll;
}

static void UpdateCheckList( const char *name, vhandle var_handle )
/*****************************************************************/
{
    file_check  *check;

    for( check = fileCheckThisPack; check != NULL; check = check->next ) {
#if defined( __UNIX__ )
        if( strcmp( name, check->name ) == 0 ) {
#else
        if( stricmp( name, check->name ) == 0 ) {
#endif
            check->var_handle = var_handle;
        }
    }
}

static void TransferCheckList( void )
/***********************************/
{
    file_check  *check, *next;

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
    file_check  *curr, *next;
    gui_message_return  ret;

    for( curr = fileCheck; curr != NULL; curr = next ) {
        next = curr->next;
        if( curr->is_dll ) {
            ret = CheckInstallDLL( curr->name, curr->var_handle );
        } else {
            ret = CheckInstallNLM( curr->name, curr->var_handle );
        }
        if( ret == GUI_RET_CANCEL )
            return( false );
        GUIMemFree( curr->name );
        GUIMemFree( curr );
    }
    return( true );
}

static void CopySetupInfFile( void )
/**********************************/
{
    const char          *p;
    char                dst_path[_MAX_PATH];
    char                tmp_path[_MAX_PATH];

    // if DoCopyInf variable is set, copy/delete setup.inf
    p = GetVariableStrVal( "DoCopyInf" );
    if( (p != NULL) && (strlen( p ) > 0) ) {
        ReplaceVars( tmp_path, sizeof( tmp_path ), p );
        _makepath( dst_path, NULL, tmp_path, "setup.inf", NULL );
        if( VarGetIntVal( UnInstall ) ) {
            remove( dst_path );
        } else {
            p = GetVariableStrVal( "SetupInfFile" );
            DoCopyFile( p, dst_path, false );
        }
    }
}

int UnPackHook( int filenum, int subfilenum, char *name )
/*******************************************************/
{
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];

    if( SimSubFileIsNLM( filenum, subfilenum ) ) {
        NewFileToCheck( name, false );
        _splitpath( name, drive, dir, fname, ext );
        _makepath( name, drive, dir, fname, "._N_" );
        return( 1 );
    } else if( SimSubFileIsDLL( filenum, subfilenum ) ) {
        NewFileToCheck( name, true );
#ifdef EXTRA_CAUTIOUS_FOR_DLLS
        if( !IsPatch ) {
            _splitpath( name, drive, dir, fname, ext );
            _makepath( name, drive, dir, fname, "._D_" );
        }
#endif
        return( 1 );
    }
    return( 0 );
}

static bool DoCopyFiles( void )
/*****************************/
{
    int                 filenum;
//    int                 disk_num;
    int                 subfilenum, max_subfiles;
    COPYFILE_ERROR      copy_error;
    char                dst_path[_MAX_PATH];
    char                tmp_path[_MAX_PATH];
    char                src_path[_MAX_PATH];
    char                file_name[_MAX_FNAME + _MAX_EXT];
    char                file_desc[MAXBUF], dir[_MAX_PATH], disk_desc[MAXBUF];
    char                old_dir[_MAX_PATH];
    long                num_total_install;
    long                num_installed;
    split_file          *split = NULL;
    split_file          **owner_split = &split;
    split_file          *junk;
    int                 value;
    vhandle             var_handle;
    gui_message_return  ret = GUI_RET_OK;
    int                 max_files = SimNumFiles();
    size_t              len;
    char                *p;
    char                *p1;
    const char          *cp;


    num_total_install = 0;
    for( filenum = 0; filenum < max_files; filenum++ ) {
        SimFileDir( filenum, dir, sizeof( dir ) );
        if( SimFileAdd( filenum ) && !SimFileUpToDate( filenum ) ) {
            num_total_install += SimFileSize( filenum );
            if( SimFileSplit( filenum ) ) {
                num_total_install += SimFileSize( filenum );
            }
            max_subfiles = SimNumSubFiles( filenum );
            for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
                if( SimSubFileReadOnly( filenum, subfilenum ) ) {
                    SimSubFileName( filenum, subfilenum, file_desc );
                    _makepath( tmp_path, NULL, dir, file_desc, NULL );
                    if( !PromptUser( tmp_path, "ReadOnlyFile", "RO_Skip_Dialog", "RO_Replace_Old", &value ) ) {
                        return( false );
                    }
                    if( value ) {
                        chmod( tmp_path, PMODE_W_USR );
                    }
                }
                if( SimSubFileNewer( filenum, subfilenum ) ) {
                    SimSubFileName( filenum, subfilenum, file_desc );
//                  _splitpath( file_desc, NULL, NULL, NULL, file_ext );
                    _makepath( tmp_path, NULL, dir, file_desc, NULL );
                    if( !PromptUser( tmp_path, "NewerFile", "Newer_Skip_Dialog", "Newer_Replace_Old", &value ) ) {
                        return( false );
                    }
                    if( value ) {
                        SetFileDate( tmp_path, SimSubFileDate( filenum, subfilenum ) - 1 );
                    }
                }
            }
        } else if( SimFileRemove( filenum ) ) {
            max_subfiles = SimNumSubFiles( filenum );
            for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
                if( !SimSubFileExists( filenum, subfilenum ) )
                    continue;
                if( SimSubFileReadOnly( filenum, subfilenum ) ) {
                    SimSubFileName( filenum, subfilenum, file_desc );
                    _makepath( tmp_path, NULL, dir, file_desc, NULL );
                    if( !PromptUser( tmp_path, "DeleteReadOnlyFile", "RO_Skip_Remove", "RO_Remove_Old", &value ) ) {
                        return( false );
                    }
                    if( value ) {
                        chmod( tmp_path, PMODE_W_USR );
                        num_total_install += OVERHEAD_SIZE;
                    }
                } else {
                    num_total_install += OVERHEAD_SIZE;
                }
            }
        }
    }

    num_installed = 0;
    StatusLines( STAT_COPYINGFILE, "" );
    StatusAmount( 0, num_total_install );

    /* remove files first so we don't go over disk space estimate */

    for( filenum = 0; filenum < max_files; filenum++ ) {
        if( SimFileRemove( filenum ) ) {
            SimFileDir( filenum, dir, sizeof( dir ) );
            max_subfiles = SimNumSubFiles( filenum );
            for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
                if( !SimSubFileExists( filenum, subfilenum ) )
                    continue;
                num_installed += OVERHEAD_SIZE;
                SimSubFileName( filenum, subfilenum, file_desc );
                _makepath( tmp_path, NULL, dir, file_desc, NULL );
                StatusLines( STAT_REMOVING, tmp_path );
                remove( tmp_path );
                if( SimSubFileInOldDir( filenum, subfilenum ) ) {
                    SimFileOldDir( filenum, old_dir, sizeof( old_dir ) );
                    _makepath( tmp_path, NULL, old_dir, file_desc, NULL );
                    StatusLines( STAT_REMOVING, tmp_path );
                    remove( tmp_path );
                }
                StatusAmount( num_installed, num_total_install );
                if( StatusCancelled() ) {
                    return( false );
                }
            }
        }
    }

    /* now go ahead and add files */

    if( FileIsPlainFS() ) {
        strcpy( src_path, GetVariableStrVal( "SrcDir" ) );
        ConcatDirSep( src_path );
        p1 = src_path + strlen( src_path );
    } else {
        *src_path = '\0';
        p1 = src_path;
    }
    for( filenum = 0; filenum < max_files; filenum++ ) {
        if( !SimFileAdd( filenum ) || SimFileUpToDate( filenum ) ) {
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
        SimFileDir( filenum, dir, sizeof( dir ) );
        SimGetFileDesc( filenum, file_desc );
        SimGetFileName( filenum, file_name );
//        disk_num = SimFileDisk( filenum, disk_desc );
        SimFileDisk( filenum, disk_desc );

//      _splitpath( file_desc, NULL, NULL, NULL, file_ext );
        _makepath( dst_path, NULL, dir, file_desc, NULL );

        cp = GetVariableStrVal( "DstDir" );
        len = strlen( cp );
        if( strncmp( dir, cp, len ) == 0 ) {
            if( dir[len] == DIR_SEP )       // if 1st char to concat is a backslash, skip it
                len++;
            strcpy( p1, dir + len );  // get rid of the dest directory, just keep the subdir
        } else {
            // use the macro as the directory name   eg: cd_drive:\winsys\filename
            SimTargetDirName( SimDirTargNum( SimFileDirNum( filenum ) ), tmp_path, sizeof( tmp_path ) );
            len = strlen( GetVariableStrVal( tmp_path ) );
            strcpy( p1, tmp_path );
            strcat( p1, dir + len );
        }
        p = p1 + strlen( p1 );
        strcpy( p, file_desc );

        if( StatusCancelled() ) {
            return( false );
        }

        max_subfiles = SimNumSubFiles( filenum );
        for( subfilenum = 0; subfilenum < max_subfiles; ++subfilenum ) {
            do {
                SimSubFileName( filenum, subfilenum, file_desc );
                var_handle = SimSubFileVar( filenum, subfilenum );
                _makepath( tmp_path, NULL, dir, file_desc, NULL );

                *p = '\0';  // nuke name from end of src_path
                strcpy( p, file_desc );
                StatusLines( STAT_COPYINGFILE, tmp_path );
                UnPackHook( filenum, subfilenum, tmp_path );
                copy_error = DoCopyFile( src_path, tmp_path, false );

                switch( copy_error ) {
                case CFE_ABORT:
                case CFE_ERROR:
                    ret = GUI_RET_CANCEL;
                    break;
                case CFE_BAD_CRC:
                    MsgBox( NULL, "IDS_BADCRC", GUI_OK, src_path );
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
                    ret = MsgBox( NULL, "IDS_CANTOPENSRC", GUI_RETRY_CANCEL, src_path  );
                    break;
                case CFE_CANTOPENDST:
                    ret = MsgBox( NULL, "IDS_CANTOPENDST", GUI_RETRY_CANCEL, tmp_path );
                    break;
                }
                if( ret == GUI_RET_CANCEL ) {
                    return( false );
                }
            } while( copy_error != CFE_NOERROR );

            if( SimSubFileExecutable( filenum, subfilenum ) ) {
                chmod( tmp_path, DEF_EXEC );
            }
            SetVariableByHandle( var_handle, tmp_path );
            UpdateCheckList( tmp_path, var_handle );
        }
        TransferCheckList();

        num_installed += SimFileSize( filenum );
        if( num_installed > num_total_install )
            num_installed = num_total_install;
        StatusAmount( num_installed, num_total_install );
        if( StatusCancelled() ) {
            return( false );
        }
    }
    if( !CheckPendingFiles() )
        return( false );
    StatusAmount( num_total_install, num_total_install );
    return( true );
}


static void RemoveUnusedDirs( void )
/**********************************/
{
    char        dst_path[_MAX_PATH];
    int         i;
    int         max_dirs = SimNumDirs();

    for( i = 0; i < max_dirs; i++ ) {
        if( !SimDirUsed( i ) ) {
            RemoveDstDir( i, dst_path, sizeof( dst_path ) );
        }
    }
}

//  TODO: Figure out what supplemental files will be applicable for the UNIX environment

static void RemoveExtraFiles( void )
/**********************************/
// remove supplemental files
{
#if !defined( __UNIX__ )
    const char          *p;
    char                dst_path[_MAX_PATH];

    if( VarGetIntVal( UnInstall ) ) {
        // delete saved autoexec's and config's
        p = GetVariableStrVal( "DstDir" );
#if defined( __NT__ )
        // Windows NT
        strcpy( dst_path, p );
        strcat( dst_path, "\\CHANGES.ENV" );
        remove( dst_path );
        // Windows 95
        strcpy( dst_path, p );
        strcat( dst_path, "\\AUTOEXEC.W95" );
        remove( dst_path );
        strcpy( dst_path, p );
        strcat( dst_path, "\\CONFIG.W95" );
        remove( dst_path );
#elif defined( __OS2__ )
        strcpy( dst_path, p );
        strcat( dst_path, "\\CONFIG.OS2" );
        remove( dst_path );
#else
        strcpy( dst_path, p );
        strcat( dst_path, "\\AUTOEXEC.DOS" );
        remove( dst_path );
        strcpy( dst_path, p );
        strcat( dst_path, "\\CONFIG.DOS" );
        remove( dst_path );
#endif
    }
#endif
}

void DetermineSrcState( const char *src_dir )
/*******************************************/
{
    char        dir[_MAX_PATH];

//  if( SrcInstState != SRC_UNKNOWN ) return;

    // if installing from CD or hard disk, add DISK# to source path
    strcpy( dir, src_dir );
#if defined( __UNIX__ )
    strcat( dir, "/diskimgs/disk01" );
#else
    strcat( dir, "\\cd_source" );
#endif
    if( access( dir, F_OK ) == 0 ) {
        SetVariableByName( "SrcIsCD", "1" );
        SrcInstState = SRC_CD;
    } else {
        SetVariableByName( "SrcIsCD", "0" );
        SrcInstState = SRC_DISK;
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
#if defined( __OS2__ ) && !defined( _UI )
    LabelDirs();    // add labels (long names) to directories
#endif
    if( !DoCopyFiles() )
        return( false );
    CopySetupInfFile();
    RemoveExtraFiles();
    RemoveUnusedDirs();
    StatusCancelled(); /* make sure display gets updated */
    return( true );
}


static bool NukePath( char *path, int status )
/********************************************/
{
    DIR                 *d;
    struct dirent       *info;
    char                *path_end;
#if defined( __UNIX__ )
    struct stat         statbuf;
#endif

    d = opendir( path );
    ConcatDirSep( path );
    path_end = path + strlen( path );
    while( (info = readdir( d )) != NULL ) {
        strcpy( path_end, info->d_name );
#if defined( __UNIX__ )
        stat( path, &statbuf );
        if( S_ISDIR( statbuf.st_mode ) ) {
#else
        if( info->d_attr & _A_SUBDIR ) {
#endif
            if( info->d_name[0] != '.' ) {
                if( !NukePath( path, status ) )
                    return( false );
                rmdir( path );
            }
        } else {
#if defined( __UNIX__ )
            if( (statbuf.st_mode & S_IWUSR) == 0 || !S_ISREG( statbuf.st_mode ) ) {
#else
            if( info->d_attr & (_A_RDONLY | _A_SYSTEM | _A_HIDDEN) ) {
#endif
                chmod( path, PMODE_W_USR );
            }
            if( remove( path ) != 0 ) {
                return( false );
            }
        }
        StatusLines( status, path );
    }
    *path_end = '\0';
    closedir( d );
    return( true );
}


// *********************** Miscellaneous Function ****************************

void DeleteObsoleteFiles( void )
/******************************/
{
    int         i, max_deletes;
    int         group;
    dlg_state   state = DLG_NEXT;
    char        buff[_MAX_PATH];
    bool        *found;
    bool        found_any;

    max_deletes = SimNumDeletes();
    group = 1;
    for( i = 0; i < max_deletes; ++i ) {
        if( SimDeleteIsDialog( i ) ) {
            ++group;
        }
    }
    found = GUIMemAlloc( sizeof( bool ) * group );
    memset( found, false, sizeof( bool ) * group );
    found_any = false;
    group = 0;
    for( i = 0; i < max_deletes; ++i ) {
        if( SimDeleteIsDialog( i ) ) {
            ++group;
        } else {
            ReplaceVars( buff, sizeof( buff ), SimDeleteName( i ) );
            if( access( buff, F_OK ) == 0 ) {
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
                ReplaceVars( buff, sizeof( buff ), SimDeleteName( i ) );
                if( SimDeleteIsDialog( i ) ) {
                    state = DoDialog( buff );
                } else if( state == DLG_NEXT ) {
                    if( SimDeleteIsDir( i ) ) {
                        NukePath( buff, STAT_REMOVING );
                        rmdir( buff );
                    } else {
                        StatusLines( STAT_REMOVING, buff );
                        remove( buff );
                    }
                }
            }
        }
    }
    GUIMemFree( found );
}

char *GetInstallName( void )
/**************************/
{
    static char name[_MAX_FNAME];
    int         argc;
    char        **argv;

    if( name[0] == '\0' ) {
        if( GetVariableByName( "InstallerName" ) != NO_VAR ) {
            strncpy( name, GetVariableStrVal( "InstallerName" ), sizeof( name ) );
            name[sizeof( name ) - 1] = '\0';
            return( name );
        }

        GUIGetArgs( &argv, &argc );
        _splitpath( argv[0], NULL, NULL, name, NULL );
        strupr( name );
    }
    return( name );
}

char *AddInstallName( char *text, bool dorealloc )
/************************************************/
{
    size_t              len;
    size_t              inst_len;
    char                *inst_name;
    char                *p;
    int                 offset;

    len = strlen( text ) + 1;
    inst_name = GetInstallName();
    inst_len = strlen( inst_name );
    for( ;; ) {
        // p = strchr( text, '@' ); no good for dbcs!!!
        p = text;
        for( ;; ) {
            if( *p == '\0' ) {
                p = NULL;
                break;
            }
            if( *p == '@' )
                break;
            p += GUICharLen( *p );
        }
        if( p == NULL )
            break;
        offset = p - text;
        if( dorealloc ) {
            text = GUIMemRealloc( text, len + inst_len );
            p = text + offset;
        }
        memmove( p + inst_len, p + 1, len - (p + 1 - text) );
        memcpy( p, inst_name, inst_len );
        p += inst_len;
        len += inst_len;
    }
    return( text );
}

gui_message_return MsgBox( gui_window *gui, const char *messageid,
                                  gui_message_type wType, ... )
/*************************************************************/
{
    gui_message_return  result;
    char                buff[1024];
    char                msg[1024];
    const char          *errormessage;
    va_list             arglist;
    int                 i;
    int                 msg_index;

    if( stricmp( messageid, "IDS_NOSETUPINFOFILE" ) == 0 ) {
        // If the message is "can't find the setup.inf file", then
        // don't look up the string, because it is in the file we can't find
        errormessage = "The file %s cannot be found.";
    } else {
        errormessage = GetVariableStrVal( messageid );
    }
    if( errormessage == NULL ) {
        strcpy( buff, GetVariableStrVal( "IDS_UNKNOWNERROR" ) );
    } else {
        va_start( arglist, wType );
        vsprintf( buff, errormessage, arglist );
        va_end( arglist );
    }
    AddInstallName( buff, false );

    if( gui == NULL ) {
//      wType |= GUI_SYSTEMMODAL;
    }

    if( GUIIsGUI() ){
        if( wType & GUI_YES_NO ) {
            wType |= GUI_QUESTION;
        } else {
            wType |= GUI_INFORMATION;
        }
    }

//  following code removed - causes infinite loop (and buff will never be NULL)
//  if( buff == NULL || buff[0] == '\0' ) {
//      SetupError( "IDS_NULLSTRING" );
//  }

    msg_index = 0;
    for( i = 0; i < strlen( buff ); i++ ) {
        if( buff[i] == '&' ) {
            continue;              // skip the '& character.
        }
        msg[msg_index] = buff[i];
        msg_index++;
    }
    msg[msg_index] = '\0';

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
        result = GUIDisplayMessage( gui == NULL ? MainWnd : gui, msg, GetInstallName(),
                                    wType );
    }
    return( result );
}


bool PromptUser( char *name, char *dlg, char *skip, char *replace, int *value )
/*****************************************************************************/
{
    dlg_state                   return_state;

    SetVariableByName( "FileDesc", name );

    // don't display the dialog if the user selected the "Skip dialog" option
    if( GetVariableIntVal( skip ) == 0 ) {
        for( ;; ) {
            return_state = DoDialog( dlg );
            if( return_state != DLG_DONE && return_state != DLG_CAN )
                break;
            if( MsgBox( NULL, "IDS_QUERYABORT", GUI_YES_NO ) == GUI_RET_YES ) {
                CancelSetup = true;
                return( false );
            }
        }
    }

    *value = (GetVariableIntVal( replace ) == 1);
    return( true );
}

static void AddDefine( char *def )
/********************************/
// Process command line switch to set a variable
//     -dvariable=value
{
    char                *p;
    DEF_VAR             *var;

    p = strchr( def, '=' );
    if( p != NULL ) {
        *p = '\0';
        ++p;
        var = GUIMemAlloc( sizeof( DEF_VAR ) );
        var->variable = strdup( def );
        var->value = strdup( p );
        var->link = ExtraVariables;
        ExtraVariables = var;
    }
}

static void DefineVars( void )
/****************************/
// Create variables specified on command line
{
    DEF_VAR             *var;

    for( var = ExtraVariables; var != NULL; var = var->link ) {
        SetVariableByName( var->variable, var->value );
    }
}

char *GetSelfWithPath( char *buff, int len, char **argv )
/*******************************************************/
{
#if defined( __UNIX__ )
    int result;

    // code stolen from watcom/c/clibext.c

    result = readlink( "/proc/self/exe", buff, len );
    if( result == -1 ) {
        // try another way for BSD
        result = readlink( "/proc/curproc/file", buff, len );
    }
    if( result != -1 && result != len ) {
        // readlink does not add a NUL so we need to do it ourselves
        buff[result] = '\0';
        return( buff );
    }
    // fall back to argv[0] if readlink doesn't work
#else
    len=len;
#endif
    return( strcpy( buff, argv[0] ) );
}

bool GetDirParams( int argc, char **argv, char **inf_name, char **src_path, char **arc_name )
/*******************************************************************************************/
{
    char                dir[_MAX_DIR];
    char                drive[_MAX_DRIVE];
    int                 i;

    *inf_name = GUIMemAlloc( _MAX_PATH );
    if( *inf_name == NULL ) {
        return( false );
    }

    *src_path = GUIMemAlloc( _MAX_PATH );
    if( *src_path == NULL ) {
        GUIMemFree( *inf_name );
        return( false );
    }

    *arc_name = GUIMemAlloc( _MAX_PATH );
    if( *arc_name == NULL ) {
        GUIMemFree( *inf_name );
        GUIMemFree( *src_path );
        return( false );
    }

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
        if( argv[i][0] == '-' || argv[i][0] == '/' ) {
#endif
            switch( argv[i][1] ) {
            case '?':
                {
                    char * msg = "Usage: @ [-options]\n\n" \
                        "Supported options (case insensitive):\n\n" \
                        "-f=script\t\tspecify script file to override setup.inf\n" \
                        "-d<name=val>\tdefine a variable for the installer\n" \
                        "-i\t\tinvisible: shows no dialogs; infers -s\n" \
                        "-s\t\tskips dialogs but shows install progress\n" \
                        "-np\t\tdoes not create Program Manager entries\n" \
                        "-ns\t\tdoes not register startup information (paths, environment)\n" ;

                    InitGlobalVarList();
                    SetVariableByName( "IDS_USAGE", "%s");
                    MsgBox( NULL, "IDS_USAGE", GUI_OK, msg );

                    /* return false to terminate installer */
                    return( false );
                }
            case 'f': // Process "script" file to override variables in setup.inf
            case 'F':
                if( argv[i][2] == '=' && argv[i][3] != '\0' &&
                    access( &argv[i][3], R_OK ) == 0 ) {
                    VariablesFile = strdup( &argv[i][3] );
                }
                break;
            case 'd':
            case 'D':
                AddDefine( &argv[i][2] );
                break;
            case 'i': // No screen output (requires SkipDialogs below as well)
            case 'I':
                Invisible = true;
                // falling through!!!

            case 's': // Skip dialogs
            case 'S':
                SkipDialogs = true;
                break;
                
            case 'n':
            case 'N':
                if( argv[i][2] == 's' || argv[i][2] == 'S' ) {
                    StartupChange = false;
                } else if( argv[i][2] == 'p' || argv[i][2] == 'P' ) {
                    ProgramGroups = false;
                }
                break; 
            }

            i++;
        } else {
            break;
        }
    }

    if( i < argc ) {
        strcpy( *arc_name, argv[i] );
        i++;
    } else {
        GetSelfWithPath( *arc_name, _MAX_PATH, argv );
    }

    if( i < argc ) {
        strcpy( *inf_name, argv[i] );
        i++;
    } else {
        char        buff[_MAX_PATH];

        // If archive exists, expect setup.inf inside. Otherwise assume
        // it's right next to the setup executable.
        if( access( *arc_name, R_OK ) == 0 ) {
            strcpy( *inf_name, "setup.inf" );
        } else {
            GetSelfWithPath( *inf_name, _MAX_PATH, argv );
            _splitpath( *inf_name, drive, dir, NULL, NULL );
            _makepath( buff, drive, dir, "setup", "inf" );
            _fullpath( *inf_name, buff, _MAX_PATH );
        }
    }

    if( i < argc ) {
        strcpy( *src_path, argv[i] );
    } else {
        _splitpath( *inf_name, drive, dir, NULL, NULL );
        _makepath( *src_path, drive, dir, NULL, NULL );
    }
    RemoveDirSep( *src_path );

    return( true );
}


bool FreeDirParams( char **inf_name, char **src_path, char **arc_name )
/*********************************************************************/
{
    if( inf_name == NULL || src_path == NULL || arc_name == NULL )
        return( false );

    GUIMemFree( *inf_name );
    GUIMemFree( *src_path );
    GUIMemFree( *arc_name );

    *inf_name = NULL;
    *src_path = NULL;
    *arc_name = NULL;

    return( true );
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

    while( fgets( buf, sizeof( buf ), fp ) != NULL ) {
        line = buf;
        while( isspace( *line ) != 0 ) {
            line++;
        }
        if( *line == '#' ) {
            continue;
        }
        while( strlen( line ) > 0 && isspace( line[strlen( line ) - 1] ) ) {
            line[strlen( line ) - 1] = '\0';
        }
        variable = strtok( line, " =\t" );
        value = strtok( NULL, "=\t\0" );
        if( value != NULL ) {
            while( isspace( *value ) ) {
                value++;
            }

            while( strlen( value ) > 0 &&
                   isspace( value[strlen( value ) - 1] ) ) {
                value[strlen( value ) - 1] = '\0';
            }
            if( variable != NULL ) {
                if( name == NULL || stricmp( name, variable ) == 0 ) {
                    if( stricmp( value, "true" ) == 0 ) {
                        SetVariableByName( variable, "1" );
                    } else if( stricmp( value, "false" ) == 0 ) {
                        SetVariableByName( variable, "0" );
                    } else {
                        SetVariableByName( variable, value );
                    }
                }
            }
        }
    }
    fclose( fp );
}

bool InitInfo( char *inf_name, char *src_path )
/*********************************************/
// initialize global vbls. and read setup.inf into memory.
{
    char                dir[_MAX_DIR];
    char                drive[_MAX_DRIVE];
    int                 ret;

    SetVariableByName( "SrcDir", src_path );
    DetermineSrcState( src_path );
    SetVariableByName( "SrcDir2", src_path );
    _splitpath( inf_name, drive, dir, NULL, NULL );

    ret = SimInit( inf_name );
    if( ret == SIM_INIT_NOERROR ) {
        DefineVars();
        if( VariablesFile != NULL ) {
            ReadVariablesFile( NULL );
        }
        return( true );
    }
    if( ret == SIM_INIT_NOMEM ) {
        MsgBox( NULL, "IDS_NOMEMORY", GUI_OK, inf_name );
    } else { // SIM_INIT_NOFILE
        MsgBox( NULL, "IDS_NOSETUPINFOFILE", GUI_OK, inf_name );
    }
    return( false );
}

void CloseDownMessage( bool installed_ok )
/****************************************/
{
    if( installed_ok ) {
        if( VarGetIntVal( UnInstall ) != 0 ) {
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
        if( VarGetIntVal( UnInstall ) != 0 ) {
            MsgBox( NULL, "IDS_UNSETUPNOGOOD", GUI_OK );
        } else {
            MsgBox( NULL, "IDS_SETUPNOGOOD", GUI_OK );
        }
    }
}


void CloseDownProgram( void )
/***************************/
{
    StatusFini();
    SetupFini();
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

char *stristr( char *str, char *substr )
/**************************************/
{
    size_t  str_len;
    size_t  substr_len;

    str_len = strlen( str );
    substr_len = strlen( substr );
    while( str_len >= substr_len ) {
        if( strnicmp( str, substr, substr_len ) == 0 )
            return( str );
        ++str;
        --str_len;
    }
    return( NULL );
}


void SetupError( const char *msg )
/********************************/
{
//    MsgBox( NULL, "IDS_ERROR", GUI_OK, msg );
    MsgBox( NULL, msg, GUI_OK );
}
