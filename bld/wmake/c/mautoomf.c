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


#include "make.h"
#include "mcache.h"
#include "memory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#include "mautodep.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "pcobj.h"

typedef struct {
    int         handle;         // file handle of open obj file
    time_t      time_stamp;     // time stamp of next dependancy comment
    char        *name;          // point to nameBuffer - name of next dependancy comment
} omf_info;

static omf_info fileHandle;
static char     nameBuffer[ _MAX_PATH2 + 1 ];

#pragma pack( push,1 );
typedef struct {
    uint_8              bits;
    uint_8              type;
    uint_16             dos_time;
    uint_16             dos_date;
    uint_8              name_len;
} obj_comment;
#pragma pack( pop );

static BOOLEAN verifyObjFile( int fh )
/************************************/
{
    auto struct {
        obj_record      header;
        obj_name        name;
    } theadr;

    if( lseek( fh, 0, SEEK_SET ) < 0 ) {
        return( FALSE );
    }
    if( read( fh, &theadr, sizeof(theadr) ) != sizeof(theadr) ) {
        return( FALSE );
    }
    if( theadr.header.command != CMD_THEADR ) {
        return( FALSE );
    }
    if(( theadr.name.len + 2 ) != theadr.header.length ) {
        return( FALSE );
    }
    if( lseek( fh, 0, SEEK_SET ) < 0 ) {
        return( FALSE );
    }
    return( TRUE );
}

STATIC omf_info *OMFInitFile( const char *name ) {
/************************************************/

    int         handle;
    omf_info    *ret_val;

    ret_val = NULL;
    handle = open( name, O_RDONLY|O_BINARY );
    if( handle != -1 ) {
        fileHandle.handle = handle;
        if( verifyObjFile( handle ) ) {
            ret_val = &fileHandle;
        } else {
            close( handle );
        }
    }
    return( ret_val );
}

static BOOLEAN getCommentRecord( omf_info *info ) {
/*************************************************/

    obj_record          header;
    obj_comment         comment;
    int                 hdl;
    unsigned            len;

    hdl = info->handle;
    for(;;) {
        if( read( hdl, &header, sizeof( header ) ) != sizeof( header ) ) break;
        if( header.command != CMD_COMENT ) {
            // first LNAMES record means objfile has no dependancy info
            if( header.command == CMD_LNAMES ) break;
            lseek( hdl, header.length, SEEK_CUR );
            continue;
        }
        if( read( hdl, &comment, sizeof( comment ) ) != sizeof( comment ) ) break;
        if( comment.type != CMT_DEPENDENCY ) {
            lseek( hdl, header.length - sizeof( comment ), SEEK_CUR );
            continue;
        }
        // NULL dependency means end of dependency info
        if( header.length < sizeof( comment ) ) break;
        // we have a dependency comment!
        len = comment.name_len + 1;
        if( read( hdl, &nameBuffer, len ) != len ) break;
        nameBuffer[ len - 1 ] = '\0';
        info->time_stamp = SysDOSStampToTime( comment.dos_date, comment.dos_time );
        info->name = &nameBuffer[ 0 ];
        return( TRUE );
    }
    return( FALSE );
}

STATIC omf_info *OMFNextDep( omf_info *info ) {
/**********************************************/

    if( getCommentRecord( info ) ) {
        return( info );
    }
    return( NULL );
}

STATIC void OMFTransDep( omf_info *info, char **name, time_t *stamp ) {
/*********************************************************************/

    *name = info->name;
    *stamp = info->time_stamp;
}

STATIC void OMFFiniFile( omf_info *info ) {
/*****************************************/

    close( info->handle );
}

const auto_dep_info OMFAutoDepInfo = {
    NULL,
    OMFInitFile,
    OMFNextDep,
    OMFTransDep,
    OMFNextDep,
    OMFFiniFile,
    NULL
};
