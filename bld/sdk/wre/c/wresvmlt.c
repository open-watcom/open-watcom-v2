/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "wremsg.h"
#include "rcstr.grh"
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

    WRESetWaitCursor( true );

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

    WRESetWaitCursor( false );

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
        gf.save_ext = true;
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
    bool                dup;

    fname = NULL;
    dup = false;

    ok = (curr != NULL && idata != NULL);

    if( ok ) {
        gf.file_name = NULL;
        gf.title = WREResSaveIntoTitle;
        gf.filter = WREResSaveMltFilter;
        gf.save_ext = true;
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
    WRSaveIntoData      *idata;
    WRSaveIntoData      *idata2;

    if( curr == NULL ) {
        return( NULL );
    }

    idata = NULL;
    tcurr = *curr;
    for( rnode = curr->type->Head; rnode != NULL; rnode = rnode->Next ) {
        for( lnode = rnode->Head; lnode != NULL; lnode = lnode->Next ) {
            tcurr.res = rnode;
            tcurr.lang = lnode;
            idata2 = WREMakeSaveIntoNode( &tcurr );
            if( idata2 == NULL ) {
                WREFreeSaveIntoData( idata );
                return( NULL );
            }
            if( idata != NULL ) {
                idata2->next = idata;
                idata = idata2;
            } else {
                idata = idata2;
            }
        }
    }

    return( idata );
}

WRSaveIntoData *WREMakeSaveIntoNode( WRECurrentResInfo *curr )
{
    WRSaveIntoData      *idata;

    if( curr == NULL ) {
        return( NULL );
    }

    if( curr->lang->Info.Length == 0 ) {
        WREDisplayErrorMsg( WRE_UPDATEBEFORESAVE1 );
        return( NULL );
    }

    idata = WREAllocSaveIntoData();

    idata->info = curr->info->info;
    idata->type = &curr->type->Info.TypeName;
    idata->name = &curr->res->Info.ResName;
    idata->lang = curr->lang->Info.lang;
    idata->size = curr->lang->Info.Length;
    idata->MemFlags = curr->lang->Info.MemoryFlags;
    idata->data = WREGetCopyResData( curr );

    if( idata->data == NULL ) {
        WRMemFree( idata );
        idata = NULL;
    }

    return( idata );
}

void WREFreeSaveIntoData( WRSaveIntoData *idata )
{
    WRSaveIntoData *idata2;

    for( ; idata != NULL; idata = idata2 ) {
        idata2 = idata->next;
        if( idata->data != NULL ) {
            WRMemFree( idata->data );
        }
        WRMemFree( idata );
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
