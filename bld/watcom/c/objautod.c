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
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "pcobj.h"
#include "objautod.h"

#pragma pack(1)
enum {
    TIME_SEC_B  = 0,
    TIME_SEC_F  = 0x001f,
    TIME_MIN_B  = 5,
    TIME_MIN_F  = 0x07e0,
    TIME_HOUR_B = 11,
    TIME_HOUR_F = 0xf800
};

enum {
    DATE_DAY_B  = 0,
    DATE_DAY_F  = 0x001f,
    DATE_MON_B  = 5,
    DATE_MON_F  = 0x01e0,
    DATE_YEAR_B = 9,
    DATE_YEAR_F = 0xfe00
};

static int verifyOBJFile( int fh )
{
    auto struct {
        obj_record      header;
        obj_name        name;
    } theadr;

    if( lseek( fh, 0, SEEK_SET ) < 0 ) {
        return( 0 );
    }
    if( read( fh, &theadr, sizeof(theadr) ) != sizeof(theadr) ) {
        return( 0 );
    }
    if( theadr.header.command != CMD_THEADR ) {
        return( 0 );
    }
    if(( theadr.name.len + 2 ) != theadr.header.length ) {
        return( 0 );
    }
    if( lseek( fh, 0, SEEK_SET ) < 0 ) {
        return( 0 );
    }
    return( 1 );
}

static time_t dosStampToTime( unsigned short date, unsigned short time )
{
    struct tm tmbuf;

    tmbuf.tm_year = ( ( date & DATE_YEAR_F ) >> DATE_YEAR_B ) + 80;
    tmbuf.tm_mon  = ( ( date & DATE_MON_F ) >> DATE_MON_B ) - 1;
    tmbuf.tm_mday = ( date & DATE_DAY_F ) >> DATE_DAY_B;

    tmbuf.tm_hour = ( time & TIME_HOUR_F ) >> TIME_HOUR_B;
    tmbuf.tm_min  = ( time & TIME_MIN_F ) >> TIME_MIN_B;
    tmbuf.tm_sec  = ( ( time & TIME_SEC_F ) >> TIME_SEC_B ) * 2;

    tmbuf.tm_isdst= -1;

    return( mktime( &tmbuf ) );
}

walk_status WalkOBJAutoDep( char *file_name, rtn_status (*rtn)( time_t, char *, void * ), void *data )
/****************************************************************************************************/
{
    walk_status wstatus;
    rtn_status rstatus;
    time_t DOS_stamp_time;
    int fh;
    size_t len;
    auto obj_record header;
    auto struct {
        uint_8          bits;
        uint_8          type;
        uint_16         dos_time;
        uint_16         dos_date;
        uint_8          name_len;
    } comment;
    auto char buff[256];

    fh = open( file_name, O_RDONLY|O_BINARY );
    if( fh < 0 ) {
        return( ADW_FILE_NOT_FOUND );
    }
    wstatus = ADW_NOT_AN_OBJ;
    if( verifyOBJFile( fh ) ) {
        wstatus = ADW_OK;
        for(;;) {
            if( read( fh, &header, sizeof(header) ) != sizeof(header) ) {
                wstatus = ADW_FILE_ERROR;
                break;
            }
            if( header.command != CMD_COMENT ) {
                if( header.command == CMD_LNAMES ) {
                    /* first LNAMES record means object file doesn't have deps */
                    wstatus = ADW_NO_DEPS;
                    break;
                }
                lseek( fh, header.length, SEEK_CUR );
                continue;
            }
            if( read( fh, &comment, sizeof(comment) ) != sizeof(comment) ) {
                wstatus = ADW_FILE_ERROR;
                break;
            }
            if( comment.type != CMT_DEPENDENCY ) {
                lseek( fh, (fpos_t)header.length - sizeof(comment), SEEK_CUR );
                continue;
            }
            if( header.length < sizeof( comment ) ) {
                /* null dependency ends our search */
                break;
            }
            len = comment.name_len;
            /* read in the checksum byte to stay in synch */
            ++len;
            if( read( fh, buff, len ) != len ) {
                wstatus = ADW_FILE_ERROR;
                break;
            }
            buff[len-1] = '\0';
            if( rtn != NULL ) {
                DOS_stamp_time = dosStampToTime( comment.dos_date,
                                                 comment.dos_time );
                rstatus = (*rtn)( DOS_stamp_time, buff, data );
                if( rstatus == ADR_STOP ) {
                    wstatus = ADW_RTN_STOPPED;
                    break;
                }
            }
        }
    }
    close( fh );
    return( wstatus );
}

#ifdef TEST
rtn_status dumpDeps( time_t t, char *name, void *data )
{
    data = data;
    printf( "%s %s\n", name, ctime( &t ), name );
    return( ADR_CONTINUE );
}

rtn_status dump2Deps( time_t t, char *name, void *data )
{
    unsigned *pc = data;

    --*pc;
    if( *pc ) {
        printf( "%s %s\n", name, ctime( &t ), name );
        return( ADR_CONTINUE );
    }
    return( ADR_STOP );
}

int main( int argc, char **argv )
{
    unsigned count;

    if( argc !=2 ) {
        puts( "usage: objautod <file>" );
        return( 1 );
    }
    WalkOBJAutoDep( argv[1], NULL, NULL );
    WalkOBJAutoDep( argv[1], dumpDeps, NULL );
    count = 2;
    WalkOBJAutoDep( argv[1], dump2Deps, &count );
    return( 0 );
}
#endif
#pragma pack()
