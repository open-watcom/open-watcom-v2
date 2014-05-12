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


#include "precomp.h"
#include <string.h>
#include <stdlib.h>

#include "watcom.h"
#include "wrdll.h"
#include "wstring.h"

#include "wreglbl.h"
#include "wregcres.h"
#include "wrerenam.h"
#include "wrenames.h"
#include "wrestrdp.h"
#include "wrelist.h"
#include "wrenew.h"
#include "wredel.h"
#include "wrestat.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wreres.h"
#include "wreftype.h"
#include "wretoolb.h"
#include "wremain.h"
#include "wre_wres.h"
#include "wre_rc.h"
#include "wrestr.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define MAX_RETRIES 99

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WREStringSession {
    WStringHandle       hndl;
    WStringInfo         *info;
    WResTypeNode        *tnode;
    WREResInfo          *rinfo;
} WREStringSession;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WREStringSession *WREFindStringSession( WStringHandle );
static WREStringSession *WREAllocStringSession( void );
static WREStringSession *WREStartStringSession( WRECurrentResInfo *, WStringNode *node );
static bool             WREAddStringToDir( WRECurrentResInfo * );
static WStringNode      *WREMakeNode( WRECurrentResInfo * );
static WStringNode      *WRECreateStringNodes( WRECurrentResInfo * );
static void             WREFreeStringNode( WStringNode *node );
static bool             WREGetStringSessionData( WREStringSession *, bool );
static void             WRERemoveStringEditSession( WREStringSession * );
static WREStringSession *WREFindResStringSession( WREResInfo *rinfo );
static WResTypeNode     *WREUseStringNodes( WResDir dir, WStringNode *node );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST *WREStringSessions = NULL;

extern bool WRENoInterface;

void WRERemoveStringEditSession( WREStringSession *session )
{
    if( session != NULL ) {
        ListRemoveElt( &WREStringSessions, session );
        if( session->info != NULL ) {
            WStrFreeStringInfo( session->info );
        }
        WRMemFree( session );
    }
}

bool WREAddStringToDir( WRECurrentResInfo *curr )
{
    WResLangType    lang;
    int             dup, num_retries;
    WResID          *rname, *tname;
    bool            ok, tname_alloc;

    ok = TRUE;
    tname_alloc = FALSE;

    WREGetCurrentResource( curr );

    if( curr->info == NULL ) {
        curr->info = WRECreateNewResource( NULL );
        ok = (curr->info != NULL);
    }

    // check for an existing string tables
    if( ok ) {
        if( WREFindTypeNode( curr->info->info->dir, (uint_16)RT_STRING, NULL ) ) {
            WREDisplayErrorMsg( WRE_STRINGTABLEEXISTS );
            return( FALSE );
        }
    }

    if( ok ) {
        if( curr->info->current_type == (uint_16)RT_STRING ) {
            tname = &curr->type->Info.TypeName;
        } else {
            tname = WResIDFromNum( (uint_16)RT_STRING );
            tname_alloc = TRUE;
        }
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
    }

    if( ok ) {
        dup = TRUE;
        num_retries = 0;
        rname = NULL;
        while( ok && dup && num_retries <= MAX_RETRIES ) {
            rname = WResIDFromNum( 0 );
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEF_MEMFLAGS, 0, 0,
                                     &lang, &dup, (uint_16)RT_STRING, tname_alloc );
                if( !ok && dup ) {
                    ok = TRUE;
                }
                num_retries++;
            }
            if( rname != NULL ) {
                WRMemFree( rname );
            }
        }
        if( dup ) {
            WREDisplayErrorMsg( WRE_CANTFINDUNUSEDNAME );
        }
    }

    if( ok ) {
        curr->info->modified = TRUE;
    }

    if( tname_alloc ) {
        WRMemFree( tname );
    }

    return( ok );
}

bool WRENewStringResource( void )
{
    WRECurrentResInfo  curr;
    bool               ok;

    ok = WREAddStringToDir( &curr );

    if( ok ) {
        ok = (WREStartStringSession( &curr, NULL ) != NULL);
    }

    return( ok );
}

bool WREEditStringResource( WRECurrentResInfo *curr )
{
    WStringNode         *nodes;
    bool                ok;
    WREStringSession    *session;

    nodes = NULL;

    ok = (curr != NULL && curr->info != NULL && curr->type != NULL);

    if( ok ) {
        session = WREFindResStringSession( curr->info );
        if( session != NULL ) {
            WStringBringToFront( session->hndl );
            return( TRUE );
        }
    }

    if( ok ) {
        nodes = WRECreateStringNodes( curr );
        ok = (nodes != NULL);
    }

    if( ok ) {
        ok = (WREStartStringSession( curr, nodes ) != NULL);
    }

    return( ok );
}

bool WREEndEditStringResource( WStringHandle hndl )
{
    WREStringSession    *session;
    WRECurrentResInfo   curr;
    bool                ret;

    ret = FALSE;

    session = WREFindStringSession( hndl );

    if( session != NULL ) {
        ret = TRUE;
        if( session->tnode == NULL || session->tnode->Head->Head->Info.Length == 0 ) {
            curr.info = session->rinfo;
            curr.type = session->tnode;
            curr.res = NULL;
            curr.lang = NULL;
            ret = WREDeleteStringResources( &curr, TRUE );
            WRESetStatusByID( -1, WRE_EMPTYREMOVED );
        }
        WRERemoveStringEditSession( session );
    }

    return( ret );
}

bool WRESaveEditStringResource( WStringHandle hndl )
{
    WREStringSession *session;

    session = WREFindStringSession( hndl );
    if( session == NULL ) {
        return( FALSE );
    }

    return( WREGetStringSessionData( session, FALSE ) );
}

WREStringSession *WREStartStringSession( WRECurrentResInfo *curr, WStringNode *nodes )
{
    WREStringSession *session;

    if( curr == NULL ) {
        return( NULL );
    }

    session = WREAllocStringSession();
    if( session == NULL ) {
        return( NULL );
    }

    session->info = WStrAllocStringInfo();
    if( session->info == NULL ) {
        return( NULL );
    }

    session->info->parent = WREGetMainWindowHandle();
    session->info->inst = WREGetAppInstance();
    session->info->file_name = WREStrDup( WREGetQueryName( curr->info ) );
    session->info->tables = nodes;
    session->info->is32bit = curr->info->is32bit;

    session->info->stand_alone = WRENoInterface;
    session->info->symbol_table = curr->info->symbol_table;
    session->info->symbol_file = curr->info->symbol_file;

    session->tnode = curr->type;
    session->rinfo = curr->info;

    session->hndl = WRStringStartEdit( session->info );

    if( session->hndl != NULL ) {
        WREInsertObject( &WREStringSessions, session );
        if( nodes != NULL ) {
            WREFreeStringNode( nodes );
            session->info->tables = NULL;
        }
    } else {
        WStrFreeStringInfo( session->info );
        WRMemFree( session );
        session = NULL;
    }

    return( session );
}

bool WREGetStringSessionData( WREStringSession *session, bool close )
{
    WResTypeNode        *tnode;
    uint_16             type;
    bool                ok;

    ok = (session != NULL && session->info != NULL && session->hndl != NULL);

    if( ok ) {
        if( close ) {
            session->info = WStringEndEdit( session->hndl );
        } else {
            session->info = WStringGetEditInfo( session->hndl );
        }
        ok = (session->info != NULL);
    }

    if( ok ) {
        tnode = WREFindTypeNode( session->rinfo->info->dir, (uint_16)RT_STRING, NULL );
    }

    if( ok && tnode != NULL && session->info->modified ) {
        ok = WRRemoveTypeNodeFromDir( session->rinfo->info->dir, tnode );
        tnode = NULL;
    }

    if( ok && session->info->modified ) {
        tnode = WREUseStringNodes( session->rinfo->info->dir, session->info->tables );
        type = 0;
        if( tnode != NULL ) {
            type = (uint_16)RT_STRING;
        }
        session->rinfo->current_type = 0;
        ok = WREInitResourceWindow( session->rinfo, type );
        session->tnode = tnode;
        WREFreeStringNode( session->info->tables );
        session->info->tables = NULL;
        session->info->modified = FALSE;
        session->rinfo->modified = TRUE;
    }

    return( ok );
}

bool WREEndAllStringSessions( bool fatal_exit )
{
    WREStringSession    *session;
    LIST                *slist;
    bool                ok;

    ok = TRUE;

    if( WREStringSessions != NULL ) {
        for( slist = WREStringSessions; slist != NULL; slist = ListNext( slist ) ) {
            session = (WREStringSession *)ListElement( slist );
            if( session != NULL ) {
                ok = WStringCloseSession( session->hndl, fatal_exit );
            }
        }
        if( ok ) {
            ListFree( WREStringSessions );
            WREStringSessions = NULL;
        }
    }

    return( ok );
}

void WREEndResStringSessions( WREResInfo *rinfo )
{
    WREStringSession    *session;

    session = WREFindResStringSession( rinfo );
    while( session != NULL ) {
        session->info = WStringEndEdit( session->hndl );
        WRERemoveStringEditSession( session );
        session = WREFindResStringSession( rinfo );
    }
}

WREStringSession *WREFindStringSession( WStringHandle hndl )
{
    WREStringSession    *session;
    LIST                *slist;

    for( slist = WREStringSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREStringSession *)ListElement( slist );
        if( session->hndl == hndl ) {
            return( session );
        }
    }

    return( NULL );
}

WREStringSession *WREFindResStringSession( WREResInfo *rinfo )
{
    WREStringSession    *session;
    LIST                *slist;

    for( slist = WREStringSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREStringSession *)ListElement( slist );
        if( session->rinfo == rinfo ) {
            return( session );
        }
    }

    return( NULL );
}

WREStringSession *WREAllocStringSession( void )
{
    WREStringSession *session;

    session = (WREStringSession *)WRMemAlloc( sizeof( WREStringSession ) );

    if( session != NULL ) {
        memset( session, 0, sizeof( WREStringSession ) );
    }

    return( session );
}

WStringNode *WREMakeNode( WRECurrentResInfo *curr )
{
    WStringNode *node;

    if( curr == NULL ) {
        return( NULL );
    }

    node = (WStringNode *)WRMemAlloc( sizeof( WStringNode ) );
    if( node == NULL ) {
        return( NULL );
    }
    memset( node, 0, sizeof( WStringNode ) );

    node->lang = curr->lang->Info.lang;
    node->MemFlags = curr->lang->Info.MemoryFlags;
    node->block_name = WRECopyWResID( &curr->res->Info.ResName );
    node->data_size = curr->lang->Info.Length;
    node->data = WREGetCurrentResData( curr );

    if( node->data == NULL ) {
        WREFreeStringNode( node );
        node = NULL;
    }

    return( node );
}

WStringNode *WRECreateStringNodes( WRECurrentResInfo *curr )
{
    WRECurrentResInfo   tcurr;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WStringNode         *nodes;
    WStringNode         *new;
    WResLangType        lang;

    if( curr == NULL ) {
        return( NULL );
    }

    nodes = NULL;
    tcurr = *curr;
    rnode = curr->type->Head;

    while( rnode ) {
        lnode = rnode->Head;
        if( lnode != NULL ) {
            lang = lnode->Info.lang;
        }
        while( lnode != NULL ) {
            if( lnode->Info.lang.lang == lang.lang &&
                lnode->Info.lang.sublang == lang.sublang ) {
                tcurr.res = rnode;
                tcurr.lang = lnode;
                new = WREMakeNode( &tcurr );
                if( new == NULL ) {
                    WREFreeStringNode( nodes );
                    return( NULL );
                }
                if( nodes != NULL ) {
                    new->next = nodes;
                    nodes = new;
                } else {
                    nodes = new;
                }
            }
            lnode = lnode->Next;
        }
        rnode = rnode->Next;
    }

    return( nodes );
}

void WREFreeStringNode( WStringNode *node )
{
    WStringNode *n;

    while( node != NULL ) {
        n = node;
        node = node->next;
        if( n->block_name ) {
            WRMemFree( n->block_name );
        }
        if( n->data ) {
            WRMemFree( n->data );
        }
        WRMemFree( n );
    }
}

WResTypeNode *WREUseStringNodes( WResDir dir, WStringNode *node )
{
    WResID              *tname;
    WResTypeNode        *tnode;
    bool                ok;

    tnode = NULL;
    tname = WResIDFromNum( (long)RT_STRING );
    ok = (tname != NULL);

    while( ok && node != NULL ) {
        ok = !WResAddResource( tname, node->block_name, node->MemFlags, 0,
                               node->data_size, dir, &node->lang, NULL );
        if( ok ) {
            ok = WRFindAndSetData( dir, tname, node->block_name,
                                   &node->lang, node->data );
            node->data = NULL;
        }
        node = node->next;
    }

    if( tname != NULL ) {
        WRMemFree( tname );
    }

    if( ok ) {
        tnode = WREFindTypeNode( dir, (uint_16)RT_STRING, NULL );
    }

    return( tnode );
}
