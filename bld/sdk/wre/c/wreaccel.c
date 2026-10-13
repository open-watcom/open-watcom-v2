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
#include "waccel.h"
#include "wregcres.h"
#include "wrenames.h"
#include "wrerenam.h"
#include "wrestrdp.h"
#include "wrelist.h"
#include "wrenew.h"
#include "wremsg.h"
#include "ldstr.h"
#include "wredel.h"
#include "wrestat.h"
#include "wreres.h"
#include "wremain.h"
#include "wre_wres.h"
#include "wre.rh"
#include "wreaccel.h"
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
typedef struct WREAccelSession {
    WAccelHandle        hndl;
    WAccelInfo          *info;
    WResTypeNode        *typenode;
    WResResNode         *resnode;
    WResLangNode        *langnode;
    WREResInfo          *rinfo;
} WREAccelSession;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WResID           *WRECreateAccTitle( void );
static WREAccelSession  *WREFindAccelSession( WAccelHandle );
static WREAccelSession  *WREAllocAccelSession( void );
static WREAccelSession  *WREStartAccelSession( WRECurrentResInfo * );
static bool             WREAddAccelToDir( WRECurrentResInfo * );
static bool             WREGetAccelSessionData( WREAccelSession *, bool );
static void             WRERemoveAccelEditSession( WREAccelSession * );
static WREAccelSession  *WREFindResAccelSession( WREResInfo *rinfo );
static WREAccelSession  *WREFindLangAccelSession( WResLangNode *langnode );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST     *WREAccSessions = NULL;
static uint_32  WRENumAccTitles = 0;

extern bool WRENoInterface;

static void DumpEmptyResource( WREAccelSession *session )
{
    WRECurrentResInfo   curr;

    if( session->langnode->Info.Length == 0 ) {
        curr.info = session->rinfo;
        curr.typenode = session->typenode;
        curr.resnode = session->resnode;
        curr.langnode = session->langnode;
        WRERemoveEmptyResource( &curr );
        WRESetStatusByID( 0, WRE_EMPTYREMOVED );
    }
}

void WRERemoveAccelEditSession( WREAccelSession *session )
{
    if( session != NULL ) {
        ListRemoveElt( &WREAccSessions, session );
        if( session->info != NULL ) {
            WAccFreeAccelInfo( session->info );
        }
        MemFree( session );
    }
}

WResID *WRECreateAccTitle( void )
{
    char        *text;
    char        *title;
    WResID      *name;

    WRENumAccTitles++;

    name = NULL;
    text = AllocRCString( WRE_DEFACCELNAME );
    if( text != NULL ) {
        title = MemAlloc( strlen( text ) + 20 + 1 );
        if( title != NULL ) {
            title[0] = '\0';
            sprintf( title, text, WRENumAccTitles );
            name = WResIDFromStr( title );
            MemFree( title );
        }
        FreeRCString( text );
    }

    return( name );
}

bool WREAddAccelToDir( WRECurrentResInfo *curr )
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

    if( ok ) {
        if( curr->info->current_type == RESOURCE2INT( RT_ACCELERATOR ) ) {
            tname = &curr->typenode->Info.TypeName;
        } else {
            tname = WResIDFromNum( RESOURCE2INT( RT_ACCELERATOR ) );
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
            rname = WRECreateAccTitle();
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEFAULT_FLAGS_MP, 0, 0,
                                     &lang, &dup, RESOURCE2INT( RT_ACCELERATOR ),
                                     tname_alloc );
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

bool WRENewAccelResource( void )
{
    WRECurrentResInfo  curr;
    bool               ok;

    ok = WREAddAccelToDir( &curr );

    if( ok ) {
        ok = (WREStartAccelSession( &curr ) != NULL);
    }

    return( ok );
}

bool WREEditAccelResource( WRECurrentResInfo *curr )
{
    bool                ok;
    WREAccelSession     *session;

    ok = (curr != NULL && curr->langnode != NULL);

    if( ok ) {
        session = WREFindLangAccelSession( curr->langnode );
        if( session == NULL ) {
            if( curr->langnode->data == NULL && curr->langnode->Info.Length != 0 ) {
                curr->langnode->data = WREGetCopyResData( curr );
                if( curr->langnode->data == NULL ) {
                    ok = false;
                } else {
                    if( WREStartAccelSession( curr ) == NULL ) {
                        ok = false;
                    }
                    MemFree( curr->langnode->data );
                    curr->langnode->data = NULL;
                }
            } else {
                if( WREStartAccelSession( curr ) == NULL ) {
                    ok = false;
                }
            }
        } else {
            WAccelBringToFront( session->hndl );
        }
    }

    return( ok );
}

bool WREEndEditAccelResource( WAccelHandle hndl )
{
    WREAccelSession     *session;
    bool                ret;

    ret = false;

    session = WREFindAccelSession( hndl );

    if( session != NULL ) {
        ret = true;
        DumpEmptyResource( session );
        WRERemoveAccelEditSession( session );
    }

    return( ret );
}

bool WRESaveEditAccelResource( WAccelHandle hndl )
{
    WREAccelSession *session;

    session = WREFindAccelSession( hndl );
    if( session == NULL ) {
        return( false );
    }

    return( WREGetAccelSessionData( session, false ) );
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
    session->info->file_name = WREStrdup( WREGetQueryName( curr->info ) );
    session->info->res_name = WRECopyWResID( &curr->resnode->Info.ResName );
    session->info->lang = curr->langnode->Info.lang;
    session->info->res_flags = curr->langnode->Info.res_flags;
    session->info->data_size = curr->langnode->Info.Length;
    session->info->data = curr->langnode->data;
    session->info->is32bit = curr->info->is32bit;

    session->info->stand_alone = WRENoInterface;
    session->info->symbol_table = curr->info->symbol_table;
    session->info->symbol_file = curr->info->symbol_file;

    session->typenode = curr->typenode;
    session->resnode = curr->resnode;
    session->langnode = curr->langnode;
    session->rinfo = curr->info;

    session->hndl = WAccelStartEdit( session->info );

    if( session->hndl ) {
        WREInsertObject( &WREAccSessions, session );
    } else {
        WAccFreeAccelInfo( session->info );
        MemFree( session );
        session = NULL;
    }

    return( session );
}

bool WREGetAccelSessionData( WREAccelSession *session, bool close )
{
    bool ok;

    ok = (session != NULL && session->hndl != 0 && session->langnode != NULL);

    if( ok ) {
        if( close ) {
            session->info = WAccelEndEdit( session->hndl );
        } else {
            session->info = WAccelGetEditInfo( session->hndl );
        }
        ok = (session->info != NULL);
    }

    if( ok && session->info->modified ) {
        ok = WRERenameWResResNode( session->typenode, &session->resnode,
                                   session->info->res_name );
        WRESetResNamesFromType( session->rinfo, RESOURCE2INT( RT_ACCELERATOR ), true,
                                session->info->res_name, 0 );
    }

    if( ok && session->info->modified ) {
        if( session->langnode->data != NULL ) {
            MemFree( session->langnode->data );
        }
        session->langnode->data = session->info->data;
        session->langnode->Info.lang = session->info->lang;
        session->langnode->Info.Length = session->info->data_size;
        session->langnode->Info.res_flags = session->info->res_flags;
        session->langnode->Info.Offset = 0;
        session->info->data = NULL;
        session->info->data_size = 0;
        session->info->modified = false;
        session->rinfo->modified = true;
    }

    return( ok );
}

bool WREEndAllAccelSessions( bool fatal_exit )
{
    WREAccelSession     *session;
    LIST                *slist;
    bool                ok;

    ok = true;

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

void WREEndLangAccelSession( WResLangNode *langnode )
{
    WREAccelSession     *session;

    while( (session = WREFindLangAccelSession( langnode )) != NULL ) {
        session->info = WAccelEndEdit( session->hndl );
        WRERemoveAccelEditSession( session );
    }
}

void WREEndResAccelSessions( WREResInfo *rinfo )
{
    WREAccelSession     *session;

    while( (session = WREFindResAccelSession( rinfo )) != NULL ) {
        session->info = WAccelEndEdit( session->hndl );
        WRERemoveAccelEditSession( session );
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

WREAccelSession *WREFindLangAccelSession( WResLangNode *langnode )
{
    WREAccelSession *session;
    LIST            *slist;

    for( slist = WREAccSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREAccelSession *)ListElement( slist );
        if( session->langnode == langnode ) {
            return( session );
        }
    }

    return( NULL );
}

WREAccelSession *WREAllocAccelSession( void )
{
    WREAccelSession *session;

    session = MemAlloc( sizeof( WREAccelSession ) );

    if( session != NULL ) {
        memset( session, 0, sizeof( WREAccelSession ) );
    }

    return( session );
}
