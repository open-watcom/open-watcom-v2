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
#include "wmenu.h"
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
#include "wreres.h"
#include "wremain.h"
#include "wre_wres.h"
#include "wre.rh"
#include "wremenu.h"
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
typedef struct WREMenuSession {
    WMenuHandle         hndl;
    WMenuInfo           *info;
    WResTypeNode        *typenode;
    WResResNode         *resnode;
    WResLangNode        *langnode;
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
static WREMenuSession   *WREFindLangMenuSession( WResLangNode *langnode );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST     *WREMenuSessions = NULL;
static uint_32  WRENumMenuTitles = 0;

extern bool WRENoInterface;

static void DumpEmptyResource( WREMenuSession *session )
{
    WRECurrentResInfo   curr;

    if( !session->langnode->Info.Length ) {
        curr.info = session->rinfo;
        curr.typenode = session->typenode;
        curr.resnode = session->resnode;
        curr.langnode = session->langnode;
        WRERemoveEmptyResource( &curr );
        WRESetStatusByID( 0, WRE_EMPTYREMOVED );
    }
}

void WRERemoveMenuEditSession( WREMenuSession *session )
{
    if( session != NULL ) {
        ListRemoveElt( &WREMenuSessions, session );
        if( session->info != NULL ) {
            WMenuFreeMenuInfo( session->info );
        }
        MemFree( session );
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
        title = MemAlloc( strlen( text ) + 20 + 1 );
        if( title != NULL ) {
            title[0] = '\0';
            sprintf( title, text, WRENumMenuTitles );
            name = WResIDFromStr( title );
            MemFree( title );
        }
        FreeRCString( text );
    }

    return( name );
}

bool WREAddMenuToDir( WRECurrentResInfo *curr )
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
        if( curr->info->current_type == RESOURCE2INT( RT_MENU ) ) {
            tname = &curr->typenode->Info.TypeName;
        } else {
            tname = WResIDFromNum( RESOURCE2INT( RT_MENU ) );
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
            rname = WRECreateMenuTitle();
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEFAULT_FLAGS_MP, 0, 0,
                                     &lang, &dup, RESOURCE2INT( RT_MENU ), tname_alloc );
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
    bool                ok;
    WREMenuSession      *session;

    ok = (curr != NULL && curr->langnode != NULL);

    if( ok ) {
        session = WREFindLangMenuSession( curr->langnode );
        if( session == NULL ) {
            if( curr->langnode->data == NULL && curr->langnode->Info.Length != 0 ) {
                curr->langnode->data = WREGetCopyResData( curr );
                if( curr->langnode->data == NULL ) {
                    ok = false;
                } else {
                    if( WREStartMenuSession( curr ) == NULL ) {
                        ok = false;
                    }
                    MemFree( curr->langnode->data );
                    curr->langnode->data = NULL;
                }
            } else {
                if( WREStartMenuSession( curr ) == NULL ) {
                    ok = false;
                }
            }
        } else {
            WMenuBringToFront( session->hndl );
        }
    }

    return( ok );
}

bool WREEndEditMenuResource( WMenuHandle hndl )
{
    WREMenuSession      *session;
    bool                ret;

    ret = false;

    session = WREFindMenuSession( hndl );

    if( session != NULL ) {
        ret = true;
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
        return( false );
    }

    return( WREGetMenuSessionData( session, false ) );
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

    session->hndl = WMenuStartEdit( session->info );

    if( session->hndl != 0 ) {
        WREInsertObject( &WREMenuSessions, session );
    } else {
        WMenuFreeMenuInfo( session->info );
        MemFree( session );
        session = NULL;
    }

    return( session );
}

bool WREGetMenuSessionData( WREMenuSession *session, bool close )
{
    bool ok;

    ok = (session != NULL && session->hndl != 0 && session->langnode != NULL);

    if( ok ) {
        if( close ) {
            session->info = WMenuEndEdit( session->hndl );
        } else {
            session->info = WMenuGetEditInfo( session->hndl );
        }
        ok = (session->info != NULL);
    }

    if( ok && session->info->modified ) {
        ok = WRERenameWResResNode( session->typenode, &session->resnode,
                                   session->info->res_name );
        WRESetResNamesFromType( session->rinfo, RESOURCE2INT( RT_MENU ), true,
                                session->info->res_name, 0 );
    }

    if( ok && session->info->modified ) {
        if( session->langnode->data != NULL ) {
            MemFree( session->langnode->data );
        }
        session->langnode->data = session->info->data;
        session->langnode->Info.lang = session->info->lang;
        session->langnode->Info.Length = (uint_32)session->info->data_size;
        session->langnode->Info.res_flags = session->info->res_flags;
        session->langnode->Info.Offset = 0;
        session->info->data = NULL;
        session->info->data_size = 0;
        session->info->modified = false;
        session->rinfo->modified = true;
    }

    return( ok );
}

bool WREEndAllMenuSessions( bool fatal_exit )
{
    WREMenuSession      *session;
    LIST                *slist;
    bool                ok;

    ok = true;

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

void WREEndLangMenuSession( WResLangNode *langnode )
{
    WREMenuSession      *session;

    while( (session = WREFindLangMenuSession( langnode )) != NULL ) {
        session->info = WMenuEndEdit( session->hndl );
        WRERemoveMenuEditSession( session );
    }
}

void WREEndResMenuSessions( WREResInfo *rinfo )
{
    WREMenuSession      *session;

    while( (session = WREFindResMenuSession( rinfo )) != NULL ) {
        session->info = WMenuEndEdit( session->hndl );
        WRERemoveMenuEditSession( session );
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

WREMenuSession *WREFindLangMenuSession( WResLangNode *langnode )
{
    WREMenuSession      *session;
    LIST                *slist;

    for( slist = WREMenuSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREMenuSession *)ListElement( slist );
        if( session->langnode == langnode ) {
            return( session );
        }
    }

    return( NULL );
}

WREMenuSession *WREAllocMenuSession( void )
{
    WREMenuSession *session;

    session = MemAlloc( sizeof( WREMenuSession ) );

    if( session != NULL ) {
        memset( session, 0, sizeof( WREMenuSession ) );
    }

    return( session );
}
