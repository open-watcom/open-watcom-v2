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
#include <stdio.h>

#include "watcom.h"
#include "wmenu.h"

#include "wreglbl.h"
#include "wregcres.h"
#include "wrenames.h"
#include "wrerenam.h"
#include "wrestrdp.h"
#include "wrelist.h"
#include "wrenew.h"
#include "wredel.h"
#include "wrestat.h"
#include "wremsg.h"
#include "ldstr.h"
#include "rcstr.gh"
#include "wreres.h"
#include "wremain.h"
#include "wre_wres.h"
#include "wre_rc.h"
#include "wremenu.h"

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
typedef struct WREMenuSession {
    WMenuHandle         hndl;
    WMenuInfo           *info;
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WREResInfo          *rinfo;
} WREMenuSession;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WResID           *WRECreateMenuTitle( void );
static WREMenuSession   *WREFindMenuSession( WMenuHandle );
static WREMenuSession   *WREAllocMenuSession( void );
static WREMenuSession   *WREStartMenuSession( WRECurrentResInfo * );
static bool             WREAddMenuToDir( WRECurrentResInfo * );
static bool             WREGetMenuSessionData( WREMenuSession *, bool );
static void             WRERemoveMenuEditSession( WREMenuSession * );
static WREMenuSession   *WREFindResMenuSession( WREResInfo *rinfo );
static WREMenuSession   *WREFindLangMenuSession( WResLangNode *lnode );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST     *WREMenuSessions = NULL;
static uint_32  WRENumMenuTitles = 0;

extern bool WRENoInterface;

static void DumpEmptyResource( WREMenuSession *session )
{
    WRECurrentResInfo   curr;

    if( !session->lnode->Info.Length ) {
        curr.info = session->rinfo;
        curr.type = session->tnode;
        curr.res = session->rnode;
        curr.lang = session->lnode;
        WRERemoveEmptyResource( &curr );
        WRESetStatusByID( -1, WRE_EMPTYREMOVED );
    }
}

void WRERemoveMenuEditSession( WREMenuSession *session )
{
    if( session != NULL ) {
        ListRemoveElt( &WREMenuSessions, session );
        if( session->info != NULL ) {
            WMenuFreeMenuInfo( session->info );
        }
        WRMemFree( session );
    }
}

WResID *WRECreateMenuTitle( void )
{
    char        *text;
    char        *title;
    WResID      *name;

    WRENumMenuTitles++;

    name = NULL;
    text = AllocRCString( WRE_DEFMENUNAME );
    if( text != NULL ) {
        title = (char *)WRMemAlloc( strlen( text ) + 20 + 1 );
        if( title != NULL ) {
            title[0] = '\0';
            sprintf( title, text, WRENumMenuTitles );
            name = WResIDFromStr( title );
            WRMemFree( title );
        }
        FreeRCString( text );
    }

    return( name );
}

bool WREAddMenuToDir( WRECurrentResInfo *curr )
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

    if( ok ) {
        if( curr->info->current_type == (uint_16)(pointer_int)RT_MENU ) {
            tname = &curr->type->Info.TypeName;
        } else {
            tname = WResIDFromNum( (uint_16)(pointer_int)RT_MENU );
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
            rname = WRECreateMenuTitle();
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEF_MEMFLAGS, 0, 0,
                                     &lang, &dup, (uint_16)(pointer_int)RT_MENU, tname_alloc );
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

bool WRENewMenuResource( void )
{
    WRECurrentResInfo  curr;
    bool               ok;

    ok = WREAddMenuToDir( &curr );

    if( ok ) {
        ok = (WREStartMenuSession( &curr ) != NULL);
    }

    return( ok );
}

bool WREEditMenuResource( WRECurrentResInfo *curr )
{
    void                *rdata;
    bool                ok, rdata_alloc;
    WREMenuSession      *session;

    rdata = NULL;
    rdata_alloc = FALSE;

    ok = (curr != NULL && curr->lang != NULL);

    if( ok ) {
        session = WREFindLangMenuSession( curr->lang );
        if( session != NULL ) {
            WMenuBringToFront( session->hndl );
            return( TRUE );
        }
    }

    if( ok ) {
        if( curr->lang->data != NULL ) {
            rdata = curr->lang->data;
        } else if( curr->lang->Info.Length != 0 ) {
            ok = ((rdata = WREGetCurrentResData( curr )) != NULL);
            if( ok ) {
                rdata_alloc = TRUE;
            }
        }
    }

    if( ok ) {
        if( rdata_alloc ) {
            curr->lang->data = rdata;
        }
        ok = (WREStartMenuSession( curr ) != NULL);
    }

    if( rdata_alloc ) {
        WRMemFree( rdata );
        curr->lang->data = NULL;
    }

    return( ok );
}

bool WREEndEditMenuResource( WMenuHandle hndl )
{
    WREMenuSession      *session;
    bool                ret;

    ret = FALSE;

    session = WREFindMenuSession( hndl );

    if( session != NULL ) {
        ret = TRUE;
        DumpEmptyResource( session );
        WRERemoveMenuEditSession( session );
    }

    return( ret );
}

bool WRESaveEditMenuResource( WMenuHandle hndl )
{
    WREMenuSession *session;

    session = WREFindMenuSession( hndl );
    if( session == NULL ) {
        return( FALSE );
    }

    return( WREGetMenuSessionData( session, FALSE ) );
}

WREMenuSession *WREStartMenuSession( WRECurrentResInfo *curr )
{
    WREMenuSession *session;

    if( curr == NULL ) {
        return( NULL );
    }

    session = WREAllocMenuSession();
    if( session == NULL ) {
        return( NULL );
    }

    session->info = WMenuAllocMenuInfo();
    if( session->info == NULL ) {
        return( NULL );
    }

    session->info->parent = WREGetMainWindowHandle();
    session->info->inst = WREGetAppInstance();
    session->info->file_name = WREStrDup( WREGetQueryName( curr->info ) );
    session->info->res_name = WRECopyWResID( &curr->res->Info.ResName );
    session->info->lang = curr->lang->Info.lang;
    session->info->MemFlags = curr->lang->Info.MemoryFlags;
    session->info->data_size = curr->lang->Info.Length;
    session->info->data = curr->lang->data;
    session->info->is32bit = curr->info->is32bit;

    session->info->stand_alone = WRENoInterface;
    session->info->symbol_table = curr->info->symbol_table;
    session->info->symbol_file = curr->info->symbol_file;

    session->tnode = curr->type;
    session->rnode = curr->res;
    session->lnode = curr->lang;
    session->rinfo = curr->info;

    session->hndl = WRMenuStartEdit( session->info );

    if( session->hndl != NULL ) {
        WREInsertObject( &WREMenuSessions, session );
    } else {
        WMenuFreeMenuInfo( session->info );
        WRMemFree( session );
        session = NULL;
    }

    return( session );
}

bool WREGetMenuSessionData( WREMenuSession *session, bool close )
{
    bool ok;

    ok = (session != NULL && session->hndl != NULL && session->lnode != NULL);

    if( ok ) {
        if( close ) {
            session->info = WMenuEndEdit( session->hndl );
        } else {
            session->info = WMenuGetEditInfo( session->hndl );
        }
        ok = (session->info != NULL);
    }

    if( ok && session->info->modified ) {
        ok = WRERenameWResResNode( session->tnode, &session->rnode,
                                   session->info->res_name );
        WRESetResNamesFromType( session->rinfo, (uint_16)(pointer_int)RT_MENU, TRUE,
                                session->info->res_name, 0 );
    }

    if( ok && session->info->modified ) {
        if( session->lnode->data != NULL ) {
            WRMemFree( session->lnode->data );
        }
        session->lnode->data = session->info->data;
        session->lnode->Info.lang = session->info->lang;
        session->lnode->Info.Length = session->info->data_size;
        session->lnode->Info.MemoryFlags = session->info->MemFlags;
        session->lnode->Info.Offset = 0;
        session->info->data = NULL;
        session->info->data_size = 0;
        session->info->modified = FALSE;
        session->rinfo->modified = TRUE;
    }

    return( ok );
}

bool WREEndAllMenuSessions( bool fatal_exit )
{
    WREMenuSession      *session;
    LIST                *slist;
    bool                ok;

    ok = TRUE;

    if( WREMenuSessions != NULL ) {
        for( slist = WREMenuSessions; slist != NULL; slist = ListNext( slist ) ) {
            session = (WREMenuSession *)ListElement( slist );
            if( session != NULL ) {
                ok = WMenuCloseSession( session->hndl, fatal_exit );
            }
        }
        if( ok ) {
            ListFree( WREMenuSessions );
            WREMenuSessions = NULL;
        }
    }

    return( ok );
}

void WREEndLangMenuSession( WResLangNode *lnode )
{
    WREMenuSession      *session;

    session = WREFindLangMenuSession( lnode );
    while( session != NULL ) {
        session->info = WMenuEndEdit( session->hndl );
        WRERemoveMenuEditSession( session );
        session = WREFindLangMenuSession( lnode );
    }
}

void WREEndResMenuSessions( WREResInfo *rinfo )
{
    WREMenuSession      *session;

    session = WREFindResMenuSession( rinfo );
    while( session != NULL ) {
        session->info = WMenuEndEdit( session->hndl );
        WRERemoveMenuEditSession( session );
        session = WREFindResMenuSession( rinfo );
    }
}

WREMenuSession *WREFindMenuSession( WMenuHandle hndl )
{
    WREMenuSession *session;
    LIST           *slist;

    for( slist = WREMenuSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREMenuSession *)ListElement( slist );
        if( session->hndl == hndl ) {
            return( session );
        }
    }

    return( NULL );
}

WREMenuSession *WREFindResMenuSession( WREResInfo *rinfo )
{
    WREMenuSession      *session;
    LIST                *slist;

    for( slist = WREMenuSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREMenuSession *)ListElement( slist );
        if( session->rinfo == rinfo ) {
            return( session );
        }
    }

    return( NULL );
}

WREMenuSession *WREFindLangMenuSession( WResLangNode *lnode )
{
    WREMenuSession      *session;
    LIST                *slist;

    for( slist = WREMenuSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREMenuSession *)ListElement( slist );
        if( session->lnode == lnode ) {
            return( session );
        }
    }

    return( NULL );
}

WREMenuSession *WREAllocMenuSession( void )
{
    WREMenuSession *session;

    session = (WREMenuSession *)WRMemAlloc( sizeof( WREMenuSession ) );

    if( session != NULL ) {
        memset( session, 0, sizeof( WREMenuSession ) );
    }

    return( session );
}
