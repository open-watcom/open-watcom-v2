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


#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>
#ifdef __QNX__
#include <dirent.h>
#else
#include "direct.h"
#endif
#include "stdui.h"
#include "dirui.h"
#include "uivedit.h"
#include "iopath.h"

#ifndef NULL
#define NULL            0               /* NULL pointer         */
#endif
#define TRUE            (0==0)          /* TRUE                 */
#define FALSE           (0==1)          /* FALSE                */
#define bool            int             /* boolean value        */

/* N.B. DIR_MAX must be the same as UTIL_LEN in dbgdefn.h */
#define DIR_MAX         80              /* max length of path   */

/* N.B. if the value of ERROR is changed here it must also be changed
 * in dbgmenu1.c */
#define EV_ERROR        -1              /* error return value   */

#define OK_DIR           1              /* return codes for DisplayDir */
#define NO_MEM_4_DIR    -1
#define INVALID_DIR     -2

#ifdef __QNX__
#   define  IS_DIR( dir )   S_ISDIR((dir)->d_stat.st_mode)
#   define  FULL_MASK       "*"
#else
#   define  IS_DIR( dir )   (((dir)->d_attr & _A_SUBDIR)!=0)
#   define  FULL_MASK       "*.*"
#endif

struct name {
    bool dir;
    char str[NAME_MAX + 7];
};

static struct name *Names = NULL;

static EVENT DirEvents[] = {
    EV_NO_EVENT,            /* end of list of ranges */
    EV_MOUSE_PRESS,
    EV_MOUSE_DRAG,
    EV_MOUSE_DCLICK,
    EV_MOUSE_RELEASE,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_RETURN,
    EV_ESCAPE,
    EV_NO_EVENT
};

static EVENT GetDirEvents[] = {
    EV_NO_EVENT,            /* end of list of ranges */
    EV_MOUSE_PRESS,
    EV_MOUSE_DCLICK,
    EV_CURSOR_UP,
    EV_CURSOR_DOWN,
    EV_RETURN,
    EV_ESCAPE,
    EV_NO_EVENT
};

static VEDITLINE DirEdit = {
    0, 0,                   /* first line of window */
    NAME_MAX + 7,           /* length of field      */
    0,                      /* first visible char   */
    0,                      /* buffer length        */
    NULL,                   /* character buffer     */
    0,                      /* cursor in buffer     */
    0,                      /* attribute            */
    FALSE,                  /* buffer changed       */
    TRUE,                   /* update               */
    FALSE,                  /* autoclear line       */
    FALSE                   /* invisible line       */
};

static  char            FullMask[] = FULL_MASK;


 static void outnames( wptr, dirptr, start, stop )
/*************************************************/

    register VSCREEN            *wptr;
    register DIRECTORY          *dirptr;
    register int                start;
    register int                stop;
{
    register int                index;
    register ORD                row;
    register int                attr;
    register char               *str;

    for( index = start; index <= stop; ++index ) {
        row = index - dirptr->index + dirptr->currrow;
        if( ( row > 0 ) && ( row < wptr->area.height ) ) {
            if( index == dirptr->index ) {
                attr = ATTR_EDIT;
            } else {
                attr = ATTR_NORMAL;
            }
            if( index < dirptr->dirsize ) {
                str = Names[index].str;
            } else {
                str = "";
            }
            uivtextput( wptr, row, 0, UIData->attrs[attr], str,
                        wptr->area.width );
        }
    }
}

#ifdef __QNX__
static void add_name( DIRECTORY *direct, char *name, char **where )
{
    char    *add_point;

    add_point = &direct->pathbuff[strlen( direct->pathbuff )];
    if( where != NULL )
        *where = add_point;
    if( add_point > direct->pathbuff && add_point[-1] != '/' ) {
        *add_point++ = '/';
    }
    strcpy( add_point, name );
}
#endif

static struct dirent *my_readdir( DIRECTORY *direct, DIR *dp )
{
    struct dirent   *dir;

    direct = direct;
    dir = readdir( dp );
    if( dir == NULL ) return( NULL );
#ifdef __QNX__
    if( !(dir->d_stat.st_status & _FILE_USED) ) {
        char            *where;

        add_name( direct, dir->d_name, &where );
        stat( direct->pathbuff, &dir->d_stat );
        *where = '\0';
    }
#endif
    return( dir );
}


#ifdef __QNX__
/*  checks name for match with wildcard mask */
/*  currently only supports masks of type *.ext */
static bool checkMask( char *name, char *mask )
{
    if( mask == NULL)
        return( TRUE );
    if( mask[0] == '*' && mask[1] == '.' &&
                ( strcspn( &mask[2], "*?" ) == strlen( &mask[2] ) ) ) {
        mask++;
        if( stricmp( mask, name + strlen( name ) - strlen( mask ) ) == 0 ) {
            return( TRUE );     /* wildcard matches */
        } else {
            return( FALSE );    /* wildcard doesn't match */
        }
    } else {
        return( TRUE );         /* mask type is unsupported */
    }
}
#endif


 static int addnames( dirptr, mask, flag, index )
/************************************************/

    register DIRECTORY          *dirptr;
    register char               *mask;
    register int                flag;
    register int                index;
{
    register size_t             len;
#ifndef __QNX__
    register char               *str;
#endif
    register DIR                *dptr;
    register int                add_mask;
    struct   dirent             *direntp;

    add_mask = concat( dirptr, mask );
    dptr = opendir( dirptr->pathbuff );
    if( dptr != NULL ) {
        for( ;; ) {
#ifdef __QNX__
/* scan for next directory matching mask */
            do {
                direntp = my_readdir( dirptr, dptr );
            } while( direntp != NULL && !IS_DIR( direntp )
                               && !checkMask( direntp->d_name, mask ) );
#else
            direntp = my_readdir( dirptr, dptr );
#endif
            if( direntp == NULL )
                break;
            if( IS_DIR( direntp ) == flag ) {
                Names[index].dir = flag;
                len = 0;
#ifdef __QNX__
#define NAME_START 6
                strcpy( &Names[index].str[0], flag ? "<DIR> " : "      " );
                strcpy( &Names[index].str[NAME_START], direntp->d_name );
#else
#define NAME_START 0
                str = direntp->d_name;
                while( *str != '\0' ) {
                    if( ( *str == '.' ) && ( *( str - len ) != '.' ) ) {
                        ++str;
                        break;
                    }
                    Names[index].str[len] = *str;
                    ++str;
                    ++len;
                }
                while( len < 9 ) {
                    Names[index].str[len] = ' ';
                    ++len;
                }
                strcpy( Names[index].str + len, str );
                len = strlen( Names[index].str );
                while( len < 14 ) {
                    Names[index].str[len] = ' ';
                    ++len;
                }
                Names[index].str[len] = '\0';
                if( Names[index].dir ) {
                    memcpy( Names[index].str + 13, "<DIR>", 6 );
                }
#endif
                ++index;
            }
        }
        closedir( dptr );
    }
    if( add_mask ) {
        strip( dirptr, NULL );
    }
    return( index );
}


 static makedir( char *buff )
/***************************/
{
#ifdef __QNX__
    buff = buff;
#else
    size_t  len;

    len = strlen( buff );
    buff += len;
    if( len > 0 && !IS_PATH_SEP( buff[-1] ) ) {
        *buff++ = DIR_SEP;
        *buff = '\0';
    }
#endif
}


 static int outdir( VSCREEN *wptr, char *buff )
/*********************************************/
{
    size_t      len;
    int         width;

    len = strlen( buff );
    width = wptr->area.width;
    if( len >= width ) {
        len -= width - 1;
    } else {
        len = 0;
    }
    buff += len;
    uivtextput( wptr, 0, 0, UIData->attrs[ATTR_BRIGHT], buff, width );
    return( len );
}


 static int concat( DIRECTORY *dirptr, char *str )
/************************************************/
{
#ifdef __QNX__
    dirptr = dirptr;
    str = str;
#else
    register char               *buf;

    buf = dirptr->pathbuff;
    while( ( *buf != ' ' ) && ( *buf != '\0' ) ) {
        ++buf;
    }
    if( ( buf == dirptr->pathbuff ) || IS_PATH_SEP( buf[-1] ) ) {
        if( *str != '.' ) {
            for( ;; ) {
                if( *str == ' ' ) {
                    while( *str == ' ' ) {
                        ++str;
                    }
                    if( ( *str != '<' ) && ( *str != '\0' ) ){
                        *buf++ = '.';
                    } else {
                        break;
                    }
                }
                *buf = *str;
                if( *buf == '\0' ) {
                    break;
                }
                ++buf;
                ++str;
            }
        }
    } else {
        return( FALSE );
    }
    *buf = '\0';
#endif
    return( TRUE );
}


 static int strip( DIRECTORY *dirptr, char *mask )
/************************************************/
{
#ifdef __QNX__
    dirptr = dirptr;
    mask = mask;
#else
    register char               *buf;

    buf = dirptr->pathbuff + strlen( dirptr->pathbuff ) - 1;
    while( buf >= dirptr->pathbuff ) {
        --buf;
        if( ( buf < dirptr->pathbuff ) || IS_PATH_SEP( *buf ) ) {
            if( mask != NULL ) {
                strcpy( mask, buf + 1 );
            }
            *( buf + 1 ) = '\0';
            return( TRUE );
        }
    }
    makedir( buf );
#endif
    return( FALSE );
}


 static int dircount( dirptr, mask, flag )
/*****************************************/

    register DIRECTORY          *dirptr;
    register char               *mask;
    register int                flag;
{
    register unsigned           count;
    register DIR                *dptr;
    register int                add_mask;
    struct   dirent             *direntp;

    count = 0;
    if( mask != NULL ) {
        add_mask = concat( dirptr, mask );
    } else {
        add_mask = FALSE;
    }
    dptr = opendir( dirptr->pathbuff );
    if( dptr == NULL ) {
        count = INVALID_DIR;
    } else {
        for( ;; ) {
#ifdef __QNX__
/* scan for next directory matching mask */
            do {
                direntp = my_readdir( dirptr, dptr );
            } while( direntp != NULL && !IS_DIR( direntp )
                               && !checkMask( direntp->d_name, mask ) );
#else
            direntp = my_readdir( dirptr, dptr );
#endif
            if( direntp == NULL ) break;
            if( IS_DIR( direntp ) == flag ) ++count;
        }
        closedir( dptr );
    }
    if( add_mask ) {
        strip( dirptr, NULL );
    }
    return( count );
}

 static void InvalidDir( DIRECTORY *dirptr, VSCREEN *wptr )
/**********************************************************/
{
    unsigned    i;
    int         row;

    DirEdit.scroll = 0;
    uivtextput( wptr, 1, 0, UIData->attrs[ATTR_NORMAL],
        "Invalid directory", wptr->area.width );
    for( i = 1; i <= wptr->area.height-1; i ++ ) {
        row = i - dirptr->index + dirptr->currrow;
        if( ( row > 0 ) && ( row < wptr->area.height ) ) {
            uivtextput( wptr, row, 0, UIData->attrs[ATTR_NORMAL], "",
                        wptr->area.width );
        }
    }
}


 static int displaydir( wptr, dirptr )
/*************************************/

    register VSCREEN            *wptr;
    register DIRECTORY          *dirptr;
{
    register int                index;
    register char               *mask;
    auto     char               buff[DIR_MAX + 1];
             void               *tmp;
             int                ret;
    struct   stat               blk;

    Names = NULL;
    index = 0;
    mask = dirptr->mask;
    ret = stat( dirptr->pathbuff, &blk );
    if( ( ret == 0 ) && ( S_ISDIR( blk.st_mode ) ) ){
        makedir( dirptr->pathbuff );
    }
    if( concat( dirptr, FullMask ) == FALSE ) {
        index = dircount( dirptr, NULL, TRUE );
        if( index != INVALID_DIR ) {
            strip( dirptr, buff );
            mask = buff;
        } else {
            strip( dirptr, buff );
            mask = buff;
        }
    } else {
        strip( dirptr, NULL );
    }
    index = dircount( dirptr, FullMask, TRUE );
    if( index == INVALID_DIR ) {
        InvalidDir( dirptr, wptr );
        return( INVALID_DIR );
    }
    ret = dircount( dirptr, mask, 0 );
    if( ret != INVALID_DIR ) {
        index += ret;
    }
    dirptr->index = 0;
    dirptr->currrow = 1;
    if( index == 0 ) {
        dirptr->dirsize = 1;
    } else {
        dirptr->dirsize = index;
    }
    tmp = (void *)malloc( dirptr->dirsize * sizeof( struct name ) );
    if( tmp != NULL ) {
        Names = tmp;
        if( index == 0 ) {
            strcpy( Names[index].str, "." );
        } else {
            index = addnames( dirptr, FullMask, TRUE, 0 );
            addnames( dirptr, mask, 0, index );
        }
        concat( dirptr, mask );
        outnames( wptr, dirptr, 0, wptr->area.height - 1 );
        DirEdit.scroll = outdir( wptr, dirptr->pathbuff );
        DirEdit.update = TRUE;
        DirEdit.index = strlen( dirptr->pathbuff );
        return( OK_DIR );
    } else {
        return( NO_MEM_4_DIR );
    }
}


 int initdir( wptr, dirptr )
/***************************/

    register VSCREEN            *wptr;
    register DIRECTORY          *dirptr;
{
    int                         ret;

    makedir( dirptr->pathbuff );
    ret = displaydir( wptr, dirptr );
    if( ret == NO_MEM_4_DIR ) {
        return( FALSE );
    } else {
        return( TRUE );
    }
}


 static int getdir( wptr, dirptr, new_dir )
/*********************************/

    register VSCREEN            *wptr;
    register DIRECTORY          *dirptr;
             bool               *new_dir;
{
    register char               *str;
    register int                len;
    register EVENT              ev;
    register EVENT              new;
    register ATTR               attr;
    auto     char               olddir[DIR_MAX + 1];
             int                ret;

    strcpy( olddir, dirptr->pathbuff );
    DirEdit.attr = UIData->attrs[ATTR_BRIGHT];
    DirEdit.fldlen = wptr->area.width;
    DirEdit.length = dirptr->pathbufflen;
    DirEdit.buffer = dirptr->pathbuff;
    DirEdit.dirty = TRUE;
    uipushlist( GetDirEvents );
    dirptr->pathbuff[DirEdit.length] = '\0';
    attr = UIData->attrs[ATTR_NORMAL];
    new = EV_NO_EVENT;
    DirEdit.update = TRUE;
    *new_dir = FALSE;
    do {
        ev = uiveditline( wptr, &DirEdit );
        switch( ev ) {
        case EV_ESCAPE:
            new = ev;
            break;
        case EV_RETURN:
            finidir( wptr, dirptr );
            len = 0;
            while( dirptr->pathbuff[len] != ' '
                        && dirptr->pathbuff[len] != '\0' ) {
                ++len;
            }
            dirptr->pathbuff[len] = '\0';
            ret = displaydir( wptr, dirptr );
            switch( ret ) {
            case NO_MEM_4_DIR:
                new = EV_ERROR;
                break;
            case OK_DIR:
                strcpy( olddir, dirptr->pathbuff );
                str = Names[0].str;
                uivtextput( wptr, 1, 0, attr, str, wptr->area.width );
                *new_dir = TRUE;
                break;
            case INVALID_DIR:
                *new_dir = TRUE;
            }
            break;
        case EV_CURSOR_DOWN:
            strcpy( dirptr->pathbuff, olddir );
            outdir( wptr, dirptr->pathbuff );
            if( *new_dir ){
                finidir( wptr, dirptr );
                displaydir( wptr, dirptr );
            }
            new = ev;
            break;
        }
    } while( new == EV_NO_EVENT );
    uipoplist();
    wptr->cursor = C_OFF;
    return( new );
}

#define NO_ROW  ((ORD)-1)

 EVENT directory( wptr, dirptr )
/*******************************/

    register VSCREEN            *wptr;
    register DIRECTORY          *dirptr;
{
    register EVENT              ev;
    register EVENT              new;
    SAREA                       area;
    register int                index;
    ORD                         row, col;
    bool                        got_new_dir;

    uipushlist( DirEvents );
    ev = uivgetevent( wptr );
    uipoplist();
    area.row = 0;
    area.col = 0;
    area.height = 1;
    area.width = wptr->area.width;
    new = EV_NO_EVENT;
    index = dirptr->index;
    got_new_dir = FALSE;
    switch( ev ) {

    case EV_CURSOR_UP :
        if( index > 0 ) {
            dirptr->index -= 1;
            if( dirptr->currrow == 1 ) {
                area.row = 1;
            }
        } else {
            /* turn off highlighting while getting new directory */
            dirptr->index ++;
            dirptr->currrow ++;
            outnames( wptr, dirptr, index, index );
            dirptr->index --;
            dirptr->currrow --;
            new = getdir( wptr, dirptr, &got_new_dir );
            if( !got_new_dir ) {
                outnames( wptr, dirptr, index, index );
            }
        }
        break;

    case EV_CURSOR_DOWN :
        if( index < dirptr->dirsize - 1 ) {
            dirptr->index += 1;
            if( dirptr->currrow == wptr->area.height - 1 ) {
                area.row = 2;
            }
        }
        break;

    case EV_PAGE_UP:
        if( index - ( wptr->area.height - 1 ) >= 0 ) {
            dirptr->index -= wptr->area.height - 1;
        } else {
            dirptr->index = 0;
        }
        dirptr->currrow = 1;
        area.row = NO_ROW;
        break;

    case EV_PAGE_DOWN:
        if( index + wptr->area.height - 1 <= dirptr->dirsize - 1 ) {
            dirptr->index += wptr->area.height - 1;
        } else {
            dirptr->index = dirptr->dirsize - 1;
        }
        dirptr->currrow = wptr->area.height - 1;
        if( dirptr->currrow > dirptr->dirsize ) {
            dirptr->currrow = dirptr->dirsize;
        }
        area.row = NO_ROW;
        break;

    case EV_MOUSE_PRESS:
        if( wptr != uivmousepos( wptr, &row, &col ) ) {
            new = EV_ESCAPE;
            break;
        }
    case EV_MOUSE_DCLICK:
    case EV_MOUSE_DRAG:
        if( wptr == uivmousepos( wptr, &row, &col ) ){
            dirptr->index += row - dirptr->currrow;
        }
        if( dirptr->index >= dirptr->dirsize )
            dirptr->index = dirptr->dirsize - 1;
        else if( dirptr->index < 0 ) {
            dirptr->index = 0;
        }
        break;

    case EV_MOUSE_RELEASE:
        if( wptr != uivmousepos( wptr, &row, &col ) ){
            break;
        } else if( row == 0 ) {
            /* turn off highlighting while getting new directory */
            dirptr->index ++;
            dirptr->currrow ++;
            outnames( wptr, dirptr, index, index );
            dirptr->index --;
            dirptr->currrow --;
            new = getdir( wptr, dirptr, &got_new_dir );
            if( ! got_new_dir ) {
                dirptr->currrow = 1;
                outnames( wptr, dirptr, index, index );
            }
            break;
        }
        /* fall through */
        ev = EV_RETURN;
    case EV_RETURN :
        if( Names[index].dir ) {
            if( Names[index].str[NAME_START] == '.' ) {
                if( Names[index].str[NAME_START+1] == '.' ) {
#ifdef __QNX__
                    char    *start;
                    char    *end;
                    char    chr;

                    start = dirptr->pathbuff;
                    if( start[0] == '/' ) {
                        ++start;
                        if( start[0] == '/' ) {
                            ++start;
                            for( ;; ) {
                                chr = *start;
                                if( chr == '\0' ) break;
                                ++start;
                                if( chr == '/' ) break;
                            }
                        }
                    }
                    end = start + strlen( start );
                    while( end > start && *end != '/' )
                        --end;
                    *end = '\0';
                    finidir( wptr, dirptr );
                    if( displaydir( wptr, dirptr ) == NO_MEM_4_DIR ) {
                        return( EV_ERROR );
                    }
                    index = dirptr->index;
#else
                    //NYI: what to do for QNX?
                    strip( dirptr, NULL );
                    if( strip( dirptr, NULL ) ) {
                        finidir( wptr, dirptr );  /* frees up Names */
                        if( displaydir( wptr, dirptr ) == NO_MEM_4_DIR ) {
                            return( EV_ERROR );
                        }
                        index = dirptr->index;
                    } else {
                        new = ev;
                    }
#endif
                 }
            } else {
#ifdef __QNX__
                add_name( dirptr, &Names[dirptr->index].str[NAME_START], NULL );
#else
                strip( dirptr, NULL );
                concat( dirptr, Names[dirptr->index].str );
#endif
                finidir( wptr, dirptr );  /* frees up Names */
                if( initdir( wptr, dirptr ) == FALSE ) {
                    return( EV_ERROR );
                }
                index = dirptr->index;
            }
        } else {
#ifdef __QNX__
            add_name( dirptr, &Names[dirptr->index].str[NAME_START], NULL );
#else
            strip( dirptr, NULL );
            concat( dirptr, Names[dirptr->index].str );
#endif
            finidir( wptr, dirptr );
            new = ev;
        }
        break;

    default :
        new = ev;
        break;
    }
    if( !got_new_dir ) {
        if( index != dirptr->index ) {
            if( area.row == NO_ROW ) {
                index = dirptr->index - dirptr->currrow;
                outnames( wptr, dirptr, index, index + wptr->area.height- 1 );
            } else if( area.row > 0 ) {
                area.height = wptr->area.height - 2;
                uivmoveblock( wptr, area, index - dirptr->index, 0 );
            } else {
                dirptr->currrow += dirptr->index - index;
            }
            outnames( wptr, dirptr, index, index );
            outnames( wptr, dirptr, dirptr->index, dirptr->index );
        }
    }
    return( new );
}


#pragma off(unreferenced);
 void finidir( VSCREEN *wptr, DIRECTORY *dirptr )
/***********************************************/
#pragma on(unreferenced);
{
    if( Names != NULL ) {
        free( Names );
        Names = NULL;
    }
}
