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
* Description:  File open dialog.
*
****************************************************************************/


#include "guiwind.h"

#if defined(__OS2__) || defined(__OS2_PM__)
    #ifndef OS2_INCLUDED
        #undef NULL
        #define INCL_DOSMISC
        #include <os2.h>
    #endif
#elif defined(__NT__)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif defined(__RDOS__)
    #include "rdos.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#if defined( __QNX__ )
    #include <dirent.h>
    #include <unistd.h>
    #include <sys/disk.h>
    #include <fnmatch.h>
#elif defined( __LINUX__ )
    #include <dirent.h>
    #include <unistd.h>
    #include <fnmatch.h>
#elif defined( __UNIX__ )
    #include <dirent.h>
    #include <unistd.h>
    #ifdef SGI
        #include "fnmatch.h"    // We get fnmatch from wclib
    #else
        #include <fnmatch.h>    // fnmatch is found in the standard library
    #endif
#else
  #if defined( __NETWARE__ )
    #include <fnmatch.h>
  #endif
    #include <direct.h>
    #include <dos.h>
#endif
#include "walloca.h"
#include "guifdlg.h"
#include "guidlg.h"
#include "guistr.h"

#include "clibext.h"


#if defined( __UNIX__ ) || defined( __NETWARE__ )
    #define FILE_SEP    "/"
    #define FILE_SEP_CHAR       '/'
    #define FILES_ALL   "*"
#else
    #define FILE_SEP    "\\"
    #define FILE_SEP_CHAR       '\\'
    #define FILES_ALL   "*.*"
#endif

#define INDENT_STR          " "
#define INDENT_CHAR         ' '
#define OPENED_DIR_CHAR     '-'
#define UNOPENED_DIR_CHAR   '+'

typedef struct {
    open_file_name      *currOFN;
    const char          **fileExtensions;
    char                *currExt;
    int                 currExtIndex;
    int                 dialogRC;
    bool                initted;
} dlg_info;

typedef enum {
    DRIVE_NONE,
    DRIVE_IS_REMOVABLE,
    DRIVE_IS_FIXED
} drive_type;

typedef enum {
    PROCESS_TRUE,
    PROCESS_FALSE,
    PROCESS_FAIL
} process_rc;

enum {
    CTL_CANCEL = 1,
    CTL_OK,
    CTL_FILE_LIST,
    CTL_DIR_LIST,
    CTL_EDIT,
    CTL_DIR_NAME,
    CTL_FILE_TYPES,
    CTL_DRIVES
};

#define DLG_FILE_ROWS   14
#define DLG_FILE_COLS   65
#define BOX_WIDTH       20
#define BOX_WIDTH2      25

#define DIR_START       27

// if dlgControls is modified then make sure the function InitDlgControls
// matches it
static gui_control_info dlgControls[] =
{
/*  0 */ DLG_STRING(    NULL, 2, 0, 11 ),
/*  1 */ DLG_STRING(    NULL, DIR_START, 0, DIR_START+12 ),
/*  2 */ DLG_EDIT(      NULL, CTL_EDIT, 2, 1, BOX_WIDTH+3 ),
/*  3 */ DLG_DYNSTRING( NULL, CTL_DIR_NAME, DIR_START, 1, DLG_FILE_COLS-1 ),
/*  4 */ DLG_LIST_BOX(  NULL, CTL_FILE_LIST,    2, 3, 2+BOX_WIDTH, 9 ),
/*  5 */ DLG_LIST_BOX(  NULL, CTL_DIR_LIST,     DIR_START, 3, DIR_START+BOX_WIDTH2, 9 ),
/*  6 */ DLG_DEFBUTTON( NULL, CTL_OK,   (DIR_START+BOX_WIDTH2+4), 4, (DIR_START+BOX_WIDTH2+14) ),
/*  7 */ DLG_BUTTON(    NULL, CTL_CANCEL,       (DIR_START+BOX_WIDTH2+4), 6, (DIR_START+BOX_WIDTH2+14) ),
/*  8 */ DLG_STRING(    NULL, 2, 11, 20 ),
/*  9 */ DLG_COMBO_BOX( NULL, CTL_FILE_TYPES, 2,12,2+BOX_WIDTH+3,15 ),
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
/* 10 */ DLG_STRING(    NULL, DIR_START+2, 11, DIR_START+8 ),
/* 11 */ DLG_COMBO_BOX( NULL, CTL_DRIVES, DIR_START+2,12,DIR_START+BOX_WIDTH,15 )
#endif
};

static bool     ControlsInitialized = false;

#define FILE_LIST_INDEX         4
#define DIR_LIST_INDEX          5
#define FILE_TYPES_INDEX        9
#define DRIVE_LIST_INDEX        11

#define NUM_CONTROLS ( sizeof( dlgControls ) / sizeof( gui_control_info ) )

#define GetDriveTextList()      ((const char **)dlgControls[DRIVE_LIST_INDEX].text)
#define SetDriveTextList(x)     dlgControls[DRIVE_LIST_INDEX].text = ((const char *)(x))
#define freeDriveTextList( )    freeStringList((const char ***)&dlgControls[DRIVE_LIST_INDEX].text)
#define GetFileTypesTextList()  ((const char **)dlgControls[FILE_TYPES_INDEX].text)
#define SetFileTypesTextList(x) dlgControls[FILE_TYPES_INDEX].text = ((const char *)(x))
#define freeFileTypesTextList() freeStringList((const char ***)&dlgControls[FILE_TYPES_INDEX].text)
#define GetFileExtsTextList()   (dlg.fileExtensions)
#define SetFileExtsTextList(x)  dlg->fileExtensions = (x)
#define freeFileExtsTextList()  freeStringList(&dlg.fileExtensions)

#if defined( __UNIX__ ) || defined( __NETWARE__ )
  #define PC '/'
#else   /* DOS, OS/2, Windows */
  #define PC '\\'
  #define ALT_PC '/'
#endif

static void InitDlgControls( void )
{
/*  0 */ dlgControls[0].text = LIT( File_Name_Colon );
/*  1 */ dlgControls[1].text = LIT( Directories_Colon );
/*  2 */ dlgControls[2].text = LIT( Empty );
/*  3 */ dlgControls[3].text = LIT( Empty );
/*  6 */ dlgControls[6].text = LIT( OK );
/*  7 */ dlgControls[7].text = LIT( Cancel );
/*  8 */ dlgControls[8].text = LIT( List_Files_of_Type_Colon );
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
/* 10 */ dlgControls[10].text = LIT( Drives_Colon );
#endif
}

static void copyPart( char *buff, char *p, int len, int maxlen )
{
    if( buff != NULL ) {
        if( len > maxlen ) {
            len = maxlen;
        }
        memcpy( buff, p, len );
        buff[len] = 0;
    }
}

/*
 * splitPath - need because C library will truncate fname and ext too early
 */
static void splitPath( char *path, char *drive, char *dir, char *fname,
                        char *ext )
{
    char        *dotp;
    char        *fnamep;
    char        *startp;
    char        ch;

#if defined( __UNIX__ ) || defined( __NETWARE__ )
    /* process node/drive specification */
    startp = path;
    if( path[0] == FILE_SEP_CHAR && path[1] == FILE_SEP_CHAR ) {
        path += 2;
        for( ;; ) {
            ch = *path;
            if( ch == '\0' || ch == FILE_SEP_CHAR || ch == '.' ) {
                break;
            }
            path++;
        }
    }
    copyPart( drive, startp, (int)( path - startp ), _MAX_DRIVE );
#else
    /* processs drive specification */
    if( path[0] != 0  &&  path[1] == ':' ) {
        if( drive != NULL ) {
            drive[0] = path[0];
            drive[1] = ':';
            drive[2] = 0;
        }
        path += 2;
    } else if( drive != NULL ) {
        drive[0] = 0;
    }
#endif

    dotp = NULL;
    fnamep = path;
    startp = path;

   for( ;; ) {
        ch = *path;
        if( ch == 0 ) {
            break;
        }
        if( ch == '.' ) {
            dotp = path;
            path++;
            continue;
        }
        path++;
#if defined( __UNIX__ ) || defined( __NETWARE__ )
        if( ch == FILE_SEP_CHAR ) {
#else
        if( ch == FILE_SEP_CHAR  ||  ch == '/' ) {
#endif
            fnamep = path;
            dotp = NULL;
        }
    }
    copyPart( dir, startp, (int)( fnamep - startp ), _MAX_DIR - 1 );
    if( dotp == NULL ) {
        dotp = path;
    }
#if defined( __UNIX__ ) || defined( __NETWARE__ )
    if( ext == NULL )  {
        dotp = path;
    }
#endif
    copyPart( fname, fnamep, (int)( dotp - fnamep ), _MAX_PATH - 1 );
    copyPart( ext, dotp, (int)( path - dotp ), _MAX_PATH - 1);

} /* splitPath */

#ifdef __OS2__
#ifdef _M_I86
#define STUPID_UINT     unsigned short
#else
#define STUPID_UINT     unsigned long
#endif
static drive_type getDriveType( char drive )
{
    STUPID_UINT         disk;
    unsigned long       map;
    char                drv;

    DosQCurDisk( &disk, &map );
    for( drv = 'A'; drv <= 'Z'; drv++ ) {
        if( drive == drv ) {
            if( map & 1 ) {
                return( DRIVE_IS_FIXED );
            } else {
                return( DRIVE_NONE );
            }
        }
        map >>= 1;
    }
    return( DRIVE_NONE );
}
#elif defined( __UNIX__ ) || defined( __NETWARE__ )
#elif defined( __NT__ )
static drive_type getDriveType( char drv )
{
    drive_type type;
    UINT       ret;
    char       drive[4];

    drive[0] = drv;
    drive[1] = ':';
    drive[2] = '\\';
    drive[3] = 0;

    ret = GetDriveType( drive );
    switch ( ret ) {
        case DRIVE_REMOVABLE:
            type = DRIVE_IS_REMOVABLE;
            break;

        case DRIVE_FIXED:
        case DRIVE_REMOTE:
        case DRIVE_CDROM:
        case DRIVE_RAMDISK:
            type = DRIVE_IS_FIXED;
            break;

        case 0:
        case 1:
        default:
            type = DRIVE_NONE;
    }

    return ( type );
}
#elif defined( __RDOS__ )
static drive_type getDriveType( char drv )
{
    drive_type type;
    int        CurDrive = RdosGetCurDrive();

    if( RdosSetCurDrive( drv - 'A' ) )
        type = DRIVE_IS_FIXED;
    else
        type = DRIVE_NONE;

    RdosSetCurDrive( CurDrive );

    return( type );
}
#else
extern short CheckRemovable( char );
#pragma aux CheckRemovable = \
        "mov    ax,04408h" \
        "int    021h" \
        "cmp    ax,0fh" \
        "jne    ok" \
        "mov    ax,0" \
        "jmp    done" \
        "ok:    inc ax" \
        "done:" \
        parm [bl] value[ax];

static drive_type getDriveType( char drv )
{
    return( CheckRemovable( drv - 'A' + 1 ) );
}
#endif

/*
 * hasWild - see if a file name has a wild card in it
 */
static bool hasWild( const char *txt )
{
    bool        has_wild;
    size_t      i;
    size_t      len;

    has_wild = false;
    len = strlen( txt );
    for( i = 0; i < len; i++ ) {
        if( txt[i] == '?' || txt[i] == '*' ) {
            has_wild = true;
            break;
        }
    }
    return( has_wild );

} /* hasWild */

/*
 * addToList - add an item to a list of items
 */
static bool addToList( const char ***list, int num, const char *data, size_t len )
{
    char    *str;

    *list = (const char **)GUIMemRealloc( (void *)*list, ( num + 2 ) * sizeof( char * ) );
    if( *list == NULL ) {
        return( false );
    }
    ++len;
    str = GUIMemAlloc( len );
    if( str == NULL ) {
        return( false );
    }
    memcpy( str, data, len );
    (*list)[num] = str;
    (*list)[num + 1] = NULL;
    return( true );

} /* addToList */

/*
 * freeStringList - release an array of strings
 */
static void freeStringList( const char ***list )
{
    int         cnt;

    if( *list == NULL ) {
        return;
    }
    cnt = 0;
    while( (*list)[cnt] != NULL ) {
        GUIMemFree( (void *)(*list)[cnt] );
        cnt++;
    }
    GUIMemFree( (void *)*list );
    *list = NULL;

} /* freeStringList */

#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
/*
 * buildDriveList - get a list of all drives
 */
static bool buildDriveList( void )
{
    char        drv;
    int         cnt;
    char        str[_MAX_PATH];
    const char  **list;

    cnt = 0;
    list = NULL;
    for( drv = 'A'; drv <= 'Z'; drv++ ) {
        if( getDriveType( drv ) != DRIVE_NONE ) {
            str[0] = drv;
            str[1] = ':';
            str[2] = '\0';
            if( !addToList( &list, cnt, str, 2 ) ) {
                freeStringList( &list );
                break;
            }
            cnt++;
        }
    }
    SetDriveTextList( list );
    return( list != NULL );

} /* buildDriveList */
#endif

/*
 * buildFileTypesExts - get a lists of all file types and extensions
 */
static bool buildFileTypesExts( dlg_info *dlg, const char *data )
{
    size_t      len;
    const char  **list1;
    const char  **list2;
    int         num;
    bool        ok;

    list1 = NULL;
    list2 = NULL;
    if( data != NULL ) {
        num = 0;
        ok = true;
        while( *data != '\0' ) {
            len = strlen( data );
            ok = addToList( &list1, num, data, len );
            if( !ok ) {
                break;
            }
            data += len + 1;
            len = strlen( data );
            ok = addToList( &list2, num, data, len );
            if( !ok ) {
                break;
            }
            data += len + 1;
            num++;
        }
        if( !ok ) {
            freeStringList( &list1 );
            freeStringList( &list2 );
        }
    }
    SetFileTypesTextList( list1 );
    SetFileExtsTextList( list2 );
    return( list1 != NULL && list2 != NULL );

} /* buildFileTypesExts */

/*
 * goToDir - go to a specified directory
 */
static bool goToDir( gui_window *gui, char *dir )
{
    char        drive[_MAX_DRIVE];
    unsigned    total;
    bool        removed_end;
    size_t      len;
    int         rc;

    gui = gui;
    if( dir == NULL ) {
        return( false );
    }

    if( dir[0] == 0 ) {
        return( true );
    }

    if( !(dir[1] == ':' && dir[2] == 0) ) {
        len = strlen( dir );
        removed_end = false;
        if( dir[len - 1] == FILE_SEP_CHAR ) {
            if( len > 1 && dir[len - 2] != ':' ) {
                dir[len - 1] = 0;
                removed_end = true;
            }
        }

#ifdef __OS2__
        /* prevent hard error popup */
        DosError( 1 );
#endif
        rc = chdir( dir );
#ifdef __OS2__
        /* allow hard error popup: Grrr can't restore to original state */
        DosError( 0 );
#endif
        if( removed_end ) {
            dir[len - 1] = FILE_SEP_CHAR;
        }
        if( rc ) {
            return( false );
        }
    }

    splitPath( dir, drive, NULL, NULL, NULL );
    if( drive[0] != 0 ) {
#if defined( __UNIX__ ) || defined( __NETWARE__ )
        total = 1;
#else
        _dos_setdrive( toupper( drive[0] ) - 'A' + 1, &total );
#endif
    }
    return( true );

} /* goToDir */

/*
 * Compare - quicksort comparison with special treatment of indent chars
 */
/* int Compare( const char **p1, const char **p2 ) */
int Compare( const void  *p1, const void *p2 )
{
    const char  *s1 = *(const char **)p1;
    const char  *s2 = *(const char **)p2;

    /* skip matching indents */
    while( *s1 == INDENT_CHAR && *s2 == INDENT_CHAR ) {
        ++s1;
        ++s2;
    }
    /* indents compare as chars with infinitely high value */
    if( *s1 == INDENT_CHAR )
        return( 1 );
    else if( *s2 == INDENT_CHAR )
        return( -1 );

    /* use regular string comparison for the rest */
    return( strcasecmp( s1, s2 ) );

} /* Compare */

#if defined( __QNX__ )
static void getstatus( struct dirent *dent, char *path )
{
    char        fullname[_MAX_PATH];

    if( !(dent->d_stat.st_status & _FILE_USED) ) {
        _makepath( fullname, NULL, path, dent->d_name, NULL );
        stat( fullname, &dent->d_stat );
    }
}

static bool isdir( struct dirent *dent, char *path )
{
    getstatus( dent, path );
    return( S_ISDIR( dent->d_stat.st_mode ) );
}

static bool isrdonly( struct dirent *dent, char *path )
{
    unsigned    bit;
    uid_t       user;

    user = geteuid();
    if( user == 0 ) {
        /* we're root - we can alway write the file */
        return( false );
    }
    getstatus( dent, path );
    if( dent->d_stat.st_uid == user ) {
        bit = S_IWUSR;
    } else if( dent->d_stat.st_gid == getegid() ) {
        bit = S_IWGRP;
    } else {
        bit = S_IWOTH;
    }
    return( !(dent->d_stat.st_mode & bit) );
}
#elif defined( __UNIX__ )
static bool isdir( struct dirent *dent, char *path )
{
    struct stat stats;

    // FIXME: implement a "_stat2()" equivalent.
    //_stat2( path, dent->d_name, &stats );
    stat( dent->d_name, &stats );
    return( S_ISDIR( stats.st_mode ) );
}

static bool isrdonly( struct dirent *dent, char *path )
{
    unsigned    bit;
    uid_t       user;
    struct stat stats;

    user = geteuid();
    if( user == 0 ) {
        /* we're root - we can alway write the file */
        return( false );
    }
    // FIXME: implement a "_stat2()" equivalent.
    //_stat2( path, dent->d_name, &stats );
    stat( dent->d_name, &stats );
    if( stats.st_uid == user ) {
        bit = S_IWUSR;
    } else if( stats.st_gid == getegid() ) {
        bit = S_IWGRP;
    } else {
        bit = S_IWOTH;
    }
    return( (stats.st_mode & bit) == 0 );
}
#else
static bool isdir( struct dirent *dent, char *path )
{
    path = path;
    return( dent->d_attr & _A_SUBDIR );
}

static bool isrdonly( struct dirent *dent, char *path )
{
    path = path;
    return( dent->d_attr & _A_RDONLY );
}
#endif
/*
 * setFileList - get list of files in current directory
 */
static bool setFileList( gui_window *gui, const char *ext )
{
    char                path[_MAX_PATH];
    DIR                 *directory;
    struct dirent       *dent;
    char                *ptr;
    const char          **list;
    int                 cnt;
    char                ext1[_MAX_PATH];
    int                 i;
    dlg_info            *dlg = GUIGetExtra( gui );

    cnt = 0;
    list = NULL;
    strcpy( ext1, ext );

    ptr = strtok( ext1, ";" );
    while( ptr != NULL ) {

        if( getcwd( path, sizeof( path ) ) == NULL ) {
            break;
        }

#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        if( path[strlen(path)-1] != FILE_SEP_CHAR ) {
            strcat( path, FILE_SEP );
        }
        strcat( path, ptr );
#endif

        directory = opendir( path );
        if( directory != NULL ) {
            while( ( dent = readdir( directory ) ) != NULL ) {
                if( !isdir( dent, path ) ) {
                    if( ( dlg->currOFN->flags & OFN_HIDEREADONLY ) && isrdonly( dent, path ) ) {
                        continue;
                    }
#if defined( __UNIX__ ) || defined( __NETWARE__ )
                    if( fnmatch( ptr, dent->d_name, FNM_PATHNAME ) != 0 ) {
                        continue;
                    }
#endif
                    if( !addToList( &list, cnt, dent->d_name, strlen( dent->d_name ) ) ) {
                        freeStringList( &list );
                        closedir( directory );
                        return( false );
                    }
                    cnt++;
                }
            }
            closedir( directory );
        }
        ptr = strtok( NULL, ";" );
    }
    GUIClearList( gui, CTL_FILE_LIST );
    if( cnt > 0 ) {
        qsort( (void *)list, cnt, sizeof( char * ), Compare );
        for( i = 0; i < cnt; i++ ) {
            GUIAddText( gui, CTL_FILE_LIST, list[i] );
        }
        freeStringList( &list );
    }
    return( true );

} /* setFileList */

/*
 * setDirList - set current directory list
 */
static bool setDirList( gui_window *gui )
{
    char                path[_MAX_PATH];
    char                dir[_MAX_DIR];
    char                drive[_MAX_DRIVE + 3];
    DIR                 *directory;
    struct dirent       *dent;
    char                *ptr,*start;
    char                indent[80];
    char                tmp[256];
    const char          **drvlist;
    int                 i;
    size_t              len;
    int                 curr,cnt;
    const char          **list;

    GUIClearList( gui, CTL_DIR_LIST );
    cnt = 0;
    list = NULL;

    if( getcwd( path, sizeof( path ) ) == NULL ) {
        return( true );
    }

    if( path[strlen( path ) - 1] == FILE_SEP_CHAR ) {
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        strcat( path, FILES_ALL );
#endif
    } else {
#if defined( __UNIX__ ) || defined( __NETWARE__ )
        strcat( path, FILE_SEP );
#else
        strcat( path, FILE_SEP FILES_ALL );
#endif
    }
    splitPath( path, drive + 1, dir, NULL, NULL );

    directory = opendir( path );
    if( directory == NULL ) {
        return( false );
    }

    drive[0] = OPENED_DIR_CHAR;
    drvlist = NULL;
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    drvlist = GetDriveTextList();
#endif
    i = 0;
    while( drvlist != NULL ) {
        if( drvlist[i] == NULL ) {
            break;
        }
        if( drvlist[i][0] == drive[1] ) {
            GUISetCurrSelect( gui, CTL_DRIVES, i );
            break;
        }
        i++;
    }
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    drive[3] = '\\';
    drive[4] = '\0';
#endif
    if( !addToList( &list, cnt, drive, strlen( drive ) ) ) {
        freeStringList( &list );
        return( false );
    }
    cnt++;
    strcpy( indent, INDENT_STR );

    ptr = dir + 1;
    start = ptr;
    while( *ptr != 0 ) {
        if( *ptr == FILE_SEP_CHAR ) {
            *ptr = 0;
            len = strlen( indent );
            memcpy( tmp, indent, len );
            tmp[len++] = OPENED_DIR_CHAR;
            strcpy( tmp + len, start );
            if( !addToList( &list, cnt, tmp, strlen( tmp ) ) ) {
                freeStringList( &list );
                return( false );
            }
            cnt++;
            start = ptr + 1;
            strcat( indent, INDENT_STR );
        }
        ptr++;
    }

    curr = cnt;
    while( ( dent = readdir( directory ) ) != NULL ) {
        if( isdir( dent, path ) ) {
            if( (dent->d_name[0] == '.') && ((dent->d_name[1] == 0) ||
                (dent->d_name[1] == '.' && dent->d_name[2] == 0)) ) {
                continue;
            }
            len = strlen( indent );
            memcpy( tmp, indent, len );
            tmp[len++] = UNOPENED_DIR_CHAR;
            strcpy( tmp + len, dent->d_name );
            if( !addToList( &list, cnt, tmp, strlen( tmp ) ) ) {
                freeStringList( &list );
                return( false );
            }
            cnt++;
        }
    }
    closedir( directory );

    qsort( (void *)list, cnt, sizeof( char * ), Compare );
    for( i = 0; i < cnt; i++ ) {
        GUIAddText( gui, CTL_DIR_LIST, list[i] );
    }
    GUISetCurrSelect( gui, CTL_DIR_LIST, curr - 1 );
    freeStringList( &list );

    return( true );

} /* setDirList */

/*
 * initDialog - initialize all dialog fields
 */
static bool initDialog( gui_window *gui, const char *ext, const char *name )
{
    char        path[_MAX_PATH];
    dlg_info    *dlg = GUIGetExtra( gui );

    if( ext != NULL && hasWild( ext ) ) {
        char    *str;
        size_t  len;

        len = strlen( ext ) + 1;
        str = GUIMemAlloc( len );
        GUIMemFree( dlg->currExt );
        dlg->currExt = str;
        if( str == NULL ) {
            return( false );
        }
        memcpy( str, ext, len );
    }
    if( !setFileList( gui, dlg->currExt ) ) {
        return( false );
    }
    if( !setDirList( gui ) ) {
        return( false );
    }
    getcwd( path, sizeof( path ) );
    GUISetText( gui, CTL_DIR_NAME, path );
    if( name != NULL && *name != '\0' ) {
        GUISetText( gui, CTL_EDIT, name );
    } else if( ext != NULL ) {
        GUISetText( gui, CTL_EDIT, ext );
    }
    return( true );

} /* initDialog */

/*
 * processFileName - process a new file name
 */
static process_rc processFileName( gui_window *gui )
{
    char        *tmp;
    char        *txt;
    size_t      len;
    char        path[_MAX_PATH];
    char        dir[_MAX_DIR];
    char        drive[_MAX_DRIVE];
    char        fname[_MAX_PATH];
    char        ext[_MAX_PATH];
    char        *buff;
    bool        has_wild;
    struct stat buf;
    int         rc;
    dlg_info            *dlg = GUIGetExtra( gui );

    tmp = GUIGetText( gui, CTL_EDIT );
    if( tmp == NULL ) {
        return( PROCESS_FALSE );
    }
    txt = alloca( strlen( tmp ) + 1 );
    if( txt == NULL ) {
        GUIMemFree( tmp );
        return( PROCESS_FALSE );
    }
    strcpy( txt, tmp );
    GUIMemFree( tmp );
    splitPath( txt, drive, dir, fname, ext );

    has_wild = hasWild( txt );
    if( has_wild && fname[0] == 0 ) {
        return( PROCESS_FALSE );
    }

    if( !has_wild ) {

        rc = stat( txt, &buf );
        if( !rc ) {
            if( S_ISDIR( buf.st_mode ) ) {
                goToDir( gui, txt );
                if( !initDialog( gui, dlg->fileExtensions[dlg->currExtIndex], NULL ) ) {
                    return( PROCESS_FAIL );
                }
                return( PROCESS_FALSE );
            }
        }
        _makepath( path, drive, dir, NULL, NULL );
        if( !goToDir( gui, path ) ) {
            return( PROCESS_FALSE );
        }
        if( !rc && (dlg->currOFN->flags & OFN_OVERWRITEPROMPT) ) {
            buff = alloca( strlen( txt ) + 100 );
            strcpy( buff, txt );
            strcat( buff, LIT( File_Exists_Replace ) );
            rc = GUIDisplayMessage( gui, buff, dlg->currOFN->title, GUI_YES_NO );
            if( rc == GUI_RET_NO ) {
                return( PROCESS_FALSE );
            }
        }
        _makepath( path, NULL, NULL, fname, ext );

        if( dlg->currOFN->base_file_name != NULL ) {
            len = strlen( txt );
            if( len >= dlg->currOFN->max_base_file_name ) {
                len = dlg->currOFN->max_base_file_name - 1;
            }
            memcpy( dlg->currOFN->base_file_name, txt, len );
            dlg->currOFN->base_file_name[len] = 0;
        }
        if( dlg->currOFN->file_name != NULL ) {
            getcwd( path, sizeof( path ) );
            len = strlen( path );
            if( path[len - 1] != FILE_SEP_CHAR ) {
                path[len] = FILE_SEP_CHAR;
                path[len + 1] = 0;
            }
            strcat( path, fname );
            strcat( path, ext );
            len = strlen( path );
            if( len >= dlg->currOFN->max_file_name ) {
                len = dlg->currOFN->max_file_name-1;
            }
            memcpy( dlg->currOFN->file_name, path, len );
            dlg->currOFN->file_name[len] = 0;
        }
        return( PROCESS_TRUE );
    }
    _makepath( path, drive, dir, NULL, NULL );
    if( !goToDir( gui, path ) ) {
        return( PROCESS_FALSE );
    }
    _makepath( path, NULL, NULL, fname, ext );
    if( !initDialog( gui, path, NULL ) ) {
        return( PROCESS_FAIL );
    }
    return( PROCESS_FALSE );

} /* processFileName */

/*
 * ProcessOKorDClick -- user clicked OK or double clicked on a file
 */
void ProcessOKorDClick( gui_window *gui, gui_ctl_id id  )
{
    process_rc  prc;
    int         sel;
    int         realsel;
    char        path[_MAX_PATH];
    char        *optr;
    char        *ptr;
    int         i;
    gui_ctl_id  focusid;
    dlg_info    *dlg = GUIGetExtra( gui );

    if( id == CTL_OK ) { /* hit enter or clicked ok */
        GUIGetFocus( gui, &focusid );
        switch( focusid ) {
        case CTL_DIR_LIST  :
            id = focusid;
            break;
        case CTL_FILE_LIST :
            ptr = GUIGetText( gui, CTL_FILE_LIST );
            GUISetText( gui, CTL_EDIT, ptr );
            GUIMemFree( ptr );
            break;
        }
    }
    switch( id ) {
    case CTL_FILE_LIST :
    case CTL_OK :
        prc = processFileName( gui );
        if( prc == PROCESS_TRUE ) {
            dlg->dialogRC = OFN_RC_FILE_SELECTED;
            GUICloseDialog( gui );
        } else if( prc == PROCESS_FAIL ) {
            dlg->dialogRC = OFN_RC_RUNTIME_ERROR;
            GUICloseDialog( gui );
        }
        break;
    case CTL_DIR_LIST :
        sel = GUIGetCurrSelect( gui, id );
#if defined( __UNIX__ ) || defined( __NETWARE__ )
        path[0] = FILE_SEP_CHAR;
        path[1] = 0;
#else
        path[0] = 0;
#endif
        realsel = 0;
        for( i=0;i<sel;i++ ) {
            ptr = GUIGetListItem( gui, id, i );
            if( ptr == NULL ) {
                return;
            }
            optr = ptr;
            while( *ptr == INDENT_CHAR ) {
                ptr++;
            }
            if( *ptr == '-' ) {
                strcat( path, ptr+1 );
                realsel++;
                if( i > 0 ) {
                    strcat( path, FILE_SEP );
                }
            } else {
                GUIMemFree( optr );
                break;
            }
            GUIMemFree( optr );
        }
        ptr = GUIGetListItem( gui, id, sel );
        if( ptr == NULL ) {
            return;
        }
        optr = ptr;
        while( *ptr == INDENT_CHAR ) {
            ptr++;
        }
        strcat( path, ptr+1 );
        GUIMemFree( optr );
        goToDir( gui, path );
        if( !initDialog( gui, NULL, NULL ) ) {
            dlg->dialogRC = OFN_RC_RUNTIME_ERROR;
            GUICloseDialog( gui );
        } else {
            GUISetCurrSelect( gui, id, realsel );
        }
        break;
   }

} /* ProcessOKorDClick */

static void InitTextList( gui_window *gui, gui_ctl_id id, const char **text_list )
{
    int         i;

    for( i = 0; text_list[i] != NULL; i++ ) {
        GUIAddText( gui, id, text_list[i] );
    }
    GUISetCurrSelect( gui, id, 0 );
}

/*
 * GetFileNameEvent - event handler for GetFileName dialog
 */
extern bool GetFileNameEvent( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    int         sel;
    char        *ptr;
    char        path[_MAX_PATH];
    dlg_info    *dlg = GUIGetExtra( gui );

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        dlg->initted = false;
        InitTextList( gui, CTL_FILE_TYPES, GetFileTypesTextList() );
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        InitTextList( gui, CTL_DRIVES, GetDriveTextList() );
#endif
        if( !initDialog( gui, dlg->fileExtensions[dlg->currExtIndex], dlg->currOFN->file_name ) ) {
            dlg->dialogRC = OFN_RC_FAILED_TO_INITIALIZE;
            return( false );
        }
        dlg->initted = true;
        GUISetFocus( gui, CTL_EDIT );
        return( true );
        break;
    case GUI_CONTROL_DCLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_FILE_LIST:
        case CTL_DIR_LIST:
            ProcessOKorDClick( gui, id );
            break;
        }
        break;
    case GUI_CONTROL_CLICKED:
        if( !dlg->initted )
            break;
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_OK:
            ProcessOKorDClick( gui, id );
            break;
        case CTL_CANCEL:
            GUICloseDialog( gui );
            break;
        case CTL_FILE_LIST:
            ptr = GUIGetText( gui, id );
            GUISetText( gui, CTL_EDIT, ptr );
            GUIMemFree( ptr );
            break;
        case CTL_DRIVES :
            sel = GUIGetCurrSelect( gui, id );
            strcpy( path, GetDriveTextList()[sel] );
            path[2] = 0;
            goToDir( gui, path );
            if( !initDialog( gui, NULL, NULL ) ) {
                dlg->dialogRC = OFN_RC_RUNTIME_ERROR;
                GUICloseDialog( gui );
            }
            break;
        case CTL_FILE_TYPES:
            sel = GUIGetCurrSelect( gui, id );
            if( !initDialog( gui, dlg->fileExtensions[sel], NULL ) ) {
                dlg->dialogRC = OFN_RC_RUNTIME_ERROR;
                GUICloseDialog( gui );
            }
            break;
        }
        return( true );
    default:
        break;  // makes GCC happy.
    }
    return( false );

} /* GetFileNameEvent */

/*
 * GUIGetFileName - get a file name from the user
 */
int GUIGetFileName( gui_window *gui, open_file_name *ofn )
{
    char        olddir[_MAX_PATH];
    dlg_info    dlg;
    bool        ok;

    if( !ControlsInitialized ) {
        InitDlgControls();
        ControlsInitialized = true;
    }
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    if( !buildDriveList() ) {
        return( OFN_RC_FAILED_TO_INITIALIZE );
    }
#endif
    ok = buildFileTypesExts( &dlg, ofn->filter_list );
    if( !ok ) {
        dlg.currExt = NULL;
        dlg.dialogRC = OFN_RC_FAILED_TO_INITIALIZE;
    } else {
        dlg.currOFN = ofn;
        dlg.currExt = NULL;
        dlg.currExtIndex = ofn->filter_index;
        dlg.dialogRC = OFN_RC_NO_FILE_SELECTED;

        getcwd( olddir, sizeof( olddir ) );
        goToDir( gui, ofn->initial_dir );

        GUIModalDlgOpen( gui, ofn->title, DLG_FILE_ROWS, DLG_FILE_COLS,
                    dlgControls, NUM_CONTROLS, &GetFileNameEvent, &dlg );

        if( !(ofn->flags & OFN_CHANGEDIR) ) {
            goToDir( gui, olddir );
        }
    }
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    freeDriveTextList();
#endif
    freeFileTypesTextList();
    freeFileExtsTextList();
    GUIMemFree( dlg.currExt );
    return( dlg.dialogRC );

} /* GUIGetFileName */

void GUIHookFileDlg( bool hook )
{
    hook = hook;
}
