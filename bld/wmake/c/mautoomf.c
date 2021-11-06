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
* Description:  OMF autodependency records access routines.
*
****************************************************************************/


#include "make.h"
#include "mcache.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#include "mautodep.h"
#include "dostimet.h"
#include "pcobj.h"

#include "clibext.h"


typedef struct {
    FILE    *fp;            // file handle of open obj file
    time_t  time_stamp;     // time stamp of next dependancy comment
    char    *name;          // point to nameBuffer - name of next dependancy comment
} omf_info;

static omf_info fileHandle;
static char     nameBuffer[_MAX_PATH2 + 1];

#include "pushpck1.h"
typedef struct {
    UINT8   bits;
    UINT8   type;
    UINT16  dos_time;
    UINT16  dos_date;
    UINT8   name_len;
} omf_comment;
#include "poppck.h"

static bool verifyObjFile( FILE *fp )
/***********************************/
{
    struct {
        omf_record  header;
        omf_name    name;
    }   theadr;

    if( fseek( fp, 0, SEEK_SET ) < 0 ) {
        return( false );
    }
    if( fread( &theadr, 1, sizeof( theadr ), fp ) != sizeof( theadr ) ) {
        return( false );
    }
    if( theadr.header.command != CMD_THEADR ) {
        return( false );
    }
    if( (theadr.name.len + 2) != theadr.header.length ) {
        return( false );
    }
    if( fseek( fp, 0, SEEK_SET ) < 0 ) {
        return( false );
    }
    return( true );
}


STATIC handle OMFInitFile( const char *name )
/*******************************************/
{
    FILE    *fp;
    handle  ret_val;

    ret_val = NULL;
    fp = fopen( name, "rb" );
    if( fp != NULL ) {
        fileHandle.fp = fp;
        if( verifyObjFile( fp ) ) {
            ret_val = &fileHandle;
        } else {
            fclose( fp );
        }
    }
    return( ret_val );
}


static bool getOMFCommentRecord( omf_info *info )
/***********************************************/
{
    omf_record  header;
    omf_comment comment;
    FILE        *fp;
    size_t      len;

    fp = info->fp;
    while( fread( &header, 1, sizeof( header ), fp ) == sizeof( header ) ) {
        if( header.command != CMD_COMENT ) {
            // first LNAMES record means objfile has no dependency info
            if( header.command == CMD_LNAMES ) {
                break;
            }
            fseek( fp, header.length, SEEK_CUR );
            continue;
        }
        if( fread( &comment, 1, sizeof( comment ), fp ) != sizeof( comment ) ) {
            break;
        }
        if( comment.type != CMT_DEPENDENCY ) {
            fseek( fp, header.length - sizeof( comment ), SEEK_CUR );
            continue;
        }
        // NULL dependency means end of dependency info
        if( header.length < sizeof( comment ) ) {
            break;
        }
        // we have a dependency comment! hooray!
        len = comment.name_len + 1;
        if( fread( nameBuffer, 1, len, fp ) != len ) {
            break;  // darn, it's broke
        }
        nameBuffer[len - 1] = NULLCHAR;
        info->time_stamp = _dos2timet( comment.dos_date * 0x10000L + comment.dos_time );
        info->name = nameBuffer;
        return( true );
    }
    return( false );
}

STATIC dep_handle OMFNextDep( dep_handle info )
/*********************************************/
{
    if( getOMFCommentRecord( info ) ) {
        return( info );
    }
    return( NULL );
}

STATIC void OMFTransDep( dep_handle info, char **name, time_t *stamp )
/********************************************************************/
{
    omf_info *o = (void*)info;

    *name  = o->name;
    *stamp = o->time_stamp;
}


STATIC void OMFFiniFile( handle info )
/************************************/
{
    omf_info *o = (void*)info;

    fclose( o->fp );
}


const auto_dep_info OMFAutoDepInfo = {
    NULL,
    OMFInitFile,
    OMFNextDep,
    OMFTransDep,
    OMFNextDep,
    OMFFiniFile,
    NULL,
    AUTO_DEP_OMF
};
