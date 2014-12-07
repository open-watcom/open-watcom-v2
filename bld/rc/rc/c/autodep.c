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
#include "errors.h"
#include "tmpctl.h"
#include "autodep.h"
#include "semantic.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"


typedef struct DepNode {
    struct DepNode      *next;
    DepInfo             info;   /* this must be the last element because
                                 * it contains a dynamic array */
} DepNode;

static DepNode          *depList;

bool AddDependency( char *fname )
{
    char                *name;
    DepNode             *new;
    DepNode             **cur;
    size_t              len;
    int                 cmp;

    if( CmdLineParms.GenAutoDep ) {
        name = RCALLOC( _MAX_PATH );
        _fullpath( name, fname, _MAX_PATH );
        cur = &depList;
        for( ;; ) {
            if( *cur == NULL ) {
                break;
            } else {
                cmp = strcmp( name, (*cur)->info.name );
                if( cmp == 0 ) {
                    RCFREE( name );
                    return( false );
                } else if( cmp > 0 ) {
                    break;
                } else {
                    cur = &(*cur)->next;
                }
            }
        }
        len = strlen( name ) + 1;
        new = RCALLOC( sizeof( DepNode ) + len );
        new->next = *cur;
        new->info.len = len;

        /*
         * Avoid false buffer overflow positive with GCC
         * and -DFORTIFY_SOURCE
         */
        memcpy( new->info.name, name, len + 1);
        *cur = new;
        RCFREE( name );
    }
    return( false );
}

static void writeOneNode( DepInfo *cur )
{
    RawDataItem         item = { 0 };

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

    /* write out len */
    item.Item.Num = cur->len;
    SemWriteRawDataItem( item );

    /* write out file name */
    item.IsString = true;
    item.Item.String = cur->name;
    item.StrLen = strlen( cur->name ) + 1;
    SemWriteRawDataItem( item );
}

static void writeDepListEOF( void )
{
    DepInfo     eof;

    memset( &eof, 0, sizeof( DepInfo ) );
    writeOneNode( &eof );
}

static void freeDepList( void )
{
    DepNode     *cur;
    DepNode     *next;

    for( cur = depList; cur != NULL; cur = next ) {
        next = cur->next;
        RCFREE( cur );
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
            cur->info.time = file_info.st_mtime;
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

extern void AutoDepInitStatics( void )
/************************************/
{
    depList = NULL;
}
