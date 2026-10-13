/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "wreglbl.h"
#include "wrdll.h"
#include "wstring.h"
#include "wregcres.h"
#include "wrerenam.h"
#include "wrenames.h"
#include "wrestrdp.h"
#include "wrelist.h"
#include "wrenew.h"
#include "wredel.h"
#include "wrestat.h"
#include "wremsg.h"
#include "wreres.h"
#include "wreftype.h"
#include "wretoolb.h"
#include "wremain.h"
#include "wre_wres.h"
#include "wre.rh"
#include "wrestr.h"
#include "wresdefn.h"


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
    WResTypeNode        *typenode;
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
        MemFree( session );
    }
}

bool WREAddStringToDir( WRECurrentResInfo *curr )
{
    WResLangType    lang;
    bool            dup;
    int             num_retries;
    WResID          *rname, *tname;
    bool            ok, tname_alloc;

    ok = true;
    tname_alloc = false;

    WREGetCurrentResource( curr );

    if( curr->info == NULL ) {
        curr->info = WRECreateNewResource( NULL );
        ok = (curr->info != NULL);
    }

    // check for an existing string tables
    if( ok ) {
        if( WREFindTypeNode( curr->info->info->dir, RESOURCE2INT( RT_STRING ), NULL ) ) {
            WREDisplayErrorMsg( WRE_STRINGTABLEEXISTS );
            return( false );
        }
    }

    if( ok ) {
        if( curr->info->current_type == RESOURCE2INT( RT_STRING ) ) {
            tname = &curr->typenode->Info.TypeName;
        } else {
            tname = WResIDFromNum( RESOURCE2INT( RT_STRING ) );
            tname_alloc = true;
        }
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
    }

    if( ok ) {
        dup = true;
        num_retries = 0;
        rname = NULL;
        while( ok && dup && num_retries <= MAX_RETRIES ) {
            rname = WResIDFromNum( 0 );
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEFAULT_FLAGS_MP, 0, 0,
                                     &lang, &dup, RESOURCE2INT( RT_STRING ), tname_alloc );
                if( !ok && dup ) {
                    ok = true;
                }
                num_retries++;
            }
            if( rname != NULL ) {
                MemFree( rname );
            }
        }
        if( dup ) {
            WREDisplayErrorMsg( WRE_CANTFINDUNUSEDNAME );
        }
    }

    if( ok ) {
        curr->info->modified = true;
    }

    if( tname_alloc ) {
        MemFree( tname );
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

    ok = (curr != NULL && curr->info != NULL && curr->typenode != NULL);

    if( ok ) {
        session = WREFindResStringSession( curr->info );
        if( session == NULL ) {
            nodes = WRECreateStringNodes( curr );
            if( nodes == NULL ) {
                ok = false;
            } else {
                if( WREStartStringSession( curr, nodes ) == NULL ) {
                    ok = false;
                }
            }
        } else {
            WStringBringToFront( session->hndl );
        }
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
        ret = true;
        if( session->typenode == NULL || session->typenode->Head->Head->Info.Length == 0 ) {
            curr.info = session->rinfo;
            curr.typenode = session->typenode;
            curr.resnode = NULL;
            curr.langnode = NULL;
            ret = WREDeleteStringResources( &curr, true );
            WRESetStatusByID( 0, WRE_EMPTYREMOVED );
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
    session->info->file_name = WREStrdup( WREGetQueryName( curr->info ) );
    session->info->tables = nodes;
    session->info->is32bit = curr->info->is32bit;

    session->info->stand_alone = WRENoInterface;
    session->info->symbol_table = curr->info->symbol_table;
    session->info->symbol_file = curr->info->symbol_file;

    session->typenode = curr->typenode;
    session->rinfo = curr->info;

    session->hndl = WStringStartEdit( session->info );

    if( session->hndl != 0 ) {
        WREInsertObject( &WREStringSessions, session );
        if( nodes != NULL ) {
            WREFreeStringNode( nodes );
            session->info->tables = NULL;
        }
    } else {
        WStrFreeStringInfo( session->info );
        MemFree( session );
        session = NULL;
    }

    return( session );
}

bool WREGetStringSessionData( WREStringSession *session, bool close )
{
    WResTypeNode        *typenode;
    uint_16             type;
    bool                ok;

    ok = (session != NULL && session->info != NULL && session->hndl != 0);

    if( ok ) {
        if( close ) {
            session->info = WStringEndEdit( session->hndl );
        } else {
            session->info = WStringGetEditInfo( session->hndl );
        }
        ok = (session->info != NULL);
    }

    if( ok ) {
        typenode = WREFindTypeNode( session->rinfo->info->dir, RESOURCE2INT( RT_STRING ), NULL );
    }

    if( ok && typenode != NULL && session->info->modified ) {
        ok = WRRemoveTypeNodeFromDir( session->rinfo->info->dir, typenode );
        typenode = NULL;
    }

    if( ok && session->info->modified ) {
        typenode = WREUseStringNodes( session->rinfo->info->dir, session->info->tables );
        type = 0;
        if( typenode != NULL ) {
            type = RESOURCE2INT( RT_STRING );
        }
        session->rinfo->current_type = 0;
        ok = WREInitResourceWindow( session->rinfo, type );
        session->typenode = typenode;
        WREFreeStringNode( session->info->tables );
        session->info->tables = NULL;
        session->info->modified = false;
        session->rinfo->modified = true;
    }

    return( ok );
}

bool WREEndAllStringSessions( bool fatal_exit )
{
    WREStringSession    *session;
    LIST                *slist;
    bool                ok;

    ok = true;

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

    while( (session = WREFindResStringSession( rinfo )) != NULL ) {
        session->info = WStringEndEdit( session->hndl );
        WRERemoveStringEditSession( session );
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

    session = MemAlloc( sizeof( WREStringSession ) );

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

    node = MemAlloc( sizeof( WStringNode ) );
    if( node == NULL ) {
        return( NULL );
    }
    memset( node, 0, sizeof( WStringNode ) );

    node->lang = curr->langnode->Info.lang;
    node->res_flags = curr->langnode->Info.res_flags;
    node->block_name = WRECopyWResID( &curr->resnode->Info.ResName );
    node->data_size = curr->langnode->Info.Length;
    node->data = WREGetCopyResData( curr );

    if( node->data == NULL ) {
        WREFreeStringNode( node );
        node = NULL;
    }

    return( node );
}

WStringNode *WRECreateStringNodes( WRECurrentResInfo *curr )
{
    WRECurrentResInfo   tcurr;
    WResResNode         *resnode;
    WResLangNode        *langnode;
    WStringNode         *nodes;
    WStringNode         *new;
    WResLangType        lang;

    if( curr == NULL ) {
        return( NULL );
    }

    nodes = NULL;
    tcurr = *curr;
    for( resnode = curr->typenode->Head; resnode != NULL; resnode = resnode->next ) {
        langnode = resnode->Head;
        if( langnode != NULL ) {
            lang = langnode->Info.lang;
        }
        for( ; langnode != NULL; langnode = langnode->next ) {
            if( langnode->Info.lang.lang == lang.lang &&
                langnode->Info.lang.sublang == lang.sublang ) {
                tcurr.resnode = resnode;
                tcurr.langnode = langnode;
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
        }
    }

    return( nodes );
}

void WREFreeStringNode( WStringNode *node )
{
    WStringNode *next;

    for( ; node != NULL; node = next ) {
        next = node->next;
        if( node->block_name != NULL ) {
            MemFree( node->block_name );
        }
        if( node->data != NULL ) {
            MemFree( node->data );
        }
        MemFree( node );
    }
}

WResTypeNode *WREUseStringNodes( WResDir dir, WStringNode *node )
{
    WResID              *tname;
    WResTypeNode        *typenode;
    bool                ok;

    typenode = NULL;
    tname = WResIDFromNum( RESOURCE2INT( RT_STRING ) );
    ok = ( tname != NULL );
    for( ; ok && node != NULL; node = node->next ) {
        ok = !WResAddResource( tname, node->block_name, node->res_flags, 0,
                               (uint_32)node->data_size, dir, &node->lang, NULL );
        if( ok ) {
            ok = WRFindAndSetData( dir, tname, node->block_name,
                                   &node->lang, node->data );
            node->data = NULL;
        }
    }

    if( tname != NULL ) {
        MemFree( tname );
    }

    if( ok ) {
        typenode = WREFindTypeNode( dir, RESOURCE2INT( RT_STRING ), NULL );
    }

    return( typenode );
}
