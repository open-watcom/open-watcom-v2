/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <malloc.h>
#include <dos.h>
#include <windows.h>
#include <ole.h>
#include "winext.h"
#include "winstubs.h"
#include "_ole.h"

extern LPVOID FAR BackPatch_olecli( char *str );
#pragma aux BackPatch_olecli __parm [__ax]

extern LPVOID FAR BackPatch_olesvr( char *str );
#pragma aux BackPatch_olesvr __parm [__ax]

static OLESTATUS (FAR PASCAL *olecliOleActivate)( LPOLEOBJECT, UINT, BOOL, BOOL, HWND, const RECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleClone)( LPOLEOBJECT, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleClose)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleCopyFromLink)( LPOLEOBJECT, LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleCopyToClipboard)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleCreate)( LPCSTR, LPOLECLIENT, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleCreateFromClip)( LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleCreateFromFile)( LPCSTR, LPOLECLIENT, LPCSTR, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleCreateFromTemplate)( LPCSTR, LPOLECLIENT, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleCreateInvisible)( LPCSTR, LPOLECLIENT, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT, BOOL );
static OLESTATUS (FAR PASCAL *olecliOleCreateLinkFromClip)( LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleCreateLinkFromFile)( LPCSTR, LPOLECLIENT, LPCSTR, LPCSTR, LPCSTR, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR *, OLEOPT_RENDER, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleDelete)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleDraw)( LPOLEOBJECT, HDC, const RECT FAR *, const RECT FAR *, HDC );
static OLECLIPFORMAT (FAR PASCAL *olecliOleEnumFormats)( LPOLEOBJECT, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleEqual)( LPOLEOBJECT, LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleExecute)( LPOLEOBJECT, HGLOBAL, UINT );
static OLESTATUS (FAR PASCAL *olecliOleGetData)( LPOLEOBJECT, OLECLIPFORMAT, HANDLE FAR * );
static OLESTATUS (FAR PASCAL *olecliOleGetLinkUpdateOptions)( LPOLEOBJECT, OLEOPT_UPDATE FAR * );
static OLESTATUS (FAR PASCAL *olecliOleLoadFromStream)( LPOLESTREAM, LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleLockServer)( LPOLEOBJECT, LHSERVER FAR * );
static OLESTATUS (FAR PASCAL *olecliOleObjectConvert)( LPOLEOBJECT, LPCSTR, LPOLECLIENT, LHCLIENTDOC, LPCSTR, LPOLEOBJECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleQueryBounds)( LPOLEOBJECT, RECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleQueryName)( LPOLEOBJECT, LPSTR, UINT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleQueryOpen)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleQueryOutOfDate)( LPOLEOBJECT );
static LPVOID (FAR PASCAL *olecliOleQueryProtocol)( LPOLEOBJECT, LPCSTR );
static OLESTATUS (FAR PASCAL *olecliOleQueryReleaseError)( LPOLEOBJECT );
static OLE_RELEASE_METHOD (FAR PASCAL *olecliOleQueryReleaseMethod)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleQueryReleaseStatus)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleQuerySize)( LPOLEOBJECT, DWORD FAR * );
static OLESTATUS (FAR PASCAL *olecliOleQueryType)( LPOLEOBJECT, LONG FAR * );
static OLESTATUS (FAR PASCAL *olecliOleReconnect)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleRelease)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olecliOleRename)( LPOLEOBJECT, LPCSTR );
static OLESTATUS (FAR PASCAL *olecliOleRequestData)( LPOLEOBJECT, OLECLIPFORMAT );
static OLESTATUS (FAR PASCAL *olecliOleSaveToStream)( LPOLEOBJECT, LPOLESTREAM );
static OLESTATUS (FAR PASCAL *olecliOleSetBounds)( LPOLEOBJECT, const RECT FAR * );
static OLESTATUS (FAR PASCAL *olecliOleSetColorScheme)( LPOLEOBJECT, const LOGPALETTE FAR * );
static OLESTATUS (FAR PASCAL *olecliOleSetData)( LPOLEOBJECT, OLECLIPFORMAT, HANDLE );
static OLESTATUS (FAR PASCAL *olecliOleSetHostNames)( LPOLEOBJECT, LPCSTR, LPCSTR );
static OLESTATUS (FAR PASCAL *olecliOleSetLinkUpdateOptions)( LPOLEOBJECT, OLEOPT_UPDATE );
static OLESTATUS (FAR PASCAL *olecliOleSetTargetDevice)( LPOLEOBJECT, HGLOBAL );
static OLESTATUS (FAR PASCAL *olecliOleUpdate)( LPOLEOBJECT );
static OLESTATUS (FAR PASCAL *olesvrOleRegisterServer)( LPCSTR, LPOLESERVER, LHSERVER FAR *, HINSTANCE, OLE_SERVER_USE );
static OLESTATUS (FAR PASCAL *olesvrOleRegisterServerDoc)( LHSERVER, LPCSTR, LPOLESERVERDOC, LHSERVERDOC FAR * );
static OLESTATUS (FAR PASCAL *olesvrOleRevokeObject)( LPOLECLIENT );


static void permAliasOleClient( LPOLECLIENT _DLLFAR *lpclient )
{
    LPOLECLIENT     new;
    DWORD           odata;

    new = _fmalloc( sizeof( OLECLIENT ) );
    new->lpvtbl = _fmalloc( sizeof( OLECLIENTVTBL ) );
    odata = GETALIAS( &(*lpclient)->lpvtbl );
    _fmemcpy( new->lpvtbl, (*lpclient)->lpvtbl, sizeof( OLECLIENTVTBL ) );
    RELEASEALIAS( &(*lpclient)->lpvtbl, odata );
    *lpclient = new;
}

/*
 * __OleActivate - cover function for olecli function OleActivate
 */
OLESTATUS FAR PASCAL __OleActivate(LPOLEOBJECT lpobject, UINT verb, BOOL show,
                BOOL takefocus, HWND hwnd, const LPRECT bound )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleActivate == NULL ) {
        olecliOleActivate = BackPatch_olecli( "OleActivate" );
        if( olecliOleActivate == NULL ) {
            return( 0 );
        }
    }
    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleActivate( lpobject, verb, show, takefocus, hwnd, bound );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleActivate */

/*
 * __OleClone - cover function for olecli function OleClone
 */
OLESTATUS FAR PASCAL __OleClone( LPOLEOBJECT lpobject, LPOLECLIENT lpclient,
                LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR *lplpobj )
{
    OLESTATUS       rc;
    DWORD           odata1;
    DWORD           odata2;

    if( olecliOleClone == NULL ) {
        olecliOleClone = BackPatch_olecli( "OleClone" );
        if( olecliOleClone == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &lpobject->lpvtbl );
    odata2 = GETALIAS( &lpclient->lpvtbl );
    rc = olecliOleClone( lpobject, lpclient, cdoc, objname, lplpobj );
    RELEASEALIAS( &lpclient->lpvtbl, odata2 );
    RELEASEALIAS( &lpobject->lpvtbl, odata1 );

    return( rc );

} /* __OleClone */

/*
 * __OleClose - cover function for olecli function OleClose
 */
OLESTATUS FAR PASCAL __OleClose( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleClose == NULL ) {
        olecliOleClose = BackPatch_olecli( "OleClose" );
        if( olecliOleClose == NULL ) {
            return( 0 );
        }
    }
    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleClose( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleClose */

/*
 * __OleCopyFromLink - cover function for olecli function OleCopyFromLink
 */
OLESTATUS FAR PASCAL __OleCopyFromLink( LPOLEOBJECT lpobject, LPCSTR protocol,
                LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                LPOLEOBJECT FAR *lplpobj )
{
    OLESTATUS       rc;
    DWORD           odata1;
    DWORD           odata2;

    if( olecliOleCopyFromLink == NULL ) {
        olecliOleCopyFromLink = BackPatch_olecli( "OleCopyFromLink" );
        if( olecliOleCopyFromLink == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &lpobject->lpvtbl );
    odata2 = GETALIAS( &lpclient->lpvtbl );
    rc = olecliOleCopyFromLink( lpobject, protocol, lpclient, cdoc, objname, lplpobj );
    RELEASEALIAS( &lpclient->lpvtbl, odata2 );
    RELEASEALIAS( &lpobject->lpvtbl, odata1 );

    return( rc );

} /* __OleCopyFromLink */

/*
 * __OleCopyToClipboard - cover function for olecli function OleCopyToClipboard
 */
OLESTATUS FAR PASCAL __OleCopyToClipboard( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleCopyToClipboard == NULL ) {
        olecliOleCopyToClipboard = BackPatch_olecli( "OleCopyToClipboard" );
        if( olecliOleCopyToClipboard == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleCopyToClipboard( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleCopyToClipboard */

/*
 * __OleCreate - cover function for olecli function OleCreate
 */
OLESTATUS FAR PASCAL __OleCreate( LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LHCLIENTDOC cdoc, LPCSTR objname,
                        LPOLEOBJECT FAR *lplpobj, OLEOPT_RENDER renderopt,
                        OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreate == NULL ) {
        olecliOleCreate = BackPatch_olecli( "OleCreate" );
        if( olecliOleCreate == NULL ) {
            return( 0 );
        }
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
                        LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR *lplpobj,
                         OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateFromClip == NULL ) {
        olecliOleCreateFromClip = BackPatch_olecli( "OleCreateFromClip" );
        if( olecliOleCreateFromClip == NULL ) {
            return( 0 );
        }
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
                        LPCSTR objname, LPOLEOBJECT FAR *lplpobj,
                        OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateFromFile == NULL ) {
        olecliOleCreateFromFile = BackPatch_olecli( "OleCreateFromFile" );
        if( olecliOleCreateFromFile == NULL ) {
            return( 0 );
        }
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
                        LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR *lplpobj,
                        OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateFromTemplate == NULL ) {
        olecliOleCreateFromTemplate = BackPatch_olecli( "OleCreateFromTemplate" );
        if( olecliOleCreateFromTemplate == NULL ) {
            return( 0 );
        }
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
                         LPOLEOBJECT FAR *lplpobj, OLEOPT_RENDER renderopts,
                         OLECLIPFORMAT cfmt, BOOL activate )
{
    OLESTATUS           rc;

    if( olecliOleCreateInvisible == NULL ) {
        olecliOleCreateInvisible = BackPatch_olecli( "OleCreateInvisible" );
        if( olecliOleCreateInvisible == NULL ) {
            return( 0 );
        }
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
                                LPCSTR objname, LPOLEOBJECT FAR *lplpobj,
                                OLEOPT_RENDER renderopts, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateLinkFromClip == NULL ) {
        olecliOleCreateLinkFromClip = BackPatch_olecli( "OleCreateLinkFromClip" );
        if( olecliOleCreateLinkFromClip == NULL ) {
            return( 0 );
        }
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateLinkFromClip( protocol, lpclient, cdoc, objname,
                        lplpobj, renderopts, cfmt );

    return( rc );

} /* __OleCreateLinkFromClip */

/*
 * __OleCreateLinkFromFile - cover function for olecli function OleCreateLinkFromFile
 */
OLESTATUS FAR PASCAL __OleCreateLinkFromFile( LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LPCSTR file, LPCSTR item, LHCLIENTDOC cdoc,
                        LPCSTR objname, LPOLEOBJECT FAR *lplpobj,
                        OLEOPT_RENDER renderopts, OLECLIPFORMAT cfmt )
{
    OLESTATUS           rc;

    if( olecliOleCreateLinkFromFile == NULL ) {
        olecliOleCreateLinkFromFile = BackPatch_olecli( "OleCreateLinkFromFile" );
        if( olecliOleCreateLinkFromFile == NULL ) {
            return( 0 );
        }
    }

    permAliasOleClient( &lpclient );
    rc = olecliOleCreateLinkFromFile( protocol, lpclient, class, file, item,
                        cdoc, objname, lplpobj, renderopts, cfmt );

    return( rc );

} /* __OleCreateLinkFromFile */

/*
 * __OleDelete - cover function for olecli function OleDelete
 */
OLESTATUS FAR PASCAL __OleDelete( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleDelete == NULL ) {
        olecliOleDelete = BackPatch_olecli( "OleDelete" );
        if( olecliOleDelete == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleDelete( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleDelete */

/*
 * __OleDraw - cover function for olecli function OleDraw
 */
OLESTATUS FAR PASCAL __OleDraw( LPOLEOBJECT lpobject, HDC hdc,
                                const RECT FAR *bounds, const RECT FAR *wbounds,
                                HDC hdcformat )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleDraw == NULL ) {
        olecliOleDraw = BackPatch_olecli( "OleDraw" );
        if( olecliOleDraw == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleDraw( lpobject, hdc, bounds, wbounds, hdcformat );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleDraw */

/*
 * __OleEnumFormats - cover function for olecli function OleEnumFormats
 */
OLECLIPFORMAT FAR PASCAL __OleEnumFormats( LPOLEOBJECT lpobject,
                                                OLECLIPFORMAT cfmt )
{
    OLECLIPFORMAT   rc;
    DWORD           odata;

    if( olecliOleEnumFormats == NULL ) {
        olecliOleEnumFormats = BackPatch_olecli( "OleEnumFormats" );
        if( olecliOleEnumFormats == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleEnumFormats( lpobject, cfmt );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleEnumFormats */

/*
 * __OleEqual - cover function for olecli function OleEqual
 */
OLESTATUS FAR PASCAL __OleEqual( LPOLEOBJECT lpobject1, LPOLEOBJECT lpobject2 )
{
    OLESTATUS       rc;
    DWORD           odata1;
    DWORD           odata2;

    if( olecliOleEqual == NULL ) {
        olecliOleEqual = BackPatch_olecli( "OleEqual" );
        if( olecliOleEqual == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpobject1->lpvtbl );
    odata2 = GETALIAS( &lpobject2->lpvtbl );
    rc = olecliOleEqual( lpobject1, lpobject2 );
    RELEASEALIAS( &lpobject2->lpvtbl, odata2 );
    RELEASEALIAS( &lpobject1->lpvtbl, odata1 );

    return( rc );

} /* __OleEqual */

/*
 * __OleExecute - cover function for olecli function OleExecute
 */
OLESTATUS FAR PASCAL __OleExecute( LPOLEOBJECT lpobject, HGLOBAL cmds, UINT r )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleExecute == NULL ) {
        olecliOleExecute = BackPatch_olecli( "OleExecute" );
        if( olecliOleExecute == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleExecute( lpobject, cmds, r );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleExecute */

/*
 * __OleGetData - cover function for olecli function OleGetData
 */
OLESTATUS FAR PASCAL __OleGetData( LPOLEOBJECT lpobject, OLECLIPFORMAT cfmt,
                        HANDLE FAR *res )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleGetData == NULL ) {
        olecliOleGetData = BackPatch_olecli( "OleGetData" );
        if( olecliOleGetData == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleGetData( lpobject, cfmt, res );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleGetData */

/*
 * __OleGetLinkUpdateOptions - cover function for olecli function OleGetLinkUpdateOptions
 */
OLESTATUS FAR PASCAL __OleGetLinkUpdateOptions( LPOLEOBJECT lpobject,
                                OLEOPT_UPDATE FAR *updateopt )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleGetLinkUpdateOptions == NULL ) {
        olecliOleGetLinkUpdateOptions = BackPatch_olecli( "OleGetLinkUpdateOptions" );
        if( olecliOleGetLinkUpdateOptions == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleGetLinkUpdateOptions( lpobject, updateopt );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleGetLinkUpdateOptions */

/*
 * __OleLoadFromStream - cover function for olecli function OleLoadFromStream
 */
OLESTATUS FAR PASCAL __OleLoadFromStream( LPOLESTREAM lpstream, LPCSTR protocol,
                LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                LPOLEOBJECT FAR *lplpobj )
{
    OLESTATUS       rc;
    DWORD           odata1;
    DWORD           odata2;

    if( olecliOleLoadFromStream == NULL ) {
        olecliOleLoadFromStream = BackPatch_olecli( "OleLoadFromStream" );
        if( olecliOleLoadFromStream == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpclient->lpvtbl );
    odata2 = GETALIAS( &lpstream->lpstbl );
    rc = olecliOleLoadFromStream( lpstream, protocol, lpclient, cdoc, objname, lplpobj );
    RELEASEALIAS( &lpstream->lpstbl, odata2 );
    RELEASEALIAS( &lpclient->lpvtbl, odata1 );

    return( rc );

} /* __OleLoadFromStream */

/*
 * __OleLockServer - cover function for olecli function OleLockServer
 */
OLESTATUS FAR PASCAL __OleLockServer( LPOLEOBJECT lpobject, LHSERVER FAR *hsrv )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleLockServer == NULL ) {
        olecliOleLockServer = BackPatch_olecli( "OleLockServer" );
        if( olecliOleLockServer == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleLockServer( lpobject, hsrv );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleLockServer */

/*
 * __OleObjectConvert - cover function for olecli function OleObjectConvert
 */
OLESTATUS FAR PASCAL __OleObjectConvert( LPOLEOBJECT lpobject, LPCSTR protocol,
                        LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                        LPOLEOBJECT FAR *lplpobj )
{
    OLESTATUS       rc;
    DWORD           odata1;
    DWORD           odata2;

    if( olecliOleObjectConvert == NULL ) {
        olecliOleObjectConvert = BackPatch_olecli( "OleObjectConvert" );
        if( olecliOleObjectConvert == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpobject->lpvtbl );
    odata2 = GETALIAS( &lpclient->lpvtbl );
    rc = olecliOleObjectConvert( lpobject, protocol, lpclient, cdoc, objname, lplpobj );
    RELEASEALIAS( &lpclient->lpvtbl, odata2 );
    RELEASEALIAS( &lpobject->lpvtbl, odata1 );

    return( rc );

} /* __OleObjectConvert */

/*
 * __OleQueryBounds - cover function for olecli function OleQueryBounds
 */
OLESTATUS FAR PASCAL __OleQueryBounds( LPOLEOBJECT lpobject, RECT FAR *bounds )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryBounds == NULL ) {
        olecliOleQueryBounds = BackPatch_olecli( "OleQueryBounds" );
        if( olecliOleQueryBounds == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryBounds( lpobject, bounds );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryBounds */

/*
 * __OleQueryName - cover function for olecli function OleQueryName
 */
OLESTATUS FAR PASCAL __OleQueryName( LPOLEOBJECT lpobject, LPSTR object,
                                UINT FAR *buffsize )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryName == NULL ) {
        olecliOleQueryName = BackPatch_olecli( "OleQueryName" );
        if( olecliOleQueryName == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryName( lpobject, object, buffsize );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryName */

/*
 * __OleQueryOpen - cover function for olecli function OleQueryOpen
 */
OLESTATUS FAR PASCAL __OleQueryOpen( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryOpen == NULL ) {
        olecliOleQueryOpen = BackPatch_olecli( "OleQueryOpen" );
        if( olecliOleQueryOpen == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryOpen( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryOpen */

/*
 * __OleQueryOutOfDate - cover function for olecli function OleQueryOutOfDate
 */
OLESTATUS FAR PASCAL __OleQueryOutOfDate( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryOutOfDate == NULL ) {
        olecliOleQueryOutOfDate = BackPatch_olecli( "OleQueryOutOfDate" );
        if( olecliOleQueryOutOfDate == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryOutOfDate( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryOutOfDate */

/*
 * __OleQueryProtocol - cover function for olecli function OleQueryProtocol
 */
LPVOID FAR PASCAL __OleQueryProtocol( LPOLEOBJECT lpobject, LPCSTR protocol )
{
    LPVOID          rc;
    DWORD           odata;

    if( olecliOleQueryProtocol == NULL ) {
        olecliOleQueryProtocol = BackPatch_olecli( "OleQueryProtocol" );
        if( olecliOleQueryProtocol == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryProtocol( lpobject, protocol );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryProtocol */

/*
 * __OleQueryReleaseError - cover function for olecli function OleQueryReleaseError
 */
OLESTATUS FAR PASCAL __OleQueryReleaseError( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryReleaseError == NULL ) {
        olecliOleQueryReleaseError = BackPatch_olecli( "OleQueryReleaseError" );
        if( olecliOleQueryReleaseError == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryReleaseError( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryReleaseError */

/*
 * __OleQueryReleaseMethod - cover function for olecli function OleQueryReleaseMethod
 */
OLE_RELEASE_METHOD FAR PASCAL __OleQueryReleaseMethod( LPOLEOBJECT lpobject )
{
    OLE_RELEASE_METHOD  rc;
    DWORD               odata;

    if( olecliOleQueryReleaseMethod == NULL ) {
        olecliOleQueryReleaseMethod = BackPatch_olecli( "OleQueryReleaseMethod" );
        if( olecliOleQueryReleaseMethod == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryReleaseMethod( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryReleaseMethod */

/*
 * __OleQueryReleaseStatus - cover function for olecli function OleQueryReleaseStatus
 */
OLESTATUS FAR PASCAL __OleQueryReleaseStatus( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryReleaseStatus == NULL ) {
        olecliOleQueryReleaseStatus = BackPatch_olecli( "OleQueryReleaseStatus" );
        if( olecliOleQueryReleaseStatus == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryReleaseStatus( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryReleaseStatus */

/*
 * __OleQuerySize - cover function for olecli function OleQuerySize
 */
OLESTATUS FAR PASCAL __OleQuerySize( LPOLEOBJECT lpobject, DWORD FAR *size )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQuerySize == NULL ) {
        olecliOleQuerySize = BackPatch_olecli( "OleQuerySize" );
        if( olecliOleQuerySize == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQuerySize( lpobject, size );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQuerySize */

/*
 * __OleQueryType - cover function for olecli function OleQueryType
 */
OLESTATUS FAR PASCAL __OleQueryType( LPOLEOBJECT lpobject, LONG FAR *type )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleQueryType == NULL ) {
        olecliOleQueryType = BackPatch_olecli( "OleQueryType" );
        if( olecliOleQueryType == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleQueryType( lpobject, type );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleQueryType */

/*
 * __OleReconnect - cover function for olecli function OleReconnect
 */
OLESTATUS FAR PASCAL __OleReconnect( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleReconnect == NULL ) {
        olecliOleReconnect = BackPatch_olecli( "OleReconnect" );
        if( olecliOleReconnect == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleReconnect( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleReconnect */

/*
 * __OleRelease - cover function for olecli function OleRelease
 */
OLESTATUS FAR PASCAL __OleRelease( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleRelease == NULL ) {
        olecliOleRelease = BackPatch_olecli( "OleRelease" );
        if( olecliOleRelease == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleRelease( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleRelease */

/*
 * __OleRename - cover function for olecli function OleRename
 */
OLESTATUS FAR PASCAL __OleRename( LPOLEOBJECT lpobject, LPCSTR newname )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleRename == NULL ) {
        olecliOleRename = BackPatch_olecli( "OleRename" );
        if( olecliOleRename == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleRename( lpobject, newname );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleRename */

/*
 * __OleRequestData - cover function for olecli function OleRequestData
 */
OLESTATUS FAR PASCAL __OleRequestData( LPOLEOBJECT lpobject , OLECLIPFORMAT cfmt )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleRequestData == NULL ) {
        olecliOleRequestData = BackPatch_olecli( "OleRequestData" );
        if( olecliOleRequestData == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleRequestData( lpobject, cfmt );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleRequestData */

/*
 * __OleSaveToStream - cover function for olecli function OleSaveToStream
 */
OLESTATUS FAR PASCAL __OleSaveToStream( LPOLEOBJECT lpobject, LPOLESTREAM lpstream )
{
    OLESTATUS       rc;
    DWORD           odata1;
    DWORD           odata2;

    if( olecliOleSaveToStream == NULL ) {
        olecliOleSaveToStream = BackPatch_olecli( "OleSaveToStream" );
        if( olecliOleSaveToStream == NULL ) {
            return( 0 );
        }
    }

    odata1 = GETALIAS( &lpstream->lpstbl );
    odata2 = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSaveToStream( lpobject, lpstream );
    RELEASEALIAS( &lpobject->lpvtbl, odata2 );
    RELEASEALIAS( &lpstream->lpstbl, odata1 );

    return( rc );

} /* __OleSaveToStream */

/*
 * __OleSetBounds - cover function for olecli function OleSetBounds
 */
OLESTATUS FAR PASCAL __OleSetBounds( LPOLEOBJECT lpobject, const RECT FAR *bound )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleSetBounds == NULL ) {
        olecliOleSetBounds = BackPatch_olecli( "OleSetBounds" );
        if( olecliOleSetBounds == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSetBounds( lpobject, bound );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleSetBounds */

/*
 * __OleSetColorScheme - cover function for olecli function OleSetColorScheme
 */
OLESTATUS FAR PASCAL __OleSetColorScheme( LPOLEOBJECT lpobject,
                                        const LOGPALETTE FAR *pal )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleSetColorScheme == NULL ) {
        olecliOleSetColorScheme = BackPatch_olecli( "OleSetColorScheme" );
        if( olecliOleSetColorScheme == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSetColorScheme( lpobject, pal);
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleSetColorScheme */

/*
 * __OleSetData - cover function for olecli function OleSetData
 */
OLESTATUS FAR PASCAL __OleSetData( LPOLEOBJECT lpobject, OLECLIPFORMAT cfmt,
                                        HANDLE hdata )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleSetData == NULL ) {
        olecliOleSetData = BackPatch_olecli( "OleSetData" );
        if( olecliOleSetData == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSetData( lpobject, cfmt, hdata );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleSetData */

/*
 * __OleSetHostNames - cover function for olecli function OleSetHostNames
 */
OLESTATUS FAR PASCAL __OleSetHostNames( LPOLEOBJECT lpobject, LPCSTR client,
                                LPCSTR clientobj )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleSetHostNames == NULL ) {
        olecliOleSetHostNames = BackPatch_olecli( "OleSetHostNames" );
        if( olecliOleSetHostNames == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSetHostNames( lpobject, client, clientobj );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleSetHostNames */

/*
 * __OleSetLinkUpdateOptions - cover function for olecli function OleSetLinkUpdateOptions
 */
OLESTATUS FAR PASCAL __OleSetLinkUpdateOptions( LPOLEOBJECT lpobject,
                                        OLEOPT_UPDATE updateopt )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleSetLinkUpdateOptions == NULL ) {
        olecliOleSetLinkUpdateOptions = BackPatch_olecli( "OleSetLinkUpdateOptions" );
        if( olecliOleSetLinkUpdateOptions == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSetLinkUpdateOptions( lpobject, updateopt );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleSetLinkUpdateOptions */

/*
 * __OleSetTargetDevice - cover function for olecli function OleSetTargetDevice
 */
OLESTATUS FAR PASCAL __OleSetTargetDevice( LPOLEOBJECT lpobject, HGLOBAL hotd )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleSetTargetDevice == NULL ) {
        olecliOleSetTargetDevice = BackPatch_olecli( "OleSetTargetDevice" );
        if( olecliOleSetTargetDevice == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleSetTargetDevice( lpobject, hotd );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleSetTargetDevice */

/*
 * __OleUpdate - cover function for olecli function OleUpdate
 */
OLESTATUS FAR PASCAL __OleUpdate( LPOLEOBJECT lpobject )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olecliOleUpdate == NULL ) {
        olecliOleUpdate = BackPatch_olecli( "OleUpdate" );
        if( olecliOleUpdate == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpobject->lpvtbl );
    rc = olecliOleUpdate( lpobject );
    RELEASEALIAS( &lpobject->lpvtbl, odata );

    return( rc );

} /* __OleUpdate */

/*
 * __OleRegisterServer - cover function for olesvr function OleRegisterServer
 */
OLESTATUS FAR PASCAL __OleRegisterServer( LPCSTR class, LPOLESERVER lpserver,
                        LHSERVER FAR *handle, HINSTANCE inst, OLE_SERVER_USE suse )
{
    OLESTATUS           rc;
    DWORD               odata;
    LPOLESERVER         new;

    if( olesvrOleRegisterServer == NULL ) {
        olesvrOleRegisterServer = BackPatch_olesvr( "OleRegisterServer" );
        if( olesvrOleRegisterServer == NULL ) {
            return( 0 );
        }
    }

    new = _fmalloc( sizeof( OLESERVER ) );
    new->lpvtbl = _fmalloc( sizeof( OLESERVERVTBL ) );
    odata = GETALIAS( &lpserver->lpvtbl );
    _fmemcpy( new->lpvtbl, lpserver->lpvtbl, sizeof( OLESERVERVTBL ) );
    RELEASEALIAS( &lpserver->lpvtbl, odata );

    rc = olesvrOleRegisterServer( class, new, handle, inst, suse );

    return( rc );

} /* __OleRegisterServer */


/*
 * __OleRegisterServerDoc - cover function for olesvr function OleRegisterServerDoc
 */
OLESTATUS FAR PASCAL __OleRegisterServerDoc( LHSERVER hsrvr, LPCSTR docname,
                        LPOLESERVERDOC lpdoc, LHSERVERDOC FAR *lphdoc )
{
    OLESTATUS       rc;
    DWORD           odata;
    LPOLESERVERDOC  new;

    if( olesvrOleRegisterServerDoc == NULL ) {
        olesvrOleRegisterServerDoc = BackPatch_olesvr( "OleRegisterServerDoc" );
        if( olesvrOleRegisterServerDoc == NULL ) {
            return( 0 );
        }
    }

    new = _fmalloc( sizeof( OLESERVERDOC ) );
    new->lpvtbl = _fmalloc( sizeof( OLESERVERDOCVTBL ) );
    odata = GETALIAS( &lpdoc->lpvtbl );
    _fmemcpy( new->lpvtbl, lpdoc->lpvtbl, sizeof( OLESERVERDOCVTBL ) );
    RELEASEALIAS( &lpdoc->lpvtbl, odata );

    rc = olesvrOleRegisterServerDoc( hsrvr, docname, new, lphdoc );

    return( rc );

} /* __OleRegisterServerDoc */

/*
 * __OleRevokeObject - cover function for olesvr function OleRevokeObject
 */
OLESTATUS FAR PASCAL __OleRevokeObject( LPOLECLIENT lpclient )
{
    OLESTATUS       rc;
    DWORD           odata;

    if( olesvrOleRevokeObject == NULL ) {
        olesvrOleRevokeObject = BackPatch_olesvr( "OleRevokeObject" );
        if( olesvrOleRevokeObject == NULL ) {
            return( 0 );
        }
    }

    odata = GETALIAS( &lpclient->lpvtbl );
    rc = olesvrOleRevokeObject( lpclient );
    RELEASEALIAS( &lpclient->lpvtbl, odata );

    return( rc );

} /* __OleRevokeObject */
