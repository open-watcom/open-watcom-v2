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


#define INCLUDE_OLE_H
#include "variety.h"
#include "cover.h"

void Ole32InitObjectVtbl( LPOLEOBJECTVTBL povt, BOOL srvronly )
{
    if( povt->QueryProtocol != NULL ) {
        povt->QueryProtocol = SetProc( povt->QueryProtocol, GETPROC_OLE_OV_QUERYPROTOCOL );
    }
    if( povt->Release != NULL ) {
        povt->Release = SetProc( povt->Release, GETPROC_OLE_OV_RELEASE );
    }
    if( povt->Show != NULL ) {
        povt->Show = SetProc( povt->Show, GETPROC_OLE_OV_SHOW );
    }
    if( povt->DoVerb != NULL ) {
        povt->DoVerb = SetProc( povt->DoVerb, GETPROC_OLE_OV_DOVERB );
    }
    if( povt->GetData != NULL ) {
        povt->GetData = SetProc( povt->GetData, GETPROC_OLE_OV_GETDATA );
    }
    if( povt->SetData != NULL ) {
        povt->SetData = SetProc( povt->SetData, GETPROC_OLE_OV_SETDATA );
    }
    if( povt->SetTargetDevice != NULL ) {
        povt->SetTargetDevice = SetProc( povt->SetTargetDevice, GETPROC_OLE_OV_SETTARGETDEVICE );
    }
    if( povt->SetBounds != NULL ) {
        povt->SetBounds = SetProc( povt->SetBounds, GETPROC_OLE_OV_SETBOUNDS );
    }
    if( povt->EnumFormats != NULL ) {
        povt->EnumFormats = SetProc( povt->EnumFormats, GETPROC_OLE_OV_ENUMFORMATS );
    }
    if( povt->SetColorScheme != NULL ) {
        povt->SetColorScheme = SetProc( povt->SetColorScheme, GETPROC_OLE_OV_SETCOLORSCHEME );
    }

    if( srvronly ) return;

    if( povt->Delete != NULL ) {
        povt->Delete = SetProc( povt->Delete, GETPROC_OLE_OV_DELETE );
    }
    if( povt->SetHostNames != NULL ) {
        povt->SetHostNames = SetProc( povt->SetHostNames, GETPROC_OLE_OV_SETHOSTNAMES );
    }
    if( povt->SaveToStream != NULL ) {
        povt->SaveToStream = SetProc( povt->SaveToStream, GETPROC_OLE_OV_SAVETOSTREAM );
    }
    if( povt->Clone != NULL ) {
        povt->Clone = SetProc( povt->Clone, GETPROC_OLE_OV_CLONE );
    }
    if( povt->CopyFromLink != NULL ) {
        povt->CopyFromLink = SetProc( povt->CopyFromLink, GETPROC_OLE_OV_COPYFROMLINK );
    }
    if( povt->Equal != NULL ) {
        povt->Equal = SetProc( povt->Equal, GETPROC_OLE_OV_EQUAL );
    }
    if( povt->CopyToClipboard != NULL ) {
        povt->CopyToClipboard = SetProc( povt->CopyToClipboard, GETPROC_OLE_OV_COPYTOCLIPBOARD );
    }
    if( povt->Draw != NULL ) {
        povt->Draw = SetProc( povt->Draw, GETPROC_OLE_OV_DRAW );
    }
    if( povt->Activate != NULL ) {
        povt->Activate = SetProc( povt->Activate, GETPROC_OLE_OV_ACTIVATE );
    }
    if( povt->Execute != NULL ) {
        povt->Execute = SetProc( povt->Execute, GETPROC_OLE_OV_EXECUTE );
    }
    if( povt->Close != NULL ) {
        povt->Close = SetProc( povt->Close, GETPROC_OLE_OV_CLOSE );
    }
    if( povt->Update != NULL ) {
        povt->Update = SetProc( povt->Update, GETPROC_OLE_OV_UPDATE );
    }
    if( povt->Reconnect != NULL ) {
        povt->Reconnect = SetProc( povt->Reconnect, GETPROC_OLE_OV_RECONNECT );
    }
    if( povt->ObjectConvert != NULL ) {
        povt->ObjectConvert = SetProc( povt->ObjectConvert, GETPROC_OLE_OV_OBJECTCONVERT );
    }
    if( povt->GetLinkUpdateOptions != NULL ) {
        povt->GetLinkUpdateOptions = SetProc( povt->GetLinkUpdateOptions, GETPROC_OLE_OV_GETLINKUPDATEOPTIONS );
    }
    if( povt->SetLinkUpdateOptions != NULL ) {
        povt->SetLinkUpdateOptions = SetProc( povt->SetLinkUpdateOptions, GETPROC_OLE_OV_SETLINKUPDATEOPTIONS );
    }
    if( povt->Rename != NULL ) {
        povt->Rename = SetProc( povt->Rename, GETPROC_OLE_OV_RENAME );
    }
    if( povt->QueryName != NULL ) {
        povt->QueryName = SetProc( povt->QueryName, GETPROC_OLE_OV_QUERYNAME );
    }
    if( povt->QueryType != NULL ) {
        povt->QueryType = SetProc( povt->QueryType, GETPROC_OLE_OV_QUERYTYPE );
    }
    if( povt->QueryBounds != NULL ) {
        povt->QueryBounds = SetProc( povt->QueryBounds, GETPROC_OLE_OV_QUERYBOUNDS );
    }
    if( povt->QuerySize != NULL ) {
        povt->QuerySize = SetProc( povt->QuerySize, GETPROC_OLE_OV_QUERYSIZE );
    }
    if( povt->QueryOpen != NULL ) {
        povt->QueryOpen = SetProc( povt->QueryOpen, GETPROC_OLE_OV_QUERYOPEN );
    }
    if( povt->QueryOutOfDate != NULL ) {
        povt->QueryOutOfDate = SetProc( povt->QueryOutOfDate, GETPROC_OLE_OV_QUERYOUTOFDATE );
    }
    if( povt->QueryReleaseStatus != NULL ) {
        povt->QueryReleaseStatus = SetProc( povt->QueryReleaseStatus, GETPROC_OLE_OV_QUERYRELEASESTATUS );
    }
    if( povt->QueryReleaseError != NULL ) {
        povt->QueryReleaseError = SetProc( povt->QueryReleaseError, GETPROC_OLE_OV_QUERYRELEASEERROR );
    }
    if( povt->QueryReleaseMethod != NULL ) {
        povt->QueryReleaseMethod = SetProc( povt->QueryReleaseMethod, GETPROC_OLE_OV_QUERYRELEASEMETHOD );
    }
    if( povt->RequestData != NULL ) {
        povt->RequestData = SetProc( povt->RequestData, GETPROC_OLE_OV_REQUESTDATA );
    }
    if( povt->ObjectLong != NULL ) {
        povt->ObjectLong = SetProc( povt->ObjectLong, GETPROC_OLE_OV_OBJECTLONG );
    }
    if( povt->ChangeData != NULL ) {
        povt->ChangeData = SetProc( povt->ChangeData, GETPROC_OLE_OV_CHANGEDATA );
    }

}

void Ole32InitServerDocVtbl( LPOLESERVERDOCVTBL psdvt )
{
    if( psdvt->Save != NULL ) {
        psdvt->Save = SetProc( psdvt->Save, GETPROC_OLE_SDV_SAVE );
    }
    if( psdvt->Close != NULL ) {
        psdvt->Close = SetProc( psdvt->Close, GETPROC_OLE_SDV_CLOSE );
    }
    if( psdvt->SetHostNames != NULL ) {
        psdvt->SetHostNames = SetProc( psdvt->SetHostNames, GETPROC_OLE_SDV_SETHOSTNAMES );
    }
    if( psdvt->SetDocDimensions != NULL ) {
        psdvt->SetDocDimensions = SetProc( psdvt->SetDocDimensions, GETPROC_OLE_SDV_SETDOCDIMENSIONS );
    }
    if( psdvt->GetObject != NULL ) {
        psdvt->GetObject = SetProc( psdvt->GetObject, GETPROC_OLE_SDV_GETOBJECT );
    }
    if( psdvt->Release != NULL ) {
        psdvt->Release = SetProc( psdvt->Release, GETPROC_OLE_SDV_RELEASE );
    }
    if( psdvt->SetColorScheme != NULL ) {
        psdvt->SetColorScheme = SetProc( psdvt->SetColorScheme, GETPROC_OLE_SDV_SETCOLORSCHEME );
    }
    if( psdvt->Execute != NULL ) {
        psdvt->Execute = SetProc( psdvt->Execute, GETPROC_OLE_SDV_EXECUTE );
    }

}

void Ole32InitServerVtbl( LPOLESERVERVTBL psvt )
{
    if( psvt->Open != NULL ) {
        psvt->Open = SetProc( psvt->Open, GETPROC_OLE_SV_OPEN );
    }
    if( psvt->Create != NULL ) {
        psvt->Create = SetProc( psvt->Create, GETPROC_OLE_SV_CREATE );
    }
    if( psvt->CreateFromTemplate != NULL ) {
        psvt->CreateFromTemplate = SetProc( psvt->CreateFromTemplate, GETPROC_OLE_SV_CREATEFROMTEMPLATE );
    }
    if( psvt->Edit != NULL ) {
        psvt->Edit = SetProc( psvt->Edit, GETPROC_OLE_SV_EDIT );
    }
    if( psvt->Exit != NULL ) {
        psvt->Exit = SetProc( psvt->Exit, GETPROC_OLE_SV_EXIT );
    }
    if( psvt->Release != NULL ) {
        psvt->Release = SetProc( psvt->Release, GETPROC_OLE_SV_RELEASE );
    }
    if( psvt->Execute != NULL ) {
        psvt->Execute = SetProc( psvt->Execute, GETPROC_OLE_SV_EXECUTE );
    }
}

void Ole32InitClientVtbl( LPOLECLIENTVTBL pcvt )
{
    if( pcvt->CallBack != NULL ) {
        // OLE_OV_GETDATA also has DWORD,WORD,DWORD parms, and this
        // will minimize the number of changes required
        pcvt->CallBack = SetProc( pcvt->CallBack, GETPROC_OLE_OV_GETDATA );
    }
}
