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


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#if defined( __QNX__ )
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#else
#include <direct.h>
#include <sys/utime.h>
#endif

#include "watcom.h"
#include "banner.h"
#include "touch.h"
#include "wtmsg.h"

#if defined( __NT__ )
#undef STDOUT_FILENO
#define STDOUT_FILENO (stdout->_handle)
#else
#ifndef STDOUT_FILENO
#define STDOUT_FILENO   1
#endif
#endif

extern  time_t      _d2ttime( unsigned int date, unsigned int time );

extern touchflags   TouchFlags;
extern timestruct   TimeAdjust;
extern datestruct   DateAdjust;
extern char         timeSeparator;
extern char *       timeFormat;
extern char         dateSeparator;
extern char *       dateFormat;

static struct tm touchTime;

static struct file_list {
    struct file_list    *next;
    char                *file_spec;
} *fileList;

/* external function prototypes */

extern void DoDOption( char * );
extern void DoTOption( char * );
extern void WhereAmI( void );

extern void Error( int msgnum, char *p )
/**************************/
/* routine called by dtparse.c whenever there is an error */
{
    writeMsg( msgnum, p );
}

static void writeStr( char *p )
/*****************************/
{
    size_t len;

    len = strlen( p );
    write( STDOUT_FILENO, p, len );
}

static void writeMsg( int msgnum, char *p )
/*****************************************/
{
    char        msgbuff[MAX_RESOURCE_SIZE];

    MsgSubStr( msgnum, msgbuff, p );
    writeStr( msgbuff );
    newLine();
}

static void newLine( void )
/*************************/
{
    write( STDOUT_FILENO, "\r\n", 2 );
}

static void showDateTimeFormat( void )
/************************************/
{
    char buff[16];
    char *p;

    newLine();
    strcpy( buff, dateFormat );
    for( p = buff; *p; ++p ) {
        if( *p == DEFAULT_DATE_SEPARATOR ) {
            *p = dateSeparator;
        }
    }
    writeMsg( MSG_USAGE_DATE_FORM, buff );
    strcpy( buff, timeFormat );
    for( p = buff; *p; ++p ) {
        if( *p == DEFAULT_TIME_SEPARATOR ) {
            *p = timeSeparator;
        }
    }
    writeMsg( MSG_USAGE_TIME_FORM, buff );
    if( TouchFlags.time_24hr == 0 ) {
        writeMsg( MSG_USAGE_AM_PM_0, NULL );
        writeMsg( MSG_USAGE_AM_PM_1, NULL );
    }
}

static void usage( void )
/***********************/
{
    static char *useText[] = {
banner1w( "Touch Utility", _WTOUCH_VERSION_ ),
banner2( "1988" ),
banner3,
    NULL
    };
    char **text;
    char msgbuff[MAX_RESOURCE_SIZE];
    int  i;

    text = useText;
    while( *text ) {
        writeStr( *text++ );
        newLine();
    }
    for( i = MSG_USE_BASE;; i++ ) {
        MsgGet( i, msgbuff );
        if( ( msgbuff[0] == '.' ) && ( msgbuff[1] == 0 ) ) break;
        writeStr( msgbuff );
        newLine();
    }
    showDateTimeFormat();
    exit( EXIT_FAILURE );
}

static void doFOption( char *date_file )
/**************************************/
{
    struct stat stat_buff;
    struct tm *ptime;

    if( stat( date_file, &stat_buff ) == -1 ) {
        if( !TouchFlags.quiet ) {
            writeMsg( MSG_NO_TIME_STAMP, date_file );
        }
        return;
    }
    ptime = localtime( &stat_buff.st_mtime );
    touchTime = *ptime;
    TouchFlags.file_specified = 1;
}

static void addFileSpec( char *new_file )
/***************************************/
{
    struct file_list *new;

    new = malloc( sizeof( struct file_list ) );
    new->next = fileList;
    new->file_spec = new_file;
    fileList = new;
}

static void syncStamp( struct utimbuf *stamp )
/********************************************/
{
    time_t touch_time;

    touch_time = mktime( &touchTime );
    stamp->actime  = touch_time;
    stamp->modtime = touch_time;
}

static void incTouchTime( void )
/******************************/
{
    touchTime.tm_sec += MINIMUM_SEC_INCREMENT;
    /* adjust fields in touchTime to correct values */
    mktime( &touchTime );
}

static void incFilesOwnTime( struct dirent *dir, struct utimbuf *stamp )
/**********************************************************************/
{
    time_t      ftime;
    struct tm  *ptime;

    /* check for the case of only specifying '/i' with nothing else */
    if( ! TouchFlags.increment_time ) {
        return;
    }
    if( TouchFlags.file_specified ) {
        return;
    }
    if( TouchFlags.time_specified ) {
        return;
    }
    if( TouchFlags.date_specified ) {
        return;
    }
    /* we need to access the file's time stamp and increment it */
    #ifdef __QNX__
        ftime = dir->d_stat.st_mtime;
    #else
        ftime = _d2ttime( dir->d_date, dir->d_time );
    #endif
    ptime = localtime( &ftime );
    touchTime = *ptime;
    incTouchTime();
    syncStamp( stamp );
}

static void processOptions( int argc, char **argv )
/*************************************************/
{
    time_t      curr_time;
    struct tm   *ptime;
    char        *p;
    int         files;

    WhereAmI();
    time( &curr_time );
    ptime = localtime( &curr_time );
    touchTime = *ptime;
    TouchFlags.create_file = 1;
    TouchFlags.date_specified = 0;
    TouchFlags.time_specified = 0;
    TouchFlags.allow_read_only = 0;
    TouchFlags.quiet = 0;
    TouchFlags.usa_date_time = 0;
    TouchFlags.increment_time = 0;
    files = 0;
    while( --argc ) {
        p = *++argv;
        switch( p[0] ) {
        case '-':
    #ifndef __QNX__
        case '/':
    #endif
            if( ( isalpha( p[1] ) || p[1] == '?' ) && p[2] == '\0' ) {
                switch( tolower( p[1] ) ) {
                case '?':
                    usage();
                    break;
                case 'u':
                    TouchFlags.usa_date_time = 1;
                    WhereAmI();
                    break;
                case 'r':
                    TouchFlags.allow_read_only = 1;
                    break;
                case 'q':
                    TouchFlags.quiet = 1;
                    break;
                case 'c':
                    TouchFlags.create_file = 0;
                    break;
                case 'i':
                    TouchFlags.increment_time = 1;
                    break;
                case 'd':
                    if( ( argc - 1 ) != 0 && argv[1] != NULL ) {
                        DoDOption( argv[1] );
                        ++argv;
                        --argc;
                    } else {
                        writeMsg( MSG_NO_DATE, NULL );
                    }
                    break;
                case 'f':
                    if( ( argc - 1 ) != 0 && argv[1] != NULL ) {
                        doFOption( argv[1] );
                        ++argv;
                        --argc;
                    } else {
                        writeMsg( MSG_NO_FILE, NULL );
                    }
                    break;
                case 't':
                    if( ( argc - 1 ) != 0 && argv[1] != NULL ) {
                        DoTOption( argv[1] );
                        ++argv;
                        --argc;
                    } else {
                        writeMsg( MSG_NO_TIME, NULL );
                    }
                    break;
                default:
                    writeMsg( MSG_UNKNOWN_OPTION, p );
                }
                break;
            }
            /* fall through (could be a file name!) */
        default:
            files++;
            addFileSpec( p );
            break;
        }
    }
    if( files == 0 ) {
        usage();
    }
}

static void doTouch( void )
/*************************/
{
    int stat_rc;
    int utime_rc;
    int fh;
    char *item;
    DIR *dirpt;
    struct dirent *ndir;
    unsigned number_of_successful_touches;
    struct file_list *curr;
    struct utimbuf stamp;
    struct stat sb;
    char sp_buf[ _MAX_PATH2 ];
    char *drive;
    char *dir;
    char full_name[_MAX_PATH];
    char dir_name[_MAX_PATH];
    int len;

    if( TouchFlags.time_specified ) {
        touchTime.tm_sec = TimeAdjust.seconds;
        touchTime.tm_min = TimeAdjust.minutes;
        touchTime.tm_hour = TimeAdjust.hours;
    }
    if( TouchFlags.date_specified ) {
        touchTime.tm_mday = DateAdjust.day;
        touchTime.tm_mon = DateAdjust.month - 1;
        touchTime.tm_year = DateAdjust.year - 1900;
        touchTime.tm_isdst = -1;
    }
    if( TouchFlags.increment_time ) {
        incTouchTime();
    }
    syncStamp( &stamp );
    for( curr = fileList; curr != NULL; curr = curr->next ) {
        item = curr->file_spec;
        len = strlen( item );
        // If a directory was specified determine if that is so and make
        // it more palatable if required.
        // the splitpath will nuke the dot instead of the last part of
        // the directory
        if( ( item[len - 1] == '\\' ) || ( item[len - 1] == '/' ) ) {
            strcpy( dir_name, item );
            dir_name[len] = '.';
            dir_name[len + 1] = '\0';
            item = dir_name;
        } else if( ( strpbrk( item, "*?" ) == NULL ) && !stat( item, &sb ) &&
                    S_ISDIR( sb.st_mode ) ) {
            strcpy( dir_name, item );
        #ifdef __QNX__
            dir_name[len] = '/';
        #else
            dir_name[len] = '\\';
        #endif
            dir_name[len + 1] = '.';
            dir_name[len + 2] = '\0';
            item = dir_name;
        }
        _splitpath2( item, sp_buf, &drive, &dir, NULL, NULL );
        dirpt = opendir( item );
        number_of_successful_touches = 0;
        if( dirpt ) {
            while( ndir = readdir( dirpt ) ) {
                int attr;

                _makepath( full_name, drive, dir, ndir->d_name, NULL );
            #ifdef __QNX__
                if( ( ndir->d_stat.st_status & _FILE_USED ) == 0 ) {
                    // stat information may not be valid
                    stat( full_name, &ndir->d_stat );
                }
                attr = ndir->d_stat.st_mode;
                if( S_ISREG( attr ) ) {
            #else
                attr = ndir->d_attr;
                if( !( ndir->d_attr & ( _A_VOLID | _A_SUBDIR ) ) ) {
            #endif
                    incFilesOwnTime( ndir, &stamp );
                    utime_rc = utime( full_name, &stamp );
                    if( utime_rc == -1 ) {
                        stat_rc = stat( full_name, &sb );
                        if( TouchFlags.allow_read_only && stat_rc == 0 ) {
                            chmod( full_name, sb.st_mode | S_IWRITE );
                            utime_rc = utime( full_name, &stamp );
                            chmod( full_name, sb.st_mode );
                        }
                        if( utime_rc == -1 ) {
                            if( !TouchFlags.quiet ) {
                                if( stat( full_name, &sb ) == 0 &&
                                        ( sb.st_mode & S_IWRITE ) == 0 ) {
                                    writeMsg( MSG_READ_ONLY, full_name );
                                    ++number_of_successful_touches;
                                } else {
                                    writeMsg( MSG_CANT_TOUCH, full_name );
                                }
                            }
                        } else {
                            ++number_of_successful_touches;
                        }
                    } else {
                        ++number_of_successful_touches;
                    }
                }
            }
            if( closedir( dirpt ) != 0 ) {
                writeMsg( MSG_ERR_CLOSE, item );
                continue;
            }
        }
        if( number_of_successful_touches == 0 ) {
            if( strpbrk( item, "*?" ) != NULL ) {
                writeMsg( MSG_NO_MATCH, item );
                continue;
            }
            if( TouchFlags.create_file ) {
                if( access( item, F_OK ) == 0 ) {
                    // (for some reason we could not touch the file)
                    // this prevents us from truncating the file if
                    // the OS (Windows/NT) has some weird case where we
                    // can't change the timestamp but we can truncate it
                    continue;
                }
                fh = creat( item, S_IWRITE | S_IREAD | S_IRGRP | S_IROTH );
                if( fh == -1 ) {
                    if( !TouchFlags.quiet ) {
                        writeMsg( MSG_CANT_CREATE, item );
                    }
                    continue;
                }
                close( fh );
                if( utime( item, &stamp ) == -1 ) {
                    if( !TouchFlags.quiet ) {
                        writeMsg( MSG_CANT_TOUCH_FILE, item );
                    }
                    continue;
                }
            }
        }
    }
}

void main( int argc, char **argv )
/********************************/
{
    if( !MsgInit() ) exit( EXIT_FAILURE );
    processOptions( argc, argv );
    doTouch();
    MsgFini();
    exit( EXIT_SUCCESS );
}
