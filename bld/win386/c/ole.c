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
* Description:  Covers for OLE functions.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <windows.h>
#include <malloc.h>
#include <ole.h>
#include "winext.h"
#include "stubs.h"

#pragma aux Backpatch_olecli parm [ax];
extern LPVOID FAR Backpatch_olecli( char *str );

#pragma aux Backpatch_olesvr parm [ax];
extern LPVOID FAR Backpatch_olesvr( char *str );

static OLESTATUS (FAR PASCAL *olecliOleActivate)(LPOLEOBJECT, UINT, BOOL, BOOL, HWND, const RECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleClone)(LPOLEOBJECT, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleClose)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleCopyFromLink)(LPOLEOBJECT, LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleCopyToClipboard)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleCreate)(LPCSTR, LPOLECLIENT, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleCreateFromClip)(LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleCreateFromFile)(LPCSTR, LPOLECLIENT, LPCSTR, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleCreateFromTemplate)(LPCSTR, LPOLECLIENT, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleCreateInvisible)(LPCSTR, LPOLECLIENT, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT, BOOL);
static OLESTATUS (FAR PASCAL *olecliOleCreateLinkFromClip)(LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleCreateLinkFromFile)(LPCSTR, LPOLECLIENT, LPCSTR, LPCSTR, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*, OLEOPT_RENDER, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleDelete)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleDraw)(LPOLEOBJECT, HDC, const RECT FAR*, const RECT FAR*, HDC);
static OLECLIPFORMAT (FAR PASCAL *olecliOleEnumFormats)(LPOLEOBJECT, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleEqual)(LPOLEOBJECT, LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleExecute)(LPOLEOBJECT, HGLOBAL, UINT);
static OLESTATUS (FAR PASCAL *olecliOleGetData)(LPOLEOBJECT, OLECLIPFORMAT, HANDLE FAR*);
static OLESTATUS (FAR PASCAL *olecliOleGetLinkUpdateOptions)(LPOLEOBJECT, OLEOPT_UPDATE FAR*);
static OLESTATUS (FAR PASCAL *olecliOleLoadFromStream)(LPOLESTREAM, LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleLockServer)(LPOLEOBJECT, LHSERVER FAR*);
static OLESTATUS (FAR PASCAL *olecliOleObjectConvert)(LPOLEOBJECT, LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleQueryBounds)(LPOLEOBJECT, RECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleQueryName)(LPOLEOBJECT, LPSTR, UINT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleQueryOpen)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleQueryOutOfDate)(LPOLEOBJECT);
static LPVOID (FAR PASCAL *olecliOleQueryProtocol)(LPOLEOBJECT, LPCSTR);
static OLESTATUS (FAR PASCAL *olecliOleQueryReleaseError)(LPOLEOBJECT);
static OLE_RELEASE_METHOD (FAR PASCAL *olecliOleQueryReleaseMethod)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleQueryReleaseStatus)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleQuerySize)(LPOLEOBJECT, DWORD FAR*);
static OLESTATUS (FAR PASCAL *olecliOleQueryType)(LPOLEOBJECT, LONG FAR*);
static OLESTATUS (FAR PASCAL *olecliOleReconnect)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleRelease)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olecliOleRename)(LPOLEOBJECT, LPCSTR);
static OLESTATUS (FAR PASCAL *olecliOleRequestData)(LPOLEOBJECT, OLECLIPFORMAT);
static OLESTATUS (FAR PASCAL *olecliOleSaveToStream)(LPOLEOBJECT, LPOLESTREAM);
static OLESTATUS (FAR PASCAL *olecliOleSetBounds)(LPOLEOBJECT, const RECT FAR*);
static OLESTATUS (FAR PASCAL *olecliOleSetColorScheme)(LPOLEOBJECT, const LOGPALETTE FAR*);
static OLESTATUS (FAR PASCAL *olecliOleSetData)(LPOLEOBJECT, OLECLIPFORMAT, HANDLE);
static OLESTATUS (FAR PASCAL *olecliOleSetHostNames)(LPOLEOBJECT, LPCSTR, LPCSTR);
static OLESTATUS (FAR PASCAL *olecliOleSetLinkUpdateOptions)(LPOLEOBJECT, OLEOPT_UPDATE);
static OLESTATUS (FAR PASCAL *olecliOleSetTargetDevice)(LPOLEOBJECT, HGLOBAL);
static OLESTATUS (FAR PASCAL *olecliOleUpdate)(LPOLEOBJECT);
static OLESTATUS (FAR PASCAL *olesvrOleRegisterServer)(LPCSTR, LPOLESERVER, LHSERVER FAR*, HINSTANCE, OLE_SERVER_USE);
static OLESTATUS (FAR PASCAL *olesvrOleRegisterServerDoc)(LHSERVER, LPCSTR, LPOLESERVERDOC, LHSERVERDOC FAR*);
static OLESTATUS (FAR PASCAL *olesvrOleRevokeObject)(LPOLECLIENT);


static void aliasOleObject( LPOLEOBJECT lpobject, LPOLEOBJECTVTBL _FAR *olpvtbl )
{
    LPOLEOBJECTVTBL     nlpvtbl;

    nlpvtbl = (*olpvtbl) = lpobject->lpvtbl;
    GetAlias( &nlpvtbl );
    lpobject->lpvtbl = nlpvtbl;

} /* aliasOleObject */

static void releaseAliasOleObject( LPOLEOBJECT lpobject, LPOLEOBJECTVTBL olpvtbl )
{
    ReleaseAlias( olpvtbl, lpobject->lpvtbl );
    lpobject->lpvtbl = olpvtbl;

} /* releaseAliasOleObject */

static void permAliasOleClient( LPOLECLIENT _FAR *lpclient )
{
    LPOLECLIENT         new;
    LPOLECLIENTVTBL     nlpvtbl;

    new = _fmalloc( sizeof( OLEOBJECT ) );
    new->lpvtbl = _fmalloc( sizeof( OLECLIENTVTBL ) );
    nlpvtbl = (*lpclient)->lpvtbl;
    GetAlias( &nlpvtbl );
    _fmemcpy( new->lpvtbl, nlpvtbl, sizeof( OLECLIENTVTBL ) );
    ReleaseAlias( (*lpclient)->lpvtbl, nlpvtbl );

    *lpclient = new;
}

static void aliasOleClient( LPOLECLIENT lpclient, LPOLECLIENTVTBL _FAR *olpvtbl )
{
    LPOLECLIENTVTBL     nlpvtbl;

    nlpvtbl = (*olpvtbl) = lpclient->lpvtbl;
    GetAlias( &nlpvtbl );
    lpclient->lpvtbl = nlpvtbl;

} /* aliasOleClient */

static void releaseAliasOleClient( LPOLECLIENT lpclient, LPOLECLIENTVTBL olpvtbl )
{
    ReleaseAlias( olpvtbl, lpclient->lpvtbl );
    lpclient->lpvtbl = olpvtbl;

} /* releaseAliasOleClient */

static void aliasOleStream( LPOLESTREAM lpstream, LPOLESTREAMVTBL _FAR *olpstbl )
{
    LPOLESTREAMVTBL     nlpstbl;

    nlpstbl = (*olpstbl) = lpstream->lpstbl;
    GetAlias( &nlpstbl );
    lpstream->lpstbl = nlpstbl;

} /* aliasOleStream */

static void releaseAliasOleStream( LPOLESTREAM lpstream, LPOLESTREAMVTBL olpstbl )
{
    ReleaseAlias( olpstbl, lpstream->lpstbl );
    lpstream->lpstbl = olpstbl;

} /* releaseAliasOleStream */

/*
 * __OleActivate - cover function for olecli function OleActivate
 */
OLESTATUS FAR PASCAL __OleActivate(LPOLEOBJECT lpobject, UINT verb, BOOL show,
                BOOL takefocus, HWND hwnd, const LPRECT bound )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleActivate == NULL ) {
        olecliOleActivate = Backpatch_olecli( "OleActivate" );
        if( olecliOleActivate == NULL ) return( 0 );
    }
    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleActivate( lpobject, verb, show, takefocus, hwnd, bound );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleActivate */

/*
 * __OleClone - cover function for olecli function OleClone
 */
OLESTATUS FAR PASCAL __OleClone( LPOLEOBJECT lpobject, LPOLECLIENT lpclient,
                LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR* lplpobj )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;
    LPOLECLIENTVTBL     olpvtblc;

    if( olecliOleClone == NULL ) {
        olecliOleClone = Backpatch_olecli( "OleClone" );
        if( olecliOleClone == NULL ) return( 0 );
    }
    aliasOleObject( lpobject, &olpvtbl );
    aliasOleClient( lpclient, &olpvtblc );

    rc = olecliOleClone( lpobject, lpclient, cdoc, objname, lplpobj );

    releaseAliasOleObject( lpobject, olpvtbl );
    releaseAliasOleClient( lpclient, olpvtblc );

    return( rc );

} /* __OleClone */

/*
 * __OleClose - cover function for olecli function OleClose
 */
OLESTATUS FAR PASCAL __OleClose(LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleClose == NULL ) {
        olecliOleClose = Backpatch_olecli( "OleClose" );
        if( olecliOleClose == NULL ) return( 0 );
    }
    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleClose( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleClose */

/*
 * __OleCopyFromLink - cover function for olecli function OleCopyFromLink
 */
OLESTATUS FAR PASCAL __OleCopyFromLink( LPOLEOBJECT lpobject, LPCSTR protocol,
                LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                LPOLEOBJECT FAR*lplpobj )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;
    LPOLECLIENTVTBL     olpvtblc;

    if( olecliOleCopyFromLink == NULL ) {
        olecliOleCopyFromLink = Backpatch_olecli( "OleCopyFromLink" );
        if( olecliOleCopyFromLink == NULL ) return( 0 );
    }
    aliasOleObject( lpobject, &olpvtbl );
    aliasOleClient( lpclient, &olpvtblc );
    rc = olecliOleCopyFromLink( lpobject, protocol, lpclient, cdoc, objname,
                                lplpobj );
    releaseAliasOleObject( lpobject, olpvtbl );
    releaseAliasOleClient( lpclient, olpvtblc );

    return( rc );

} /* __OleCopyFromLink */

/*
 * __OleCopyToClipboard - cover function for olecli function OleCopyToClipboard
 */
OLESTATUS FAR PASCAL __OleCopyToClipboard(LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleCopyToClipboard == NULL ) {
        olecliOleCopyToClipboard = Backpatch_olecli( "OleCopyToClipboard" );
        if( olecliOleCopyToClipboard == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleCopyToClipboard( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleCopyToClipboard */

/*
 * __OleCreate - cover function for olecli function OleCreate
 */
OLESTATUS FAR PASCAL __OleCreate(LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LHCLIENTDOC cdoc, LPCSTR objname,
                        LPOLEOBJECT FAR*lplpobj, OLEOPT_RENDER renderopt,
                        OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreate == NULL ) {
        olecliOleCreate = Backpatch_olecli( "OleCreate" );
        if( olecliOleCreate == NULL ) return( 0 );
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreate( protocol, lpclient, class, cdoc, objname, lplpobj,
                                renderopt, cfmt );

    return( rc );

} /* __OleCreate */

/*
 * __OleCreateFromClip - cover function for olecli function OleCreateFromClip
 */
OLESTATUS FAR PASCAL __OleCreateFromClip( LPCSTR protocol, LPOLECLIENT lpclient,
                        LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                         OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateFromClip == NULL ) {
        olecliOleCreateFromClip = Backpatch_olecli( "OleCreateFromClip" );
        if( olecliOleCreateFromClip == NULL ) return( 0 );
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateFromClip( protocol, lpclient, cdoc, objname, lplpobj,
                                renderopt, cfmt );

    return( rc );

} /* __OleCreateFromClip */

/*
 * __OleCreateFromFile - cover function for olecli function OleCreateFromFile
 */
OLESTATUS FAR PASCAL __OleCreateFromFile( LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LPCSTR file, LHCLIENTDOC cdoc,
                        LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                        OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateFromFile == NULL ) {
        olecliOleCreateFromFile = Backpatch_olecli( "OleCreateFromFile" );
        if( olecliOleCreateFromFile == NULL ) return( 0 );
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateFromFile( protocol, lpclient, class, file, cdoc,
                                objname, lplpobj, renderopt, cfmt );

    return( rc );

} /* __OleCreateFromFile */

/*
 * __OleCreateFromTemplate - cover function for olecli function OleCreateFromTemplate
 */
OLESTATUS FAR PASCAL __OleCreateFromTemplate( LPCSTR protocol,
                        LPOLECLIENT lpclient, LPCSTR template,
                        LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                        OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateFromTemplate == NULL ) {
        olecliOleCreateFromTemplate = Backpatch_olecli( "OleCreateFromTemplate" );
        if( olecliOleCreateFromTemplate == NULL ) return( 0 );
    }
    permAliasOleClient( &lpclient );
    rc = olecliOleCreateFromTemplate( protocol, lpclient, template, cdoc,
                        objname, lplpobj, renderopt, cfmt );
    return( rc );

} /* __OleCreateFromTemplate */

/*
 * __OleCreateInvisible - cover function for olecli function OleCreateInvisible
 */
OLESTATUS FAR PASCAL __OleCreateInvisible( LPCSTR protocol, LPOLECLIENT lpclient,
                         LPCSTR class, LHCLIENTDOC cdoc, LPCSTR objname,
                         LPOLEOBJECT FAR*lplpobj, OLEOPT_RENDER renderopts,
                         OLECLIPFORMAT cfmt, BOOL activate )
{
    OLESTATUS           rc;

    if( olecliOleCreateInvisible == NULL ) {
        olecliOleCreateInvisible = Backpatch_olecli( "OleCreateInvisible" );
        if( olecliOleCreateInvisible == NULL ) return( 0 );
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateInvisible( protocol, lpclient, class, cdoc, objname,
                lplpobj, renderopts, cfmt, activate );

    return( rc );

} /* __OleCreateInvisible */

/*
 * __OleCreateLinkFromClip - cover function for olecli function OleCreateLinkFromClip
 */
OLESTATUS FAR PASCAL __OleCreateLinkFromClip( LPCSTR protocol,
                                LPOLECLIENT lpclient, LHCLIENTDOC cdoc,
                                LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                                OLEOPT_RENDER renderopts, OLECLIPFORMAT cfmt)
{
    OLESTATUS           rc;

    if( olecliOleCreateLinkFromClip == NULL ) {
        olecliOleCreateLinkFromClip = Backpatch_olecli( "OleCreateLinkFromClip" );
        if( olecliOleCreateLinkFromClip == NULL ) return( 0 );
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateLinkFromClip( protocol, lpclient, cdoc, objname,
                        lplpobj, renderopts, cfmt );

    return( rc );

} /* __OleCreateLinkFromClip */

/*
 * __OleCreateLinkFromFile - cover function for olecli function OleCreateLinkFromFile
 */
OLESTATUS FAR PASCAL __OleCreateLinkFromFile(LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LPCSTR file, LPCSTR item, LHCLIENTDOC cdoc,
                        LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                        OLEOPT_RENDER renderopts, OLECLIPFORMAT cfmt)
{
    OLESTATUS           rc;

    if( olecliOleCreateLinkFromFile == NULL ) {
        olecliOleCreateLinkFromFile = Backpatch_olecli( "OleCreateLinkFromFile" );
        if( olecliOleCreateLinkFromFile == NULL ) return( 0 );
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateLinkFromFile( protocol, lpclient, class, file, item,
                        cdoc, objname, lplpobj, renderopts, cfmt );

    return( rc );

} /* __OleCreateLinkFromFile */

/*
 * __OleDelete - cover function for olecli function OleDelete
 */
OLESTATUS FAR PASCAL __OleDelete(LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleDelete == NULL ) {
        olecliOleDelete = Backpatch_olecli( "OleDelete" );
        if( olecliOleDelete == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleDelete( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleDelete */

/*
 * __OleDraw - cover function for olecli function OleDraw
 */
OLESTATUS FAR PASCAL __OleDraw( LPOLEOBJECT lpobject, HDC hdc,
                                const RECT FAR* bounds, const RECT FAR*wbounds,
                                HDC hdcformat )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleDraw == NULL ) {
        olecliOleDraw = Backpatch_olecli( "OleDraw" );
        if( olecliOleDraw == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleDraw( lpobject, hdc, bounds, wbounds, hdcformat );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleDraw */

/*
 * __OleEnumFormats - cover function for olecli function OleEnumFormats
 */
OLECLIPFORMAT FAR PASCAL __OleEnumFormats(LPOLEOBJECT lpobject,
                                                OLECLIPFORMAT cfmt )
{
    OLECLIPFORMAT       rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleEnumFormats == NULL ) {
        olecliOleEnumFormats = Backpatch_olecli( "OleEnumFormats" );
        if( olecliOleEnumFormats == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleEnumFormats( lpobject, cfmt );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleEnumFormats */

/*
 * __OleEqual - cover function for olecli function OleEqual
 */
OLESTATUS FAR PASCAL __OleEqual( LPOLEOBJECT lpobject1, LPOLEOBJECT lpobject2 )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl1,olpvtbl2;

    if( olecliOleEqual == NULL ) {
        olecliOleEqual = Backpatch_olecli( "OleEqual" );
        if( olecliOleEqual == NULL ) return( 0 );
    }

    aliasOleObject( lpobject1, &olpvtbl1 );
    aliasOleObject( lpobject2, &olpvtbl2 );
    rc = olecliOleEqual( lpobject1, lpobject2 );
    releaseAliasOleObject( lpobject1, olpvtbl1 );
    releaseAliasOleObject( lpobject2, olpvtbl2 );

    return( rc );

} /* __OleEqual */

/*
 * __OleExecute - cover function for olecli function OleExecute
 */
OLESTATUS FAR PASCAL __OleExecute( LPOLEOBJECT lpobject, HGLOBAL cmds, UINT r )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleExecute == NULL ) {
        olecliOleExecute = Backpatch_olecli( "OleExecute" );
        if( olecliOleExecute == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleExecute( lpobject, cmds, r );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleExecute */

/*
 * __OleGetData - cover function for olecli function OleGetData
 */
OLESTATUS FAR PASCAL __OleGetData( LPOLEOBJECT lpobject, OLECLIPFORMAT cfmt,
                        HANDLE FAR*res )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleGetData == NULL ) {
        olecliOleGetData = Backpatch_olecli( "OleGetData" );
        if( olecliOleGetData == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleGetData( lpobject, cfmt, res );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleGetData */

/*
 * __OleGetLinkUpdateOptions - cover function for olecli function OleGetLinkUpdateOptions
 */
OLESTATUS FAR PASCAL __OleGetLinkUpdateOptions( LPOLEOBJECT lpobject,
                                OLEOPT_UPDATE FAR*updateopt)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleGetLinkUpdateOptions == NULL ) {
        olecliOleGetLinkUpdateOptions = Backpatch_olecli( "OleGetLinkUpdateOptions" );
        if( olecliOleGetLinkUpdateOptions == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleGetLinkUpdateOptions( lpobject, updateopt );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleGetLinkUpdateOptions */

/*
 * __OleLoadFromStream - cover function for olecli function OleLoadFromStream
 */
OLESTATUS FAR PASCAL __OleLoadFromStream( LPOLESTREAM lpstream, LPCSTR protocol,
                LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                LPOLEOBJECT FAR* lplpobj )
{
    OLESTATUS           rc;
    LPOLECLIENTVTBL     olpvtblc;
    LPOLESTREAMVTBL     olpstbl;

    if( olecliOleLoadFromStream == NULL ) {
        olecliOleLoadFromStream = Backpatch_olecli( "OleLoadFromStream" );
        if( olecliOleLoadFromStream == NULL ) return( 0 );
    }

    aliasOleClient( lpclient, &olpvtblc );
    aliasOleStream( lpstream, &olpstbl );
    rc = olecliOleLoadFromStream( lpstream, protocol, lpclient, cdoc, objname,
                                        lplpobj );
    releaseAliasOleClient( lpclient, olpvtblc );
    releaseAliasOleStream( lpstream, olpstbl );

    return( rc );

} /* __OleLoadFromStream */

/*
 * __OleLockServer - cover function for olecli function OleLockServer
 */
OLESTATUS FAR PASCAL __OleLockServer( LPOLEOBJECT lpobject, LHSERVER FAR* hsrv)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleLockServer == NULL ) {
        olecliOleLockServer = Backpatch_olecli( "OleLockServer" );
        if( olecliOleLockServer == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleLockServer( lpobject, hsrv );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleLockServer */

/*
 * __OleObjectConvert - cover function for olecli function OleObjectConvert
 */
OLESTATUS FAR PASCAL __OleObjectConvert( LPOLEOBJECT lpobject, LPCSTR protocol,
                        LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                        LPOLEOBJECT FAR* lplpobj )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;
    LPOLECLIENTVTBL     olpvtblc;

    if( olecliOleObjectConvert == NULL ) {
        olecliOleObjectConvert = Backpatch_olecli( "OleObjectConvert" );
        if( olecliOleObjectConvert == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    aliasOleClient( lpclient, &olpvtblc );
    rc = olecliOleObjectConvert( lpobject, protocol, lpclient, cdoc, objname, lplpobj );
    releaseAliasOleObject( lpobject, olpvtbl );
    releaseAliasOleClient( lpclient, olpvtblc );

    return( rc );

} /* __OleObjectConvert */

/*
 * __OleQueryBounds - cover function for olecli function OleQueryBounds
 */
OLESTATUS FAR PASCAL __OleQueryBounds( LPOLEOBJECT lpobject, RECT FAR* bounds)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryBounds == NULL ) {
        olecliOleQueryBounds = Backpatch_olecli( "OleQueryBounds" );
        if( olecliOleQueryBounds == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryBounds( lpobject, bounds );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryBounds */

/*
 * __OleQueryName - cover function for olecli function OleQueryName
 */
OLESTATUS FAR PASCAL __OleQueryName(LPOLEOBJECT lpobject, LPSTR object,
                                UINT FAR*buffsize)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryName == NULL ) {
        olecliOleQueryName = Backpatch_olecli( "OleQueryName" );
        if( olecliOleQueryName == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryName( lpobject, object, buffsize );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryName */

/*
 * __OleQueryOpen - cover function for olecli function OleQueryOpen
 */
OLESTATUS FAR PASCAL __OleQueryOpen( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryOpen == NULL ) {
        olecliOleQueryOpen = Backpatch_olecli( "OleQueryOpen" );
        if( olecliOleQueryOpen == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryOpen( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryOpen */

/*
 * __OleQueryOutOfDate - cover function for olecli function OleQueryOutOfDate
 */
OLESTATUS FAR PASCAL __OleQueryOutOfDate( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryOutOfDate == NULL ) {
        olecliOleQueryOutOfDate = Backpatch_olecli( "OleQueryOutOfDate" );
        if( olecliOleQueryOutOfDate == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryOutOfDate( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryOutOfDate */

/*
 * __OleQueryProtocol - cover function for olecli function OleQueryProtocol
 */
LPVOID FAR PASCAL __OleQueryProtocol( LPOLEOBJECT lpobject, LPCSTR protocol )
{
    LPVOID              rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryProtocol == NULL ) {
        olecliOleQueryProtocol = Backpatch_olecli( "OleQueryProtocol" );
        if( olecliOleQueryProtocol == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryProtocol( lpobject, protocol );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryProtocol */

/*
 * __OleQueryReleaseError - cover function for olecli function OleQueryReleaseError
 */
OLESTATUS FAR PASCAL __OleQueryReleaseError( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryReleaseError == NULL ) {
        olecliOleQueryReleaseError = Backpatch_olecli( "OleQueryReleaseError" );
        if( olecliOleQueryReleaseError == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryReleaseError( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryReleaseError */

/*
 * __OleQueryReleaseMethod - cover function for olecli function OleQueryReleaseMethod
 */
OLE_RELEASE_METHOD FAR PASCAL __OleQueryReleaseMethod( LPOLEOBJECT lpobject )
{
    OLE_RELEASE_METHOD  rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryReleaseMethod == NULL ) {
        olecliOleQueryReleaseMethod = Backpatch_olecli( "OleQueryReleaseMethod" );
        if( olecliOleQueryReleaseMethod == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryReleaseMethod( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryReleaseMethod */

/*
 * __OleQueryReleaseStatus - cover function for olecli function OleQueryReleaseStatus
 */
OLESTATUS FAR PASCAL __OleQueryReleaseStatus( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryReleaseStatus == NULL ) {
        olecliOleQueryReleaseStatus = Backpatch_olecli( "OleQueryReleaseStatus" );
        if( olecliOleQueryReleaseStatus == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryReleaseStatus( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryReleaseStatus */

/*
 * __OleQuerySize - cover function for olecli function OleQuerySize
 */
OLESTATUS FAR PASCAL __OleQuerySize(LPOLEOBJECT lpobject, DWORD FAR*size)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQuerySize == NULL ) {
        olecliOleQuerySize = Backpatch_olecli( "OleQuerySize" );
        if( olecliOleQuerySize == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQuerySize( lpobject, size );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQuerySize */

/*
 * __OleQueryType - cover function for olecli function OleQueryType
 */
OLESTATUS FAR PASCAL __OleQueryType( LPOLEOBJECT lpobject, LONG FAR*type )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleQueryType == NULL ) {
        olecliOleQueryType = Backpatch_olecli( "OleQueryType" );
        if( olecliOleQueryType == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleQueryType( lpobject, type );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleQueryType */

/*
 * __OleReconnect - cover function for olecli function OleReconnect
 */
OLESTATUS FAR PASCAL __OleReconnect( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleReconnect == NULL ) {
        olecliOleReconnect = Backpatch_olecli( "OleReconnect" );
        if( olecliOleReconnect == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleReconnect( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleReconnect */

/*
 * __OleRelease - cover function for olecli function OleRelease
 */
OLESTATUS FAR PASCAL __OleRelease( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleRelease == NULL ) {
        olecliOleRelease = Backpatch_olecli( "OleRelease" );
        if( olecliOleRelease == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleRelease( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleRelease */

/*
 * __OleRename - cover function for olecli function OleRename
 */
OLESTATUS FAR PASCAL __OleRename( LPOLEOBJECT lpobject, LPCSTR newname )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleRename == NULL ) {
        olecliOleRename = Backpatch_olecli( "OleRename" );
        if( olecliOleRename == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleRename( lpobject, newname );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleRename */

/*
 * __OleRequestData - cover function for olecli function OleRequestData
 */
OLESTATUS FAR PASCAL __OleRequestData( LPOLEOBJECT lpobject , OLECLIPFORMAT cfmt)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleRequestData == NULL ) {
        olecliOleRequestData = Backpatch_olecli( "OleRequestData" );
        if( olecliOleRequestData == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleRequestData( lpobject, cfmt );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleRequestData */

/*
 * __OleSaveToStream - cover function for olecli function OleSaveToStream
 */
OLESTATUS FAR PASCAL __OleSaveToStream( LPOLEOBJECT lpobject, LPOLESTREAM lpstream)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;
    LPOLESTREAMVTBL     olpstbl;

    if( olecliOleSaveToStream == NULL ) {
        olecliOleSaveToStream = Backpatch_olecli( "OleSaveToStream" );
        if( olecliOleSaveToStream == NULL ) return( 0 );
    }

    aliasOleStream( lpstream, &olpstbl );
    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSaveToStream( lpobject, lpstream );
    releaseAliasOleObject( lpobject, olpvtbl );
    releaseAliasOleStream( lpstream, olpstbl );

    return( rc );

} /* __OleSaveToStream */

/*
 * __OleSetBounds - cover function for olecli function OleSetBounds
 */
OLESTATUS FAR PASCAL __OleSetBounds(LPOLEOBJECT lpobject, const RECT FAR*bound)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleSetBounds == NULL ) {
        olecliOleSetBounds = Backpatch_olecli( "OleSetBounds" );
        if( olecliOleSetBounds == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSetBounds( lpobject, bound );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleSetBounds */

/*
 * __OleSetColorScheme - cover function for olecli function OleSetColorScheme
 */
OLESTATUS FAR PASCAL __OleSetColorScheme(LPOLEOBJECT lpobject,
                                        const LOGPALETTE FAR*pal )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleSetColorScheme == NULL ) {
        olecliOleSetColorScheme = Backpatch_olecli( "OleSetColorScheme" );
        if( olecliOleSetColorScheme == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSetColorScheme( lpobject, pal);
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleSetColorScheme */

/*
 * __OleSetData - cover function for olecli function OleSetData
 */
OLESTATUS FAR PASCAL __OleSetData( LPOLEOBJECT lpobject, OLECLIPFORMAT cfmt,
                                        HANDLE hdata )
{
    OLESTATUS   rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleSetData == NULL ) {
        olecliOleSetData = Backpatch_olecli( "OleSetData" );
        if( olecliOleSetData == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSetData( lpobject, cfmt, hdata );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleSetData */

/*
 * __OleSetHostNames - cover function for olecli function OleSetHostNames
 */
OLESTATUS FAR PASCAL __OleSetHostNames(LPOLEOBJECT lpobject, LPCSTR client,
                                LPCSTR clientobj )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleSetHostNames == NULL ) {
        olecliOleSetHostNames = Backpatch_olecli( "OleSetHostNames" );
        if( olecliOleSetHostNames == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSetHostNames( lpobject, client, clientobj );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleSetHostNames */

/*
 * __OleSetLinkUpdateOptions - cover function for olecli function OleSetLinkUpdateOptions
 */
OLESTATUS FAR PASCAL __OleSetLinkUpdateOptions( LPOLEOBJECT lpobject,
                                        OLEOPT_UPDATE updateopt)
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleSetLinkUpdateOptions == NULL ) {
        olecliOleSetLinkUpdateOptions = Backpatch_olecli( "OleSetLinkUpdateOptions" );
        if( olecliOleSetLinkUpdateOptions == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSetLinkUpdateOptions( lpobject, updateopt );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleSetLinkUpdateOptions */

/*
 * __OleSetTargetDevice - cover function for olecli function OleSetTargetDevice
 */
OLESTATUS FAR PASCAL __OleSetTargetDevice( LPOLEOBJECT lpobject, HGLOBAL hotd )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleSetTargetDevice == NULL ) {
        olecliOleSetTargetDevice = Backpatch_olecli( "OleSetTargetDevice" );
        if( olecliOleSetTargetDevice == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleSetTargetDevice( lpobject, hotd );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleSetTargetDevice */

/*
 * __OleUpdate - cover function for olecli function OleUpdate
 */
OLESTATUS FAR PASCAL __OleUpdate( LPOLEOBJECT lpobject )
{
    OLESTATUS           rc;
    LPOLEOBJECTVTBL     olpvtbl;

    if( olecliOleUpdate == NULL ) {
        olecliOleUpdate = Backpatch_olecli( "OleUpdate" );
        if( olecliOleUpdate == NULL ) return( 0 );
    }

    aliasOleObject( lpobject, &olpvtbl );
    rc = olecliOleUpdate( lpobject );
    releaseAliasOleObject( lpobject, olpvtbl );

    return( rc );

} /* __OleUpdate */

/*
 * __OleRegisterServer - cover function for olesvr function OleRegisterServer
 */
OLESTATUS FAR PASCAL __OleRegisterServer( LPCSTR class, LPOLESERVER lpserver,
                        LHSERVER FAR*handle, HINSTANCE inst, OLE_SERVER_USE suse)
{
    OLESTATUS           rc;
    LPOLESERVERVTBL     nlpvtbl;
    LPOLESERVER         new;

    if( olesvrOleRegisterServer == NULL ) {
        olesvrOleRegisterServer = Backpatch_olesvr( "OleRegisterServer" );
        if( olesvrOleRegisterServer == NULL ) return( 0 );
    }

    new = _fmalloc( sizeof( OLESERVER ) );
    new->lpvtbl = _fmalloc( sizeof( OLESERVERVTBL ) );

    nlpvtbl = lpserver->lpvtbl;
    GetAlias( &nlpvtbl );
    _fmemcpy( new->lpvtbl, nlpvtbl, sizeof( OLESERVERVTBL ) );
    ReleaseAlias( lpserver->lpvtbl, nlpvtbl );

    rc = olesvrOleRegisterServer( class, new, handle, inst, suse );

    return( rc );

} /* __OleRegisterServer */

/*
 * __OleRegisterServerDoc - cover function for olesvr function OleRegisterServerDoc
 */
OLESTATUS FAR PASCAL __OleRegisterServerDoc(LHSERVER hsrvr, LPCSTR docname,
                        LPOLESERVERDOC lpdoc, LHSERVERDOC FAR* lphdoc)
{
    OLESTATUS           rc;
    LPOLESERVERDOCVTBL  nlpvtbl;
    LPOLESERVERDOC      new;

    if( olesvrOleRegisterServerDoc == NULL ) {
        olesvrOleRegisterServerDoc = Backpatch_olesvr( "OleRegisterServerDoc" );
        if( olesvrOleRegisterServerDoc == NULL ) return( 0 );
    }

    new = _fmalloc( sizeof( OLESERVERDOC ) );
    new->lpvtbl = _fmalloc( sizeof( OLESERVERDOCVTBL ) );

    nlpvtbl = lpdoc->lpvtbl;
    GetAlias( &nlpvtbl );
    _fmemcpy( new->lpvtbl, nlpvtbl, sizeof( OLESERVERDOCVTBL ) );
    ReleaseAlias( lpdoc->lpvtbl, nlpvtbl );

    rc = olesvrOleRegisterServerDoc( hsrvr, docname, new, lphdoc );

    return( rc );

} /* __OleRegisterServerDoc */

/*
 * __OleRevokeObject - cover function for olesvr function OleRevokeObject
 */
OLESTATUS FAR PASCAL __OleRevokeObject( LPOLECLIENT lpclient )
{
    OLESTATUS           rc;
    LPOLECLIENTVTBL     olpvtblc;

    if( olesvrOleRevokeObject == NULL ) {
        olesvrOleRevokeObject = Backpatch_olesvr( "OleRevokeObject" );
        if( olesvrOleRevokeObject == NULL ) return( 0 );
    }

    aliasOleClient( lpclient, &olpvtblc );
    rc = olesvrOleRevokeObject( lpclient );
    releaseAliasOleClient( lpclient, olpvtblc );

    return( rc );

} /* __OleRevokeObject */
