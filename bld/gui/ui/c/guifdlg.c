/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
    #include <direct.h>
  #if defined( __NETWARE__ )
    #include <fnmatch.h>
  #elif defined( __RDOS__ )
  #else
    #include <dos.h>
  #endif
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

#define DLG_FILE_ROWS   14
#define DLG_FILE_COLS   65
#define BOX_WIDTH       20
#define BOX_WIDTH2      25

#define DIR_START       27

#define DLGFILE_CTLS() \
    pick_p3(   FILENAME_TITLE,    DLG_STRING,     2,                            0,  11 ) \
    pick_p3(   DIRECTORIES_TITLE, DLG_STRING,     DIR_START,                    0,  DIR_START + 12 ) \
    pick_p3id( EDIT,              DLG_EDIT,       2,                            1,  BOX_WIDTH + 3 ) \
    pick_p3id( DIR_NAME,          DLG_DYNSTRING,  DIR_START,                    1,  DLG_FILE_COLS - 1 ) \
    pick_p4id( FILE_LIST,         DLG_LIST_BOX,   2,                            3,  2 + BOX_WIDTH,                9 ) \
    pick_p4id( DIR_LIST,          DLG_LIST_BOX,   DIR_START,                    3,  DIR_START + BOX_WIDTH2,       9 ) \
    pick_p3id( OK,                DLG_DEFBUTTON,  (DIR_START + BOX_WIDTH2 + 4), 4, (DIR_START + BOX_WIDTH2 + 14) ) \
    pick_p3id( CANCEL,            DLG_BUTTON,     (DIR_START + BOX_WIDTH2 + 4), 6, (DIR_START + BOX_WIDTH2 + 14) ) \
    pick_p3(   FILE_TYPES_TITLE,  DLG_STRING,     2,                            11, 20 ) \
    pick_p4id( FILE_TYPES,        DLG_COMBO_BOX,  2,                            12, 2 + BOX_WIDTH + 3,            15 )

#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
#define DLGFILE_DRV_CTLS() \
    pick_p3(   DRIVES_TITLE,      DLG_STRING,     2,                            11, 20 ) \
    pick_p4id( DRIVES,            DLG_COMBO_BOX,  2,                            12, 2 + BOX_WIDTH + 3,            15 )
#endif

enum {
    DUMMY_ID = 100,
    #define pick_p3(id,m,p1,p2,p3)      CTL_ ## id,
    #define pick_p3id(id,m,p1,p2,p3)    CTL_ ## id,
    #define pick_p4id(id,m,p1,p2,p3,p4) CTL_ ## id,
    DLGFILE_CTLS()
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    DLGFILE_DRV_CTLS()
#endif
    #undef pick_p4id
    #undef pick_p3id
    #undef pick_p3
};

enum {
    #define pick_p3(id,m,p1,p2,p3)      id ## _IDX,
    #define pick_p3id(id,m,p1,p2,p3)    id ## _IDX,
    #define pick_p4id(id,m,p1,p2,p3,p4) id ## _IDX,
    DLGFILE_CTLS()
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    DLGFILE_DRV_CTLS()
#endif
    #undef pick_p4id
    #undef pick_p3id
    #undef pick_p3
};

// if dlgControls is modified then make sure the function InitDlgControls
// matches it
static gui_control_info dlgControls[] =
{
    #define pick_p3(id,m,p1,p2,p3)      m(NULL,p1,p2,p3),
    #define pick_p3id(id,m,p1,p2,p3)    m(NULL,CTL_ ## id,p1,p2,p3),
    #define pick_p4id(id,m,p1,p2,p3,p4) m(NULL,CTL_ ## id,p1,p2,p3,p4),
    DLGFILE_CTLS()
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    DLGFILE_DRV_CTLS()
#endif
    #undef pick_p4id
    #undef pick_p3id
    #undef pick_p3
};

static bool     ControlsInitialized = false;

#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
#define GetDriveTextList()      ((const char **)dlgControls[DRIVES_IDX].text)
#define SetDriveTextList(x)     dlgControls[DRIVES_IDX].text = ((const char *)(x))
#define freeDriveTextList( )    freeStringList((const char ***)&dlgControls[DRIVES_IDX].text)
#endif
#define GetFileTypesTextList()  ((const char **)dlgControls[FILE_TYPES_IDX].text)
#define SetFileTypesTextList(x) dlgControls[FILE_TYPES_IDX].text = ((const char *)(x))
#define freeFileTypesTextList() freeStringList((const char ***)&dlgControls[FILE_TYPES_IDX].text)
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
    dlgControls[FILENAME_TITLE_IDX].text = LIT( File_Name_Colon );
    dlgControls[DIRECTORIES_TITLE_IDX].text = LIT( Directories_Colon );
    dlgControls[EDIT_IDX].text = LIT( Empty );
    dlgControls[DIR_NAME_IDX].text = LIT( Empty );
    dlgControls[OK_IDX].text = LIT( OK );
    dlgControls[CANCEL_IDX].text = LIT( Cancel );
    dlgControls[FILE_TYPES_TITLE_IDX].text = LIT( List_Files_of_Type_Colon );
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    dlgControls[DRIVES_TITLE_IDX].text = LIT( Drives_Colon );
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
static void splitPath( char *path, char *drive, char *dir, char *fname, char *ext )
{
    char        *dotp;
    char        *fnamep;
    char        *startp;
    char        ch;

#if defined( __UNIX__ ) || defined( __NETWARE__ )
    /* process node/drive specification */
    startp = path;
    if( path[0] == FILE_SEP_CHAR && path[1] == FILE_SEP_CHAR ) {
        for( path += 2; (ch = *path) != '\0'; path++ ) {
            if( ch == FILE_SEP_CHAR || ch == '.' ) {
                break;
            }
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

    for( ; (ch = *path) != '\0'; path++ ) {
        if( ch == '.' ) {
            dotp = path;
            continue;
        }
#if defined( __UNIX__ ) || defined( __NETWARE__ )
        if( ch == FILE_SEP_CHAR ) {
#else
        if( ch == FILE_SEP_CHAR  ||  ch == '/' ) {
#endif
            fnamep = path + 1;
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
        "mov  ax,4408h"     \
        "int 21h"           \
        "cmp  ax,0fh"       \
        "jne short ok"      \
        "xor  ax,ax"        \
        "jmp short done"    \
    "ok: inc  ax"           \
    "done:"                 \
    __parm      [__bl] \
    __value     [__ax] \
    __modify    []

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
static bool addToList( const char ***list, int num_items, const char *data, size_t len )
{
    char    *str;

    *list = (const char **)GUIMemRealloc( (void *)*list, ( num_items + 2 ) * sizeof( char * ) );
    if( *list == NULL ) {
        return( false );
    }
    ++len;
    str = GUIMemAlloc( len );
    if( str == NULL ) {
        return( false );
    }
    memcpy( str, data, len );
    (*list)[num_items] = str;
    (*list)[num_items + 1] = NULL;
    return( true );

} /* addToList */

/*
 * freeStringList - release an array of strings
 */
static void freeStringList( const char ***list )
{
    int         item;

    if( *list == NULL ) {
        return;
    }
    for( item = 0; (*list)[item] != NULL; item++ ) {
        GUIMemFree( (void *)(*list)[item] );
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
    int         num_items;
    char        str[_MAX_PATH];
    const char  **list;

    num_items = 0;
    list = NULL;
    for( drv = 'A'; drv <= 'Z'; drv++ ) {
        if( getDriveType( drv ) != DRIVE_NONE ) {
            str[0] = drv;
            str[1] = ':';
            str[2] = '\0';
            if( !addToList( &list, num_items, str, 2 ) ) {
                freeStringList( &list );
                break;
            }
            num_items++;
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
    int         num_items;
    bool        ok;

    list1 = NULL;
    list2 = NULL;
    if( data != NULL ) {
        num_items = 0;
        ok = true;
        for( ; *data != '\0'; data += len + 1 ) {
            len = strlen( data );
            ok = addToList( &list1, num_items, data, len );
            if( !ok ) {
                break;
            }
            data += len + 1;
            len = strlen( data );
            ok = addToList( &list2, num_items, data, len );
            if( !ok ) {
                break;
            }
            num_items++;
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
    bool        removed_end;
    size_t      len;
    int         rc;

    /* unused parameters */ (void)gui;

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
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        _chdrive( toupper( drive[0] ) - 'A' + 1 );
#endif
    }
    return( true );

} /* goToDir */

/*
 * Compare - quicksort comparison with special treatment of indent chars
 */
/* int Compare( const char **p1, const char **p2 ) */
static int Compare( const void  *p1, const void *p2 )
{
    const char  *s1 = *(const char **)p1;
    const char  *s2 = *(const char **)p2;

    /* skip matching indents */
    while( *s1 == INDENT_CHAR && *s2 == INDENT_CHAR ) {
        ++s1;
        ++s2;
    }
    /* indents compare as chars with infinitely high value */
    if( *s1 == INDENT_CHAR ) {
        return( 1 );
    } else if( *s2 == INDENT_CHAR ) {
        return( -1 );
    }
    /* use regular string comparison for the rest */
    return( strcasecmp( s1, s2 ) );

} /* Compare */

#if defined( __QNX__ )
static void getstatus( struct dirent *dent, char *path )
{
    char        fullname[_MAX_PATH];

    if( (dent->d_stat.st_status & _FILE_USED) == 0 ) {
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
    return( (dent->d_stat.st_mode & bit) == 0 );
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
    int                 num_items;
    char                ext1[_MAX_PATH];
    int                 item;
    dlg_info            *dlg = GUIGetExtra( gui );
    bool                ok;

    num_items = 0;
    list = NULL;
    strcpy( ext1, ext );
    ok = true;
    for( ptr = strtok( ext1, ";" ); ptr != NULL; ptr = strtok( NULL, ";" ) ) {

        if( getcwd( path, sizeof( path ) ) == NULL ) {
            break;
        }

#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        if( path[strlen( path ) - 1] != FILE_SEP_CHAR ) {
            strcat( path, FILE_SEP );
        }
        strcat( path, ptr );
#endif

        directory = opendir( path );
        if( directory != NULL ) {
            while( (dent = readdir( directory )) != NULL ) {
                if( !isdir( dent, path ) ) {
                    if( (dlg->currOFN->flags & FN_HIDEREADONLY) && isrdonly( dent, path ) ) {
                        continue;
                    }
#if defined( __UNIX__ ) || defined( __NETWARE__ )
                    if( fnmatch( ptr, dent->d_name, FNM_PATHNAME ) != 0 ) {
                        continue;
                    }
#endif
                    if( !addToList( &list, num_items, dent->d_name, strlen( dent->d_name ) ) ) {
                        ok = false;
                        break;
                    }
                    num_items++;
                }
            }
            closedir( directory );
        }
    }
    GUIClearList( gui, CTL_FILE_LIST );
    if( num_items > 0 ) {
        if( ok ) {
            qsort( (void *)list, num_items, sizeof( char * ), Compare );
            for( item = 0; item < num_items; item++ ) {
                GUIAddText( gui, CTL_FILE_LIST, list[item] );
            }
        }
        freeStringList( &list );
    }
    return( ok );

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
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    const char          **drvlist;
#endif
    int                 item;
    size_t              len;
    int                 selected_item;
    int                 num_items;
    const char          **list;
    bool                ok;

    GUIClearList( gui, CTL_DIR_LIST );
    num_items = 0;
    list = NULL;
    ok = true;
    if( getcwd( path, sizeof( path ) ) == NULL ) {
        return( ok );
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
    drive[0] = OPENED_DIR_CHAR;
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    drvlist = GetDriveTextList();
    for( item = 0; drvlist[item] != NULL; item++ ) {
        if( drvlist[item][0] == drive[1] ) {
            GUISetCurrSelect( gui, CTL_DRIVES, item );
            break;
        }
    }
    drive[3] = '\\';
    drive[4] = '\0';
#endif
    ok = false;
    if( addToList( &list, num_items, drive, strlen( drive ) ) ) {
        num_items++;
        ok = true;
        strcpy( indent, INDENT_STR );
        start = dir + 1;
        for( ptr = start; *ptr != '\0'; ptr++ ) {
            if( *ptr == FILE_SEP_CHAR ) {
                *ptr = '\0';
                len = strlen( indent );
                memcpy( tmp, indent, len );
                tmp[len++] = OPENED_DIR_CHAR;
                strcpy( tmp + len, start );
                if( !addToList( &list, num_items, tmp, strlen( tmp ) ) ) {
                    ok = false;
                    break;
                }
                num_items++;
                start = ptr + 1;
                strcat( indent, INDENT_STR );
            }
        }
        if( ok ) {
            selected_item = num_items;
            directory = opendir( path );
            if( directory == NULL ) {
                ok = false;
            } else {
                while( (dent = readdir( directory )) != NULL ) {
                    if( isdir( dent, path ) ) {
                        if( ( dent->d_name[0] == '.' ) && ( ( dent->d_name[1] == 0 )
                          || ( dent->d_name[1] == '.' && dent->d_name[2] == 0 ) ) ) {
                            continue;
                        }
                        len = strlen( indent );
                        memcpy( tmp, indent, len );
                        tmp[len++] = UNOPENED_DIR_CHAR;
                        strcpy( tmp + len, dent->d_name );
                        if( !addToList( &list, num_items, tmp, strlen( tmp ) ) ) {
                            ok = false;
                            break;
                        }
                        num_items++;
                    }
                }
                closedir( directory );
                if( ok ) {
                    qsort( (void *)list, num_items, sizeof( char * ), Compare );
                    for( item = 0; item < num_items; item++ ) {
                        GUIAddText( gui, CTL_DIR_LIST, list[item] );
                    }
                    GUISetCurrSelect( gui, CTL_DIR_LIST, selected_item - 1 );
                }
            }
        }
    }
    if( num_items > 0 ) {
        freeStringList( &list );
    }
    return( ok );

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
        if( !rc && (dlg->currOFN->flags & FN_OVERWRITEPROMPT) ) {
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
static void ProcessOKorDClick( gui_window *gui, gui_ctl_id id  )
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
            dlg->dialogRC = FN_RC_FILE_SELECTED;
            GUICloseDialog( gui );
        } else if( prc == PROCESS_FAIL ) {
            dlg->dialogRC = FN_RC_RUNTIME_ERROR;
            GUICloseDialog( gui );
        }
        break;
    case CTL_DIR_LIST :
        sel = -1;
        GUIGetCurrSelect( gui, id, &sel );
#if defined( __UNIX__ ) || defined( __NETWARE__ )
        path[0] = FILE_SEP_CHAR;
        path[1] = 0;
#else
        path[0] = 0;
#endif
        realsel = 0;
        for( i = 0; i < sel; i++ ) {
            ptr = GUIGetListItem( gui, id, i );
            if( ptr == NULL ) {
                return;
            }
            optr = ptr;
            while( *ptr == INDENT_CHAR ) {
                ptr++;
            }
            if( *ptr == '-' ) {
                strcat( path, ptr + 1 );
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
        if( ptr != NULL ) {
            optr = ptr;
            while( *ptr == INDENT_CHAR ) {
                ptr++;
            }
            strcat( path, ptr+1 );
            GUIMemFree( optr );
            goToDir( gui, path );
            if( !initDialog( gui, NULL, NULL ) ) {
                dlg->dialogRC = FN_RC_RUNTIME_ERROR;
                GUICloseDialog( gui );
            } else {
                GUISetCurrSelect( gui, id, realsel );
            }
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
 * GetFileNameGUIEventProc - event handler for GetFileName dialog
 */
static bool GetFileNameGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    int         sel;
    char        *ptr;
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
    char        path[_MAX_PATH];
#endif
    dlg_info    *dlg = GUIGetExtra( gui );

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        dlg->initted = false;
        InitTextList( gui, CTL_FILE_TYPES, GetFileTypesTextList() );
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        InitTextList( gui, CTL_DRIVES, GetDriveTextList() );
#endif
        if( !initDialog( gui, dlg->fileExtensions[dlg->currExtIndex], dlg->currOFN->file_name ) ) {
            dlg->dialogRC = FN_RC_FAILED_TO_INITIALIZE;
            break;
        }
        dlg->initted = true;
        GUISetFocus( gui, CTL_EDIT );
        return( true );
    case GUI_CONTROL_DCLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_FILE_LIST:
        case CTL_DIR_LIST:
            ProcessOKorDClick( gui, id );
            return( true );
        }
        break;
    case GUI_CONTROL_CLICKED:
        if( !dlg->initted )
            break;
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_OK:
            ProcessOKorDClick( gui, id );
            return( true );
        case CTL_CANCEL:
            GUICloseDialog( gui );
            return( true );
        case CTL_FILE_LIST:
            ptr = GUIGetText( gui, id );
            GUISetText( gui, CTL_EDIT, ptr );
            GUIMemFree( ptr );
            return( true );
#if !defined( __UNIX__ ) && !defined( __NETWARE__ )
        case CTL_DRIVES :
            sel = -1;
            GUIGetCurrSelect( gui, id, &sel );
            strcpy( path, GetDriveTextList()[sel] );
            path[2] = 0;
            goToDir( gui, path );
            if( !initDialog( gui, NULL, NULL ) ) {
                dlg->dialogRC = FN_RC_RUNTIME_ERROR;
                GUICloseDialog( gui );
            }
            return( true );
#endif
        case CTL_FILE_TYPES:
            sel = -1;
            GUIGetCurrSelect( gui, id, &sel );
            if( !initDialog( gui, dlg->fileExtensions[sel], NULL ) ) {
                dlg->dialogRC = FN_RC_RUNTIME_ERROR;
                GUICloseDialog( gui );
            }
            return( true );
        }
        break;
    default:
        break;  // makes GCC happy.
    }
    return( false );

} /* GetFileNameGUIEventProc */

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
        return( FN_RC_FAILED_TO_INITIALIZE );
    }
#endif
    ok = buildFileTypesExts( &dlg, ofn->filter_list );
    if( !ok ) {
        dlg.currExt = NULL;
        dlg.dialogRC = FN_RC_FAILED_TO_INITIALIZE;
    } else {
        dlg.currOFN = ofn;
        dlg.currExt = NULL;
        dlg.currExtIndex = ofn->filter_index;
        dlg.dialogRC = FN_RC_NO_FILE_SELECTED;

        getcwd( olddir, sizeof( olddir ) );
        goToDir( gui, ofn->initial_dir );

        GUIModalDlgOpen( gui, ofn->title, DLG_FILE_ROWS, DLG_FILE_COLS,
                    dlgControls, ARRAY_SIZE( dlgControls ), &GetFileNameGUIEventProc, &dlg );

        if( (ofn->flags & FN_CHANGEDIR) == 0 ) {
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
    /* unused parameters */ (void)hook;
}
