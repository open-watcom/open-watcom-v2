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
* Description:  Output autodependency information in resource file.
*
****************************************************************************/


#include "wio.h"
#include "global.h"
#include "rcerrors.h"
#include "tmpctl.h"
#include "autodep.h"
#include "semantic.h"
#include "rcrtns.h"
#include "depend.h"
#include "rccore.h"

#include "clibext.h"


typedef struct {
    uint_32             time;       /* file's time taken from stat */
    uint_16             len;        /* sizeof the name array */
    char                name[1];    /* dynamic array */
} ResDepInfo;

typedef struct DepNode {
    struct DepNode      *next;
    ResDepInfo          info;       /* this must be the last element because it contains a dynamic array */
} DepNode;

static DepNode          *depList;

bool AddDependency( const char *fname )
{
    char                *name;
    DepNode             *new;
    DepNode             **cur;
    size_t              len;
    int                 cmp;

    if( CmdLineParms.GenAutoDep ) {
        cmp = 0;
        name = RESALLOC( _MAX_PATH );
        _fullpath( name, fname, _MAX_PATH );
        for( cur = &depList; *cur != NULL; cur = &(*cur)->next ) {
            cmp = strcmp( name, (*cur)->info.name );
            if( cmp >= 0 ) {
                break;
            }
        }
        if( *cur == NULL || cmp != 0 ) {
            len = strlen( name ) + 1;
            new = RESALLOC( sizeof( DepNode ) - 1 + len );
            new->next = *cur;
            new->info.len = (uint_16)len;
            memcpy( new->info.name, name, len );
            *cur = new;
        }
        RESFREE( name );
    }
    return( false );
}

static void writeOneNode( ResDepInfo *cur )
{
    RawDataItem         item;

    item.IsString  = false;
    item.LongItem  = false;
    item.TmpStr    = false;
    item.WriteNull = false;

    /* write out time */
#ifdef __BIG_ENDIAN__
    item.Item.Num = cur->time >> 16;
    SemWriteRawDataItem( item );
    item.Item.Num = cur->time & 0xffff;
    SemWriteRawDataItem( item );
#else
    item.Item.Num = cur->time & 0xffff;
    SemWriteRawDataItem( item );
    item.Item.Num = cur->time >> 16;
    SemWriteRawDataItem( item );
#endif

    /* write out file name including termination */
    /* length already includes termination */

    /* write out length */
    item.Item.Num = cur->len;
    SemWriteRawDataItem( item );

    /* write out file name including termination */
    item.IsString = true;
    item.Item.String = cur->name;
    item.StrLen = cur->len;
    SemWriteRawDataItem( item );
}

static void writeDepListEOF( void )
{
    ResDepInfo  eof;

    memset( &eof, 0, sizeof( ResDepInfo ) );
    writeOneNode( &eof );
}

static void freeDepList( void )
{
    DepNode     *cur;
    DepNode     *next;

    for( cur = depList; cur != NULL; cur = next ) {
        next = cur->next;
        RESFREE( cur );
    }
}

bool WriteDependencyRes( void )
{
    DepNode             *cur;
    ResLocation         loc;
    WResID              *nameid;
    WResID              *typeid;
    struct stat         file_info;

    if( CmdLineParms.GenAutoDep ) {
        loc.start = SemStartResource();
        for( cur = depList; cur != NULL; cur = cur->next ) {
            if( stat( cur->info.name, &file_info ) == -1 ) {
                RcError( ERR_READING_FILE, cur->info.name, strerror( errno ) );
                ErrorHasOccured = true;
            }
            cur->info.time = (uint_32)file_info.st_mtime;
            writeOneNode( &cur->info );
        }
        writeDepListEOF();
        loc.len = SemEndResource( loc.start );
        nameid = WResIDFromStr( DEP_LIST_NAME );
        typeid = WResIDFromNum( DEP_LIST_TYPE );
        SemAddResourceFree( nameid, typeid, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, loc );
    }
    freeDepList();
    return( false );
}

void AutoDepInitStatics( void )
/*****************************/
{
    depList = NULL;
}
