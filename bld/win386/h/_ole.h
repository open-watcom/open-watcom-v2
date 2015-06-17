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


extern OLESTATUS FAR PASCAL __OleActivate(LPOLEOBJECT lpobject, UINT verb, BOOL show,
                BOOL takefocus, HWND hwnd, const LPRECT bound );
extern OLESTATUS FAR PASCAL __OleClone( LPOLEOBJECT lpobject, LPOLECLIENT lpclient,
                LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR* lplpobj );
extern OLESTATUS FAR PASCAL __OleClose(LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleCopyFromLink( LPOLEOBJECT lpobject, LPCSTR protocol,
                LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                LPOLEOBJECT FAR*lplpobj );
extern OLESTATUS FAR PASCAL __OleCopyToClipboard(LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleCreate(LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LHCLIENTDOC cdoc, LPCSTR objname,
                        LPOLEOBJECT FAR*lplpobj, OLEOPT_RENDER renderopt,
                        OLECLIPFORMAT cfmt );
extern OLESTATUS FAR PASCAL __OleCreateFromClip( LPCSTR protocol, LPOLECLIENT lpclient,
                        LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                         OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt );
extern OLESTATUS FAR PASCAL __OleCreateFromFile( LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LPCSTR file, LHCLIENTDOC cdoc,
                        LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                        OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt );
extern OLESTATUS FAR PASCAL __OleCreateFromTemplate( LPCSTR protocol,
                        LPOLECLIENT lpclient, LPCSTR template,
                        LHCLIENTDOC cdoc, LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                        OLEOPT_RENDER renderopt, OLECLIPFORMAT cfmt );
extern OLESTATUS FAR PASCAL __OleCreateInvisible( LPCSTR protocol, LPOLECLIENT lpclient,
                         LPCSTR class, LHCLIENTDOC cdoc, LPCSTR objname,
                         LPOLEOBJECT FAR*lplpobj, OLEOPT_RENDER renderopts,
                         OLECLIPFORMAT cfmt, BOOL activate );
extern OLESTATUS FAR PASCAL __OleCreateLinkFromClip( LPCSTR protocol,
                                LPOLECLIENT lpclient, LHCLIENTDOC cdoc,
                                LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                                OLEOPT_RENDER renderopts, OLECLIPFORMAT cfmt);
extern OLESTATUS FAR PASCAL __OleCreateLinkFromFile(LPCSTR protocol, LPOLECLIENT lpclient,
                        LPCSTR class, LPCSTR file, LPCSTR item, LHCLIENTDOC cdoc,
                        LPCSTR objname, LPOLEOBJECT FAR*lplpobj,
                        OLEOPT_RENDER renderopts, OLECLIPFORMAT cfmt);
extern OLESTATUS FAR PASCAL __OleDelete(LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleDraw( LPOLEOBJECT lpobject, HDC hdc,
                                const RECT FAR* bounds, const RECT FAR*wbounds,
                                HDC hdcformat );
extern OLECLIPFORMAT FAR PASCAL __OleEnumFormats(LPOLEOBJECT lpobject,
                                                OLECLIPFORMAT cfmt );
extern OLESTATUS FAR PASCAL __OleEqual( LPOLEOBJECT lpobject1, LPOLEOBJECT lpobject2 );
extern OLESTATUS FAR PASCAL __OleExecute( LPOLEOBJECT lpobject, HGLOBAL cmds, UINT r );
extern OLESTATUS FAR PASCAL __OleGetData( LPOLEOBJECT lpobject, OLECLIPFORMAT cfmt,
                        HANDLE FAR*res );
extern OLESTATUS FAR PASCAL __OleGetLinkUpdateOptions( LPOLEOBJECT lpobject,
                                OLEOPT_UPDATE FAR*updateopt);
extern OLESTATUS FAR PASCAL __OleLoadFromStream( LPOLESTREAM lpstream, LPCSTR protocol,
                LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                LPOLEOBJECT FAR* lplpobj );
extern OLESTATUS FAR PASCAL __OleLockServer( LPOLEOBJECT lpobject, LHSERVER FAR* hsrv);
extern OLESTATUS FAR PASCAL __OleObjectConvert( LPOLEOBJECT lpobject, LPCSTR protocol,
                        LPOLECLIENT lpclient, LHCLIENTDOC cdoc, LPCSTR objname,
                        LPOLEOBJECT FAR* lplpobj );
extern OLESTATUS FAR PASCAL __OleQueryBounds( LPOLEOBJECT lpobject, RECT FAR* bounds);
extern OLESTATUS FAR PASCAL __OleQueryName(LPOLEOBJECT lpobject, LPSTR object,
                                UINT FAR*buffsize);
extern OLESTATUS FAR PASCAL __OleQueryOpen( LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleQueryOutOfDate( LPOLEOBJECT lpobject );
extern LPVOID FAR PASCAL __OleQueryProtocol( LPOLEOBJECT lpobject, LPCSTR protocol );
extern OLESTATUS FAR PASCAL __OleQueryReleaseError( LPOLEOBJECT lpobject );
extern OLE_RELEASE_METHOD FAR PASCAL __OleQueryReleaseMethod( LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleQueryReleaseStatus( LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleQuerySize(LPOLEOBJECT lpobject, DWORD FAR*size);
extern OLESTATUS FAR PASCAL __OleQueryType( LPOLEOBJECT lpobject, LONG FAR*type );
extern OLESTATUS FAR PASCAL __OleReconnect( LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleRelease( LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleRename( LPOLEOBJECT lpobject, LPCSTR newname );
extern OLESTATUS FAR PASCAL __OleRequestData( LPOLEOBJECT lpobject , OLECLIPFORMAT cfmt);
extern OLESTATUS FAR PASCAL __OleSaveToStream( LPOLEOBJECT lpobject, LPOLESTREAM lpstream);
extern OLESTATUS FAR PASCAL __OleSetBounds(LPOLEOBJECT lpobject, const RECT FAR*bound);
extern OLESTATUS FAR PASCAL __OleSetColorScheme(LPOLEOBJECT lpobject,
                                        const LOGPALETTE FAR*pal );
extern OLESTATUS FAR PASCAL __OleSetData( LPOLEOBJECT lpobject, OLECLIPFORMAT cfmt,
                                        HANDLE hdata );
extern OLESTATUS FAR PASCAL __OleSetHostNames(LPOLEOBJECT lpobject, LPCSTR client,
                                LPCSTR clientobj );
extern OLESTATUS FAR PASCAL __OleSetLinkUpdateOptions( LPOLEOBJECT lpobject,
                                        OLEOPT_UPDATE updateopt);
extern OLESTATUS FAR PASCAL __OleSetTargetDevice( LPOLEOBJECT lpobject, HGLOBAL hotd );
extern OLESTATUS FAR PASCAL __OleUpdate( LPOLEOBJECT lpobject );
extern OLESTATUS FAR PASCAL __OleRegisterServer( LPCSTR class, LPOLESERVER lpserver,
                        LHSERVER FAR*handle, HINSTANCE inst, OLE_SERVER_USE suse);
extern OLESTATUS FAR PASCAL __OleRegisterServerDoc(LHSERVER hsrvr, LPCSTR docname,
                        LPOLESERVERDOC lpdoc, LHSERVERDOC FAR* lphdoc);
extern OLESTATUS FAR PASCAL __OleRevokeObject( LPOLECLIENT lpclient );
