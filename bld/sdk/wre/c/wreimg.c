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
#include <ddeml.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "watcom.h"
#include "wreglbl.h"
#include "wresall.h"
#include "wre_wres.h"
#include "wremem.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wreres.h"
#include "wrestrdp.h"
#include "wregcres.h"
#include "wrenames.h"
#include "wrerenam.h"
#include "wrelist.h"
#include "wrenew.h"
#include "wredel.h"
#include "wrestat.h"
#include "wredde.h"
#include "wre_rc.h"
#include "wreimage.h"
#include "wreimg.h"

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
typedef struct WREImageInfo {
    char            *file_name;
    WResID          *res_name;
    WResLangType    lang;
    uint_16         MemFlags;
    int             is32bit;
    uint_32         data_size;
    void            *data;
} WREImageInfo;

typedef struct WREImageSession {
    HCONV           server;
    HCONV           client;
    uint_16         type;
    Bool            new;
    WREImageInfo    info;
    WResTypeNode    *tnode;
    WResResNode     *rnode;
    WResLangNode    *lnode;
    WREResInfo      *rinfo;
} WREImageSession;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WREImageSession  *WREStartImageSession( WRESPT service, WRECurrentResInfo *curr, Bool new );
static WREImageSession  *WREAllocImageSession( void );
static WREImageSession  *WREFindImageSession( HCONV conv );
static WREImageSession  *WREFindResImageSession( WREResInfo *rinfo );
static WREImageSession  *WREFindLangImageSession( WResLangNode *lnode );
static void             WRERemoveImageEditSession( WREImageSession *session );
static void             WREFreeEditSession( WREImageSession *session );
static void             WREDisconnectSession( WREImageSession *session );
static Bool             WREAddImageToDir( WRECurrentResInfo *curr, uint_16 type );
static void             WREBringSessionToFront( WREImageSession *session );
static void             WREShowSession( WREImageSession *session, Bool show );
static void             WREPokeImageCmd( WREImageSession *session, char *cmd, Bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WREImageSession  *PendingSession = NULL;
static LIST             *WREImageSessions = NULL;
static uint_32          WRENumBitmapTitles = 0;
static uint_32          WRENumCursorTitles = 0;
static uint_32          WRENumIconTitles = 0;

static void DumpEmptyResource( WREImageSession *session )
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

WResID *WRECreateImageTitle( uint_16 type )
{
    char        *text;
    char        *title;
    uint_32     num;
    WResID      *name;

    if( type == (uint_16)RT_BITMAP ) {
        WRENumBitmapTitles++;
        num = WRENumBitmapTitles;
        text = WREAllocRCString( WRE_DEFBITMAPNAME );
    } else if( type == (uint_16)RT_GROUP_CURSOR ) {
        WRENumCursorTitles++;
        num = WRENumCursorTitles;
        text = WREAllocRCString( WRE_DEFCURSORNAME );
    } else if( type == (uint_16)RT_GROUP_ICON ) {
        WRENumIconTitles++;
        num = WRENumIconTitles;
        text = WREAllocRCString( WRE_DEFICONNAME );
    } else {
        return( NULL );
    }

    if( text != NULL ) {
        title = (char *)WREMemAlloc( strlen( text ) + 20 + 1 );
        if( title != NULL ) {
            title[0] = '\0';
            sprintf( title, text, num );
            name = WResIDFromStr( title );
            WREMemFree( title );
        }
        WREFreeRCString( text );
    }

    return( name );
}

Bool WREAddImageToDir( WRECurrentResInfo *curr, uint_16 type )
{
    WResLangType        lang;
    int                 dup, num_retries;
    WResID              *rname, *tname;
    Bool                ok, tname_alloc;

    ok = TRUE;
    tname_alloc = FALSE;

    WREGetCurrentResource( curr );

    if( curr->info == NULL ) {
        curr->info = WRECreateNewResource( NULL );
        ok = (curr->info != NULL);
    }

    if( ok ) {
        if( curr->info->current_type == type ) {
            tname = &curr->type->Info.TypeName;
        } else {
            tname = WResIDFromNum( type );
            tname_alloc = (tname != NULL);
            ok = tname_alloc;
        }
    }

    if( ok ) {
        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        dup = TRUE;
        num_retries = 0;
        rname = NULL;
        while( ok && dup && num_retries <= MAX_RETRIES ) {
            rname = WRECreateImageTitle( type );
            ok = (rname != NULL);
            if( ok ) {
                ok = WRENewResource( curr, tname, rname, DEF_MEMFLAGS, 0, 0,
                                     &lang, &dup, type, tname_alloc );
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

Bool WRENewImageResource( WRESPT service, uint_16 type )
{
    WRECurrentResInfo  curr;
    Bool               ok;

    ok = WREAddImageToDir( &curr, type );

    if( ok ) {
        ok = (WREStartImageSession( service, &curr, TRUE ) != NULL);
    }

    return( ok );
}

Bool WREDumpPendingImageSession( void )
{
    Bool                ret;

    ret = TRUE;

    if( PendingSession != NULL ) {
        DumpEmptyResource( PendingSession );
        WREFreeEditSession( PendingSession );
        PendingSession = NULL;
        WRESetPendingService( NoServicePending );
        WREDisplayErrorMsg( WRE_IMGSESSIONKILLED );
    }

    return( ret );
}

Bool WREEndEditImageResource( HCONV conv )
{
    WREImageSession     *session;
    Bool                ret;

    ret = FALSE;

    session = WREFindImageSession( conv );

    if( session != NULL ) {
        ret = TRUE;
        DumpEmptyResource( session );
        WRERemoveImageEditSession( session );
    }

    return( ret );
}

Bool WRECommitImageSession( HCONV server, HCONV client )
{
    Bool        ok;

    ok = (client != (HCONV)NULL && server != (HCONV)NULL && PendingSession != NULL);

    if( ok ) {
        WREInsertObject( &WREImageSessions, PendingSession );
        PendingSession->server = server;
        PendingSession->client = client;
    } else {
        WREFreeEditSession( PendingSession );
    }

    WRESetPendingService( NoServicePending );
    PendingSession = NULL;

    return( ok );
}

Bool WREGetImageSessionFileName( HCONV server, void **data, uint_32 *size )
{
    WREImageSession *session;

    if( data == NULL || size == NULL ) {
        return( FALSE );
    }

    session = WREFindImageSession( server );
    if( session == NULL ) {
        return( FALSE );
    }

    *data = WREStrDup( session->info.file_name );
    if( *data != NULL ) {
        *size = strlen( *data ) + 1;
    }

    return( TRUE );
}

Bool WREGetImageSessionResName( HCONV server, void **data, uint_32 *size )
{
    WREImageSession *session;

    if( data == NULL || size == NULL ) {
        return( FALSE );
    }

    session = WREFindImageSession( server );
    if( session == NULL ) {
        return( FALSE );
    }

    if( !WRWResID2Mem( session->info.res_name, data, size,
                       session->info.is32bit ) ) {
        return( FALSE );
    }

    return( TRUE );
}

Bool WREGetImageSessionData( HCONV server, void **data, uint_32 *size )
{
    WREImageSession     *session;

    if( data == NULL || size == NULL ) {
        return( FALSE );
    }

    session = WREFindImageSession( server );
    if( session == NULL ) {
        return( FALSE );
    }

    if( session->info.data == NULL || session->info.data_size == 0 ) {
        *data = NULL;
        *size = 0;
        return( TRUE );
    }

    *size = session->info.data_size;
    *data = WREMemAlloc( *size );
    if( *data == NULL ) {
        return( FALSE );
    }
    memcpy( *data, session->info.data, *size );

    if( session->type == (uint_16)RT_BITMAP ) {
        if( !WREAddBitmapFileHeader( (BYTE **)data, size ) ) {
            if( *data != NULL ) {
                WREMemFree( *data );
            }
            return( FALSE );
        }
    }

    return( TRUE );
}

Bool WRESetImageSessionResName( HCONV server, HDDEDATA hdata )
{
    WREImageSession     *session;
    WResID              *name;
    void                *data;
    uint_32             size;
    Bool                ok;

    ok = (server != (HCONV)NULL && hdata != (HDDEDATA)NULL);

    if( ok ) {
        session = WREFindImageSession( server );
        ok = (session != NULL);
    }

    if( ok ) {
        ok = WREHData2Mem( hdata, &data, &size );
    }

    if( ok ) {
        name = WRMem2WResID( data, FALSE );
        ok = (name != NULL);
    }

    if( ok ) {
        ok = WRERenameWResResNode( session->tnode, &session->rnode, name );
    }

    if( ok ) {
        WRESetResNamesFromType( session->rinfo, session->type, TRUE, name, 0 );
    }

    if( data != NULL ) {
        WREMemFree( data );
    }

    if( name != NULL ) {
        WREMemFree( name );
    }

    return( ok );
}

Bool WRESetBitmapSessionResData( WREImageSession *session, void *data, uint_32 size )
{
    Bool                ok;

    ok = (session != NULL);

    if( ok ) {
        WREStripBitmapFileHeader( (BYTE **)&data, &size );
        if( session->lnode->data != NULL ) {
            WREMemFree( session->lnode->data );
        }
        session->lnode->data = data;
        session->lnode->Info.Length = size;
        session->rinfo->modified = TRUE;
    }

    return( ok );
}

Bool WRESetCursorSessionResData( WREImageSession *session, void *data, uint_32 size )
{
    WRECurrentResInfo   curr;
    Bool                ok;

    ok = (session != NULL);

    if( ok ) {
        curr.info = session->rinfo;
        curr.type = session->tnode;
        curr.res = session->rnode;
        curr.lang = session->lnode;
        if( session->new == NULL ) {
            ok = WREDeleteGroupImages( &curr, (uint_16)RT_GROUP_CURSOR );
        }
    }

    if( ok ) {
        session->rinfo->modified = TRUE;
        ok = WRECreateCursorEntries( &curr, data, size );
    }

    return( ok );
}

Bool WRESetIconSessionResData( WREImageSession *session, void *data, uint_32 size )
{
    WRECurrentResInfo   curr;
    Bool                ok;

    ok = (session != NULL);

    if( ok ) {
        curr.info = session->rinfo;
        curr.type = session->tnode;
        curr.res = session->rnode;
        curr.lang = session->lnode;
        if( session->new == NULL ) {
            ok = WREDeleteGroupImages( &curr, (uint_16)RT_GROUP_ICON );
        }
    }

    if( ok ) {
        session->rinfo->modified = TRUE;
        ok = WRECreateIconEntries( &curr, data, size );
    }

    return( ok );
}

Bool WRESetImageSessionResData( HCONV server, HDDEDATA hdata )
{
    WREImageSession     *session;
    void                *data;
    uint_32             size;
    Bool                ok;

    ok = (server != (HCONV)NULL && hdata != (HDDEDATA)NULL);

    if( ok ) {
        session = WREFindImageSession( server );
        ok = (session != NULL);
    }

    if( ok ) {
        ok = WREHData2Mem( hdata, &data, &size );
    }

    if( ok ) {
        if( session->type == (uint_16)RT_BITMAP ) {
            ok = WRESetBitmapSessionResData( session, data, size );
        } else if( session->type == (uint_16)RT_GROUP_CURSOR ) {
            ok = WRESetCursorSessionResData( session, data, size );
        } else if( session->type == (uint_16)RT_GROUP_ICON ) {
            ok = WRESetIconSessionResData( session, data, size );
        }
    }

    return( ok );
}

WREImageSession *WREStartImageSession( WRESPT service, WRECurrentResInfo *curr, Bool new )
{
    WREImageSession     *session;
    BYTE                *data;
    uint_32             size;

    if( curr == NULL ) {
        return( NULL );
    }

    session = WREAllocImageSession();
    if( session == NULL ) {
        return( NULL );
    }

    session->new = new;
    session->info.data_size = 0;
    session->info.data = NULL;
    data = NULL;
    size = 0;

    switch( service ) {
    case CursorService:
        if( !new && !WRECreateCursorDataFromGroup( curr, &data, &size ) ) {
            return( NULL );
        }
        session->info.data_size = size;
        session->info.data = data;
        break;
    case IconService:
        if( !new && !WRECreateIconDataFromGroup( curr, &data, &size ) ) {
            return( NULL );
        }
        session->info.data_size = size;
        session->info.data = data;
        break;
    case BitmapService:
        if( !new ) {
            session->info.data_size = curr->lang->Info.Length;
            session->info.data = curr->lang->data;
        }
        break;
    case NoServicePending:
    default:
        return( NULL );
    }

    session->info.file_name = WREStrDup( WREGetQueryName( curr->info ) );
    session->info.res_name = WRECopyWResID( &curr->res->Info.ResName );
    session->info.lang = curr->lang->Info.lang;
    session->info.MemFlags = curr->lang->Info.MemoryFlags;
    session->info.is32bit = curr->info->is32bit;

    session->type = curr->info->current_type;
    session->tnode = curr->type;
    session->rnode = curr->res;
    session->lnode = curr->lang;
    session->rinfo = curr->info;

    WRESetPendingService( service );

    PendingSession = session;

    if( WinExec( "wimgedit.exe -dde", SW_SHOW ) < 32 ) {
        WREDisplayErrorMsg( WRE_IMGEDITNOTSPAWNED );
        WREFreeEditSession( session );
        PendingSession = NULL;
        WRESetPendingService( NoServicePending );
    }

    return( session );
}

Bool WREEditImageResource( WRECurrentResInfo *curr )
{
    WREImageSession     *session;
    WRESPT              service;
    Bool                ok;

    ok = (curr != NULL && curr->lang != NULL);

    if( ok ) {
        session = WREFindLangImageSession( curr->lang );
        if( session != NULL ) {
            WREBringSessionToFront( session );
            return( TRUE );
        }
    }

    if( ok ) {
        if( curr->info->current_type == (uint_16)RT_BITMAP ) {
            service = BitmapService;
        } else if( curr->info->current_type == (uint_16)RT_GROUP_CURSOR ) {
            service = CursorService;
        } else if( curr->info->current_type == (uint_16)RT_GROUP_ICON ) {
            service = IconService;
        } else {
            ok = FALSE;
        }
    }

    if( ok ) {
        if( curr->lang->data == NULL && curr->lang->Info.Length != 0 ) {
            curr->lang->data = WREGetCurrentResData( curr );
            ok = (curr->lang->data != NULL);
        }
    }

    if( ok ) {
        ok = (WREStartImageSession( service, curr, FALSE ) != NULL);
    }

    return( ok );
}

Bool WREEndAllImageSessions( Bool fatal_exit )
{
    WREImageSession     *session;
    LIST                *slist;
    Bool                ok;

    _wre_touch( fatal_exit );

    ok = TRUE;

    if( WREImageSessions != NULL ) {
        for( slist = WREImageSessions; ok && slist != NULL; slist = ListNext( slist ) ) {
            session = (WREImageSession *)ListElement( slist );
            if( session != NULL ) {
                WREDisconnectSession( session );
                WREFreeEditSession( session );
            }
        }
        if( ok ) {
            ListFree( WREImageSessions );
            WREImageSessions = NULL;
        }
    }

    return( ok );
}

void WREEndLangImageSession( WResLangNode *lnode )
{
    WREImageSession     *session;

    session = WREFindLangImageSession( lnode );
    while( session != NULL ) {
        WREDisconnectSession( session );
        WRERemoveImageEditSession( session );
        session = WREFindLangImageSession( lnode );
    }
}

void WREEndResImageSessions( WREResInfo *rinfo )
{
    WREImageSession     *session;

    session = WREFindResImageSession( rinfo );
    while( session != NULL ) {
        WREDisconnectSession( session );
        WRERemoveImageEditSession( session );
        session = WREFindResImageSession( rinfo );
    }
}

WREImageSession *WREAllocImageSession( void )
{
    WREImageSession *session;

    session = (WREImageSession *)WREMemAlloc( sizeof( WREImageSession ) );

    if( session != NULL ) {
        memset( session, 0, sizeof( WREImageSession ) );
    }

    return( session );
}

WREImageSession *WREFindImageSession( HCONV conv )
{
    WREImageSession *session;
    LIST            *slist;

    for( slist = WREImageSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREImageSession *)ListElement( slist );
        if( session->server == conv || session->client == conv ) {
            return( session );
        }
    }

    return( NULL );
}

WREImageSession *WREFindResImageSession( WREResInfo *rinfo )
{
    WREImageSession     *session;
    LIST                *slist;

    for( slist = WREImageSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREImageSession *)ListElement( slist );
        if( session->rinfo == rinfo ) {
            return( session );
        }
    }

    return( NULL );
}

WREImageSession *WREFindLangImageSession( WResLangNode *lnode )
{
    WREImageSession     *session;
    LIST                *slist;

    for( slist = WREImageSessions; slist != NULL; slist = ListNext( slist ) ) {
        session = (WREImageSession *)ListElement( slist );
        if( session->lnode == lnode ) {
            return( session );
        }
    }

    return( NULL );
}

void WRERemoveImageEditSession( WREImageSession *session )
{
    if( session != NULL ) {
        ListRemoveElt( &WREImageSessions, session );
        WREFreeEditSession( session );
    }
}

void WREFreeEditSession( WREImageSession *session )
{
    if( session != NULL ) {
        if( session->info.file_name != NULL ) {
            WREMemFree( session->info.file_name );
        }
        if( session->info.res_name != NULL ) {
            WREMemFree( session->info.res_name );
        }
        WREMemFree( session );
    }
}

void WREDisconnectSession( WREImageSession *session )
{
    if( session != NULL ) {
        WREPokeImageCmd( session, "endsession", TRUE );
        DumpEmptyResource( session );
        if( session->server != (HCONV)NULL ) {
            DdeDisconnect( session->server );
            session->server = (HCONV)NULL;
        }
        if( session->client != (HCONV)NULL ) {
            DdeDisconnect( session->client );
            session->client = (HCONV)NULL;
        }
    }
}

void WREBringSessionToFront( WREImageSession *session )
{
    WREPokeImageCmd( session, "bringtofront", FALSE );
}

void WREShowAllImageSessions( Bool show )
{
    WREImageSession     *session;
    LIST                *slist;

    if( WREImageSessions != NULL ) {
        for( slist = WREImageSessions; slist != NULL; slist = ListNext( slist ) ) {
            session = (WREImageSession *)ListElement( slist );
            if( session != NULL ) {
                WREShowSession( session, show );
            }
        }
    }
}

void WREShowSession( WREImageSession *session, Bool show )
{
    if( show ) {
        WREPokeImageCmd( session, "show", FALSE );
    } else {
        WREPokeImageCmd( session, "hide", FALSE );
    }
}

void WREPokeImageCmd( WREImageSession *session, char *cmd, Bool retry )
{
    if( session != NULL && cmd ) {
        WREPokeData( session->client, cmd, strlen( cmd ) + 1, retry );
    }
}
