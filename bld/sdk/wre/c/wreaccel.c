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

#include "waccel.h"

#include "wreglbl.h"
#include "wremem.h"
#include "wregcres.h"
#include "wrenames.h"
#include "wrerenam.h"
#include "wrestrdp.h"
#include "wrelist.h"
#include "wrenew.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wredel.h"
#include "wrestat.h"
#include "wreres.h"
#include "wremain.h"
#include "wre_wres.h"
#include "wre_rc.h"
#include "wreaccel.h"

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
typedef struct WREAccelSession {
    WAccelHandle        hndl;
    WAccelInfo          *info;
    WResTypeNode        *tnode;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WREResInfo          *rinfo;
} WREAccelSession;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WResID           *WRECreateAccTitle( void );
static WREAccelSession  *WREFindAccelSession( WAccelHandle );
static WREAccelSession  *WREAllocAccelSession( void );
static WREAccelSession  *WREStartAccelSession( WRECurrentResInfo * );
static Bool             WREAddAccelToDir( WRECurrentResInfo * );
static Bool             WREGetAccelSessionData( WREAccelSession *, Bool );
static void             WRERemoveAccelEditSession( WREAccelSession * );
static WREAccelSession  *WREFindResAccelSession( WREResInfo *rinfo );
static WREAccelSession  *WREFindLangAccelSession( WResLangNode *lnode );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST     *WREAccSessions = NULL;
static uint_32  WRENumAccTitles = 0;

extern Bool WRENoInterface;

static void DumpEmptyResource( WREAccelSession *session )
{
    WRECurrentResInfo   curr;

    if( session->lnode->Info.Length == 0 ) {
        curr.info = session->rinfo;
        curr.type = session->tnode;
        curr.res = session->rnode;
        curr.lang = session->lnode;
        WRERemoveEmptyResource( &curr );
        WRESetStatusByID( -1, WRE_EMPTYREMOVED );
    }
}

void WRERemoveAccelEditSession( WREAccelSession *session )
{
    if( session != NULL ) {
        ListRemoveElt( &WREAccSessions, session );
        if( session->info != NULL ) {
            WAccFreeAccelInfo( session->info );
        }
        WREMemFree( session );
    }
}

WResID *WRECreateAccTitle( void )
{
    char        *text;
    char        *title;
    WResID      *name;

    WRENumAccTitles++;

    name = NULL;
    text = WREAllocRCString( WRE_DEFACCELNAME );
    if( text != NULL ) {
        title = (char *)WREMemAlloc( strlen( text ) + 20 + 1 );
        if( title != NULL ) {
            title[0] = '\0';
            sprintf( title, text, WRENumAccTitles );
            name = WResIDFromStr( title );
            WREMemFree( title );
        }
        WREFreeRCString( text );
    }

    return( name );
}

Bool WREAddAccelToDir( WRECurrentResInfo *curr )
{
    WResLangType    lang;
    int             dup, num_retries;
    WResID          *rname, *tname;
    Bool            ok, tname_alloc;

    ok = TRUE;
    tname_alloc = FALSE;

    WREGetCurrentResource( curr );

    if( curr->info == NULL ) {
        curr->info = WRECreateNewResource( NULL );
        ok = (curr->info != NULL);
    }

    if( ok ) {
        if( curr->info->current_type == (uint_16)RT_ACCELERATOR ) {
            tname = &curr->type->Info.TypeName;
        } else {
            tname = WResIDFromNum( (uint_16)RT_ACCELERATOR );
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
            rname = WRECreateAccTitle();
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEF_MEMFLAGS, 0, 0,
                                     &lang, &dup, (uint_16)RT_ACCELERATOR,
                                     tname_alloc );
                if( !ok && dup ) {
                    ok = TRUE;
                }
                num_retries++;
            }
            if( rname != NULL ) {
                WREMemFree( rname );
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
        WREMemFree( tname );
    }

    return( ok );
}

Bool WRENewAccelResource( void )
{
    WRECurrentResInfo  curr;
    Bool               ok;

    ok = WREAddAccelToDir( &curr );

    if( ok ) {
        ok = (WREStartAccelSession( &curr ) != NULL);
    }

    return( ok );
}

Bool WREEditAccelResource( WRECurrentResInfo *curr )
{
    void                *rdata;
    Bool                ok, rdata_alloc;
    WREAccelSession     *session;

    rdata = NULL;
    rdata_alloc = FALSE;

    ok = (curr != NULL && curr->lang != NULL);

    if( ok ) {
        session = WREFindLangAccelSession( curr->lang );
        if( session != NULL ) {
            WAccelBringToFront( session->hndl );
            return( TRUE );
        }
    }

    if( ok ) {
        if( curr->lang->data ) {
            rdata = curr->lang->data;
        } else if( curr->lang->Info.Length != 0) {
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
        ok = (WREStartAccelSession( curr ) != NULL);
    }

    if( rdata_alloc ) {
        WREMemFree( rdata );
        curr->lang->data = NULL;
    }

    return( ok );
}

Bool WREEndEditAccelResource( WAccelHandle hndl )
{
    WREAccelSession     *session;
    Bool                ret;

    ret = FALSE;

    session = WREFindAccelSession( hndl );

    if( session != NULL ) {
        ret = TRUE;
        DumpEmptyResource( session );
        WRERemoveAccelEditSession( session );
    }

    return( ret );
}

Bool WRESaveEditAccelResource( WAccelHandle hndl )
{
    WREAccelSession *session;

    session = WREFindAccelSession( hndl );
    if( session == NULL ) {
        return( FALSE );
    }

    return( WREGetAccelSessionData( session, FALSE ) );
}

WREAccelSession *WREStartAccelSession( WRECurrentResInfo *curr )
{
    WREAccelSession *session;

    if( curr == NULL ) {
        return( NULL );
    }

    session = WREAllocAccelSession();
    if( session == NULL ) {
        return( NULL );
    }

    session->info = WAccAllocAccelInfo();
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

    session->hndl = WAccelStartEdit( session->info );

    if( session->hndl ) {
        WREInsertObject( &WREAccSessions, session );
    } else {
        WAccFreeAccelInfo( session->info );
        WREMemFree( session );
        session = NULL;
    }

    return( session );
}

Bool WREGetAccelSessionData( WREAccelSession *session, Bool close )
{
    Bool ok;

    ok = (session != NULL && session->hndl != 0 && session->lnode != NULL);

    if( ok ) {
        if( close ) {
            session->info = WAccelEndEdit( session->hndl );
        } else {
            session->info = WAccelGetEditInfo( session->hndl );
        }
        ok = (session->info != NULL);
    }

    if( ok && session->info->modified ) {
        ok = WRERenameWResResNode( session->tnode, &session->rnode,
                                   session->info->res_name );
        WRESetResNamesFromType( session->rinfo, (uint_16)RT_ACCELERATOR, TRUE,
                                session->info->res_name, 0 );
    }

    if( ok && session->info->modified ) {
        if( session->lnode->data != NULL ) {
            WREMemFree( session->lnode->data );
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

Bool WREEndAllAccelSessions( Bool fatal_exit )
{
    WREAccelSession     *session;
    LIST                *slist;
    Bool                ok;

    ok = TRUE;

    if( WREAccSessions != NULL ) {
        for( slist = WREAccSessions; ok && slist != NULL; slist = ListNext( slist ) ) {
            session = (WREAccelSession *)ListElement( slist );
            if( session != NULL ) {
                ok = WAccelCloseSession( session->hndl, fatal_exit );
            }
        }
        if( ok ) {
            ListFree( WREAccSessions );
            WREAccSessions = NULL;
        }
    }

    return( ok );
}

void WREEndLangAccelSession( WResLangNode *lnode )
{
    WREAccelSession     *session;

    session = WREFindLangAccelSession( lnode );
    while( session != NULL ) {
        session->info = WAccelEndEdit( session->hndl );
        WRERemoveAccelEditSession( session );
        session = WREFindLangAccelSession( lnode );
    }
}

void WREEndResAccelSessions( WREResInfo *rinfo )
{
    WREAccelSession     *session;

    session = WREFindResAccelSession( rinfo );
    while( session != NULL ) {
        session->info = WAccelEndEdit( session->hndl );
        WRERemoveAccelEditSession( session );
        session = WREFindResAccelSession( rinfo );
    }
}

WREAccelSession *WREFindAccelSession( WAccelHandle hndl )
{
    WREAccelSession *session;
    LIST            *slist;

    for( slist = WREAccSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREAccelSession *)ListElement( slist );
        if( session->hndl == hndl ) {
            return( session );
        }
    }

    return( NULL );
}

WREAccelSession *WREFindResAccelSession( WREResInfo *rinfo )
{
    WREAccelSession *session;
    LIST            *slist;

    for( slist = WREAccSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREAccelSession *)ListElement( slist );
        if( session->rinfo == rinfo ) {
            return( session );
        }
    }

    return( NULL );
}

WREAccelSession *WREFindLangAccelSession( WResLangNode *lnode )
{
    WREAccelSession *session;
    LIST            *slist;

    for( slist = WREAccSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREAccelSession *)ListElement( slist );
        if( session->lnode == lnode ) {
            return( session );
        }
    }

    return( NULL );
}

WREAccelSession *WREAllocAccelSession( void )
{
    WREAccelSession *session;

    session = (WREAccelSession *)WREMemAlloc( sizeof( WREAccelSession ) );

    if( session != NULL ) {
        memset( session, 0, sizeof( WREAccelSession ) );
    }

    return( session );
}
