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
#include "watcom.h"
#include "wreglbl.h"
#include "wremsg.h"
#include "rcstr.gh"
#include "wrewait.h"
#include "wrdll.h"
#include "wrselft.h"
#include "wreselft.h"
#include "wregetfn.h"
#include "wregcres.h"
#include "wresvmlt.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool             SaveObjectsAs( WRECurrentResInfo *curr, WRSaveIntoData *idata );
static bool             SaveObjectsInto( WRECurrentResInfo *curr, WRSaveIntoData *idata );
static WRSaveIntoData   *WRECreateSaveData( WRECurrentResInfo *curr );
static WRSaveIntoData   *WREMakeSaveIntoNode( WRECurrentResInfo *curr );
static void             WREFreeSaveIntoData( WRSaveIntoData *idata );
static WRSaveIntoData   *WREAllocSaveIntoData( void );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
extern char *WREResSaveIntoTitle;
extern char *WREResSaveAsTitle;
extern char *WREResSaveMltFilter;

bool SaveMultObjects( bool save_into )
{
    WRECurrentResInfo   curr;
    WRSaveIntoData      *idata;
    bool                ok;

    WRESetWaitCursor( TRUE );

    curr.info = NULL;
    curr.type = NULL;

    WREGetCurrentResource( &curr );

    ok = (curr.info != NULL && curr.type != NULL);

    if( ok ) {
        if( curr.lang == NULL ) {
            idata = WRECreateSaveData( &curr );
        } else {
            idata = WREMakeSaveIntoNode( &curr );
        }
        ok = (idata != NULL);
    }

    if( ok ) {
        if( save_into ) {
            ok = SaveObjectsInto( &curr, idata );
        } else {
            ok = SaveObjectsAs( &curr, idata );
        }
    }

    if( idata != NULL ) {
        WREFreeSaveIntoData( idata );
    }

    WRESetWaitCursor( FALSE );

    return( ok );
}

bool SaveObjectsAs( WRECurrentResInfo *curr, WRSaveIntoData *idata )
{
    bool                ok;
    char                *fname;
    WRFileType          ftype;
    WREGetFileStruct    gf;

    fname = NULL;

    ok = (curr != NULL && idata != NULL);

    if( ok ) {
        gf.file_name = NULL;
        gf.title = WREResSaveAsTitle;
        gf.filter = WREResSaveMltFilter;
        gf.save_ext = TRUE;
        fname = WREGetSaveFileName( &gf );
        ok = (fname != NULL && *fname != '\0');
    }

    if( ok ) {
        ftype = WRESelectFileType( fname, curr->info->is32bit );
        ok = (ftype != WR_DONT_KNOW);
    }

    if( ok ) {
        ok = WRSaveObjectAs( fname, ftype, idata );
    }

    if( fname != NULL ) {
        WRMemFree( fname );
    }

    return( ok );
}

bool SaveObjectsInto( WRECurrentResInfo *curr, WRSaveIntoData *idata )
{
    bool                ok;
    char                *fname;
    WREGetFileStruct    gf;
    int                 dup;

    fname = NULL;
    dup = FALSE;

    ok = (curr != NULL && idata != NULL);

    if( ok ) {
        gf.file_name = NULL;
        gf.title = WREResSaveIntoTitle;
        gf.filter = WREResSaveMltFilter;
        gf.save_ext = TRUE;
        fname = WREGetOpenFileName( &gf );
        ok = (fname != NULL && *fname != '\0');
    }

    if( ok ) {
        ok = WRSaveObjectInto( fname, idata, &dup ) && !dup;
    }

    if( dup ) {
        WREDisplayErrorMsg( WRE_ERRORONDUPOBJS );
    }

    if( fname != NULL ) {
        WRMemFree( fname );
    }

    return( ok );
}

WRSaveIntoData *WRECreateSaveData( WRECurrentResInfo *curr )
{
    WRECurrentResInfo   tcurr;
    WResResNode         *rnode;
    WResLangNode        *lnode;
    WRSaveIntoData      *nodes;
    WRSaveIntoData      *new;

    if( curr == NULL ) {
        return( NULL );
    }

    nodes = NULL;
    tcurr = *curr;
    rnode = curr->type->Head;

    while( rnode != NULL ) {
        lnode = rnode->Head;
        while( lnode != NULL ) {
            tcurr.res = rnode;
            tcurr.lang = lnode;
            new = WREMakeSaveIntoNode( &tcurr );
            if( new == NULL ) {
                WREFreeSaveIntoData( nodes );
                return( NULL );
            }
            if( nodes ) {
                new->next = nodes;
                nodes = new;
            } else {
                nodes = new;
            }
            lnode = lnode->Next;
        }
        rnode = rnode->Next;
    }

    return( nodes );
}

WRSaveIntoData *WREMakeSaveIntoNode( WRECurrentResInfo *curr )
{
    WRSaveIntoData      *node;

    if( curr == NULL ) {
        return( NULL );
    }

    if( curr->lang->Info.Length == 0 ) {
        WREDisplayErrorMsg( WRE_UPDATEBEFORESAVE1 );
        return( NULL );
    }

    node = WREAllocSaveIntoData();

    node->info = curr->info->info;
    node->type = &curr->type->Info.TypeName;
    node->name = &curr->res->Info.ResName;
    node->lang = curr->lang->Info.lang;
    node->size = curr->lang->Info.Length;
    node->MemFlags = curr->lang->Info.MemoryFlags;
    node->data = WREGetCurrentResData( curr );

    if( node->data == NULL ) {
        WRMemFree( node );
        node = NULL;
    }

    return( node );
}

void WREFreeSaveIntoData( WRSaveIntoData *idata )
{
    WRSaveIntoData *next;

    while( idata != NULL ) {
        next = idata->next;
        if( idata->data != NULL ) {
            WRMemFree( idata->data );
        }
        WRMemFree( idata );
        idata = next;
    }
}

WRSaveIntoData *WREAllocSaveIntoData( void )
{
    WRSaveIntoData *idata;

    idata = (WRSaveIntoData *)WRMemAlloc( sizeof( WRSaveIntoData ) );
    if( idata != NULL ) {
        memset( idata, 0, sizeof( WRSaveIntoData ) );
    }

    return( idata );
}
