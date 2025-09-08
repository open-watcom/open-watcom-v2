/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Output OBJ queues routines
*
****************************************************************************/

#include "asmglob.h"
#include "asmalloc.h"
#include "mangle.h"
#include "directiv.h"
#include "omfobjre.h"
#include "omfqueue.h"
#include "queues.h"
#include "myassert.h"
#include "omfgen.h"


typedef struct queuenode {
    void *next;
    void *data;
} queuenode;

static qdesc   *LnameQueue  = NULL;   // queue of LNAME structs
static qdesc   *PubQueue    = NULL;   // queue of pubdefs
static qdesc   *AliasQueue  = NULL;   // queue of aliases
static qdesc   *LinnumQueue = NULL;   // queue of linnum_data structs

static void QAddItem( qdesc **queue, void *data )
/***********************************************/
{
    queuenode   *node;

    node = AsmAlloc( sizeof( queuenode ) );
    node->data = data;
    if( *queue == NULL ) {
        *queue = AsmAlloc( sizeof( qdesc ) );
        QInit( *queue );
    }
    QEnqueue( *queue, node );
}

static int QCount( qdesc *q )
/****************************
 * count the # of entries in the queue
 * if the retval is -1 then we have an error
 */
{
    unsigned long       count;
    queuenode           *node;

    count = 0;
    if( q != NULL ) {
        for( node = q->head; node != NULL; node = node->next ) {
            count++;
            if( count > INT_MAX ) {
                return( -1 );
            }
        }
    }
    return( (int)count );
}

static char **NameArray;

const char *NameGet( uint_16 hdl )
/*********************************
 * OMF output callback routine
 */
{
    return( NameArray[hdl] );
}

void AddPublicData( dir_node_handle dir )
/***************************************/
{
    dir->sym.public = true;
    QAddItem( &PubQueue, dir );
}

void AddPublicProc( dir_node_handle dir )
/***************************************/
{
    QAddItem( &PubQueue, dir );
}

bool GetPublicData( void )
/************************/
{
    obj_rec_handle  objr;
    queuenode       *start;
    queuenode       *curr;
    queuenode       *last;
    dir_node_handle curr_seg;
    dir_node_handle dir;
    pubdef_data     *d;
    unsigned char   cmd;
    name_handle     i;

    if( PubQueue == NULL )
        return( false );

    for( start = PubQueue->head; start != NULL; start = last ) {
        dir = (dir_node_handle)start->data;
        cmd = ( dir->sym.public ) ? CMD_PUBDEF : CMD_STATIC_PUBDEF;
        objr = ObjNewRec( cmd );
        objr->is_32 = 0;
        objr->u.pubdef.free_pubs = 1;
        objr->u.pubdef.num_pubs = 0;
        objr->u.pubdef.base.frame = 0;
        objr->u.pubdef.base.grp_idx = 0;
        objr->u.pubdef.base.seg_idx = 0;
        curr_seg = (dir_node_handle)dir->sym.segment;
        if( curr_seg != NULL ) {
            objr->u.pubdef.base.seg_idx = curr_seg->e.seginfo->idx;
            if( curr_seg->e.seginfo->group != NULL ) {
                objr->u.pubdef.base.grp_idx = ((dir_node_handle)curr_seg->e.seginfo->group)->e.grpinfo->idx;
            }
        }
        for( curr = start; curr != NULL; curr = curr->next ) {
            if( objr->u.pubdef.num_pubs > MAX_PUB_SIZE )
                break;
            dir = (dir_node_handle)curr->data;
            if( (dir_node_handle)dir->sym.segment != curr_seg )
                break;
            if( dir->sym.state == SYM_PROC ) {
                if( dir->sym.public ) {
                    if( cmd == CMD_STATIC_PUBDEF ) {
                        break;
                    }
                } else {
                    if( cmd == CMD_PUBDEF ) {
                        break;
                    }
                }
            } else {
                if( cmd == CMD_STATIC_PUBDEF ) {
                    break;
                }
            }
            if( dir->sym.offset > 0xffffUL ) {
                objr->is_32 = 1;
            }
            objr->u.pubdef.num_pubs++;
        }
        last = curr;

        d = AsmAlloc( objr->u.pubdef.num_pubs * sizeof( pubdef_data ) );
        objr->u.pubdef.pubs = d;
        NameArray = AsmAlloc( objr->u.pubdef.num_pubs * sizeof( char * ) );
        for( i = 0, curr = start; curr != last; curr = curr->next, ++i ) {
            dir = (dir_node_handle)curr->data;
            NameArray[i] = Mangle( &dir->sym );
            d->name = i;
            if( dir->sym.state != SYM_CONST ) {
                d->offset = dir->sym.offset;
            } else {
                if( dir->e.constinfo->tokens[0].class != TC_NUM  ) {
                    AsmWarn( 2, PUBLIC_CONSTANT_NOT_NUMERIC );
                    d->offset = 0;
                } else {
                    d->offset = dir->e.constinfo->tokens[0].u.value;
                }
            }
            d->type.idx = 0;
            ++d;
        }
        d = objr->u.pubdef.pubs;
        write_record( objr, true );
        for( i = 0; i < objr->u.pubdef.num_pubs; i++ ) {
            AsmFree( NameArray[i] );
        }
        AsmFree( NameArray );
    }
    return( true );
}

static void FreePubQueue( void )
/******************************/
{
    if( PubQueue != NULL ) {
        while( PubQueue->head != NULL ) {
            AsmFree( QDequeue( PubQueue ) );
        }
        AsmFree( PubQueue );
    }
}

void AddAliasData( char *data )
/*****************************/
{
    QAddItem( &AliasQueue, data );
}

char *GetAliasData( bool first )
/******************************/
{
    static queuenode    *node;
    char                *p;

    if( AliasQueue == NULL )
        return( NULL );
    if( first )
        node = AliasQueue->head;
    if( node == NULL )
        return( NULL );
    p = node->data ;
    node = node->next;
    return( p );
}

static void FreeAliasQueue( void )
/********************************/
{
    if( AliasQueue != NULL ) {
        while( AliasQueue->head != NULL ) {
            queuenode   *node;

            node = QDequeue( AliasQueue );
            AsmFree( node->data );
            AsmFree( node );
        }
        AsmFree( AliasQueue );
    }
}

void AddLnameData( dir_node_handle dir )
/**************************************/
{
    QAddItem( &LnameQueue, dir );
}

bool GetLnameData( obj_rec_handle objr )
/**************************************/
{
    queuenode       *curr;
    dir_node_handle dir;
    size_t          len;

    if( LnameQueue == NULL )
        return( false );

    len = 0;
    for( curr = LnameQueue->head; curr != NULL ; curr = curr->next ) {
        dir = (dir_node_handle)(curr->data);
        len += strlen( dir->sym.name ) + 1;
    }
    ObjAllocData( objr, (uint_16)len );
    for( curr = LnameQueue->head; curr != NULL ; curr = curr->next ) {
        dir = (dir_node_handle)curr->data;
        len = strlen( dir->sym.name );
        ObjPutName( objr, dir->sym.name, (uint_8)len );
        objr->u.lnames.num_names++;
        switch( dir->sym.state ) {
        case SYM_GRP:
            dir->e.grpinfo->idx = objr->u.lnames.num_names;
            break;
        case SYM_SEG:
            dir->e.seginfo->idx = objr->u.lnames.num_names;
            break;
        case SYM_CLASS_LNAME:
            dir->e.lnameinfo->idx = objr->u.lnames.num_names;
            break;
        }
    }
    return( true );
}

static void FreeLnameQueue( void )
/********************************/
{
    dir_node_handle dir;
    queuenode       *node;

    if( LnameQueue != NULL ) {
        while( LnameQueue->head != NULL ) {
            node = QDequeue( LnameQueue );
            dir = (dir_node_handle)node->data;
            if( dir->sym.state == SYM_CLASS_LNAME ) {
                AsmFree( dir->e.lnameinfo );
                AsmFree( dir->sym.name );
                AsmFree( dir );
            }
            AsmFree( node );
        }
        AsmFree( LnameQueue );
    }
}

void AddLinnumData( line_num_info *data )
/***************************************/
{
    QAddItem( &LinnumQueue, data );
}

int GetLinnumData( int limit, linnum_data **ldata, bool *need32 )
/***************************************************************/
{
    queuenode       *node;
    line_num_info   *node_data;
    int             count;
    int             i;

    count = QCount( LinnumQueue );
    if( count <= 0 )
        return( 0 );
    if( limit > count )
        limit = count;
    *need32 = false;
    *ldata = AsmAlloc( limit * sizeof( linnum_data ) );
    for( i = 0; i < limit; i++ ) {
        node = QDequeue( LinnumQueue );
        node_data = (line_num_info *)(node->data);
        if( *ldata != NULL ) {
            (*ldata)[i].number = node_data->number;
            (*ldata)[i].offset = node_data->offset;
            if( node_data->offset > 0xffffUL ) {
                *need32 = true;
            }
        }
        AsmFree( node_data );
        AsmFree( node );
    }
    return( limit );
}

static void FreeLinnumQueue( void )
/*********************************/
{
    queuenode *node;

    if( LinnumQueue != NULL ) {
        while( LinnumQueue->head != NULL ) {
            node = QDequeue( LinnumQueue );
            AsmFree( node->data );
            AsmFree( node );
        }
        AsmFree( LinnumQueue );
    }
}

void FreeAllQueues( void )
/************************/
{
    FreeLinnumQueue();
    FreePubQueue();
    FreeAliasQueue();
    FreeLnameQueue();
}
