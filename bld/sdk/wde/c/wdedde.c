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


#include <windows.h>
#include <ddeml.h>
#include <string.h>

#include "wdeglbl.h"
#include "wrdll.h"
#include "wdemsgbx.h"
#include "wdemsgs.h"
#include "wdemem.h"
#include "wderes.h"
#include "wdefdiag.h"
#include "wdei2mem.h"
#include "wdemsgbx.h"
#include "wdesdlg.h"
#include "wdesvres.h"
#include "wdemain.h"
#include "wderesin.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define TIME_OUT                4000
#define WRE_SERVICE_NAME        "WATCOMResourceEditor"
#define WRE_DIALOG_TOPIC        "WATCOMEditDialogs"
#define WRE_DIALOG_DUMP         "WATCOMDumpDialog"
#define WDE_SERVICE_NAME        "WATCOMDialogEditor"
#define WDE_DIALOG_TOPIC        "WATCOMDialogEditLink"

#define WRE_32BIT_ITEM          "WATCOMRes32Bit"
#define WRE_FILE_ITEM           "WATCOMResFile"
#define WRE_NAME_ITEM           "WATCOMResName"
#define WRE_DATA_ITEM           "WATCOMResData"

#ifndef __ALPHA__
void WdeInt3( void );
#pragma aux WdeInt3 =                                   \
        " int    3h             "
#endif

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern HDDEDATA WINEXPORT DdeCallBack( WORD wType, WORD wFmt, HCONV hConv,
                                      HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
                                      DWORD lData1, DWORD lData2 );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void     *WdeHData2Mem           ( HDDEDATA hData );
static Bool     WdeStartDDEEditSession  ( void );
static HDDEDATA WdeCreateResNameData    ( WResID *name, Bool is32bit );
static HDDEDATA WdeCreateResData        ( WdeResDlgItem *ditem );
static void     WdeHandlePokedData      ( HDDEDATA hdata );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static  DWORD   IdInst = 0;
static  FARPROC DdeProc;
static  HSZ     hDialogService = NULL;
static  HSZ     hDialogTopic = NULL;
static  HSZ     hService = NULL;
static  HSZ     hTopic = NULL;
static  HSZ     hFileItem = NULL;
static  HSZ     hIs32BitItem = NULL;
static  HSZ     hNameItem = NULL;
static  HSZ     hDataItem = NULL;
static  HCONV   WdeClientConv = NULL;
static  HCONV   WdeServerConv = NULL;
static  UINT    WdeDataClipbdFormat  = 0;

Bool WdeDDEStart( HINSTANCE inst )
{
    WORD        ret;
    DWORD       flags;

    _wde_touch(inst); /* MakeProcInstance vanishes in NT */

    if( IdInst != 0 ) {
        return( FALSE );
    }

    WdeDataClipbdFormat  = RegisterClipboardFormat( WR_CLIPBD_DIALOG );
    if( WdeDataClipbdFormat == 0 ) {
        return( FALSE );
    }

    DdeProc = MakeProcInstance( (FARPROC)DdeCallBack, inst );
    if( DdeProc == (FARPROC)NULL ) {
        return( FALSE );
    }

    flags = APPCLASS_STANDARD | APPCMD_FILTERINITS |
                CBF_FAIL_ADVISES | CBF_FAIL_SELFCONNECTIONS |
                CBF_SKIP_REGISTRATIONS | CBF_SKIP_UNREGISTRATIONS;

    ret = DdeInitialize( &IdInst, (PFNCALLBACK)DdeProc, flags, 0 );
    if( ret != DMLERR_NO_ERROR ) {
        return( FALSE );
    }

    hDialogService = DdeCreateStringHandle( IdInst, WDE_SERVICE_NAME, CP_WINANSI );
    if( hDialogService == (HSZ)NULL ) {
        return( FALSE );
    }

    hDialogTopic = DdeCreateStringHandle( IdInst, WDE_DIALOG_TOPIC, CP_WINANSI );
    if( hDialogTopic == (HSZ)NULL ) {
        return( FALSE );
    }

    hFileItem = DdeCreateStringHandle( IdInst, WRE_FILE_ITEM, CP_WINANSI );
    if( hFileItem == (HSZ)NULL ) {
        return( FALSE );
    }

    hIs32BitItem = DdeCreateStringHandle( IdInst, WRE_32BIT_ITEM, CP_WINANSI );
    if( hIs32BitItem == (HSZ)NULL ) {
        return( FALSE );
    }

    hNameItem = DdeCreateStringHandle( IdInst, WRE_NAME_ITEM, CP_WINANSI );
    if( hNameItem == (HSZ)NULL ) {
        return( FALSE );
    }

    hDataItem = DdeCreateStringHandle( IdInst, WRE_DATA_ITEM, CP_WINANSI );
    if( hDataItem == (HSZ)NULL ) {
        return( FALSE );
    }

    DdeNameService( IdInst, hDialogService, (HSZ)NULL, DNS_REGISTER );

    return( TRUE );
}

void WdeDDEEnd( void )
{
    if( IdInst != 0 ) {
        DdeNameService( IdInst, (HSZ)NULL, (HSZ)NULL, DNS_UNREGISTER );
        if( hDataItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hDataItem );
        }
        if( hNameItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hNameItem );
        }
        if( hFileItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hFileItem );
        }
        if( hIs32BitItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hIs32BitItem );
        }
        if( hDialogTopic != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hDialogTopic );
        }
        if( hDialogService != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hDialogService );
        }
        DdeUninitialize( IdInst );
        IdInst = 0;
    }
    if( DdeProc != (FARPROC)NULL ) {
        FreeProcInstance( DdeProc );
    }
}

Bool WdeDDEDumpConversation( HINSTANCE inst )
{
    HCONV       hconv;
    HSZ         hservice;
    HSZ         htopic;
    Bool        ok;

    ok = WdeDDEStart( inst );

    if( ok ) {
        hservice = DdeCreateStringHandle( IdInst, WRE_SERVICE_NAME, CP_WINANSI );
        ok = ( hservice != (HSZ)NULL );
    }

    if( ok ) {
        htopic = DdeCreateStringHandle( IdInst, WRE_DIALOG_DUMP, CP_WINANSI );
        ok = ( htopic != (HSZ)NULL );
    }

    if( ok ) {
        // We expect the server to reject this connect attempt
        // if it doesn't then we terminate the conversation
        hconv = DdeConnect( IdInst, hservice, htopic, (LPVOID)NULL );
        if( hconv != (HCONV)NULL ) {
            DdeDisconnect( hconv );
        }
    }

    if( hservice != (HSZ)NULL ) {
        DdeFreeStringHandle( IdInst, hservice );
    }

    if( htopic != (HSZ)NULL ) {
        DdeFreeStringHandle( IdInst, htopic );
    }

    WdeDDEEnd();

    if( !ok ) {
        WdeDisplayErrorMsg( WDE_DDEDEATHMSG );
    }

    return( ok );
}

Bool WdeDDEStartConversation( void )
{
    if( IdInst == 0 ) {
        return( FALSE );
    }

    hService = DdeCreateStringHandle( IdInst, WRE_SERVICE_NAME, CP_WINANSI );
    if( hService == (HSZ)NULL ) {
        return( FALSE );
    }

    hTopic = DdeCreateStringHandle( IdInst, WRE_DIALOG_TOPIC, CP_WINANSI );
    if( hTopic == (HSZ)NULL ) {
        return( FALSE );
    }

    WdeClientConv = DdeConnect( IdInst, hService, hTopic, (LPVOID)NULL );
    if( WdeClientConv == (HCONV)NULL ) {
        return( FALSE );
    }

    if( !WdeStartDDEEditSession() ) {
        return( FALSE );
    }

    return( TRUE );
}

void WdeDDEEndConversation( void )
{
    if( WdeClientConv != (HCONV)NULL ) {
        DdeDisconnect( WdeClientConv );
        WdeClientConv = (HCONV)NULL;
    }
    if( WdeServerConv != (HCONV)NULL ) {
        DdeDisconnect( WdeServerConv );
        WdeServerConv = (HCONV)NULL;
    }
    if( hService != (HSZ)NULL ) {
        DdeFreeStringHandle( IdInst, hService );
        hService = (HSZ)NULL;
    }
    if( hTopic != (HSZ)NULL ) {
        DdeFreeStringHandle( IdInst, hTopic );
        hTopic = (HSZ)NULL;
    }
}

void *WdeHData2Mem( HDDEDATA hData )
{
    void        *mem;
    uint_32     size;

    if( hData == (HDDEDATA)NULL ) {
        return( NULL );
    }

    size = (uint_32)DdeGetData( hData, NULL, 0, 0 );
    if( size == 0 ) {
        return( NULL );
    }

    mem = WdeMemAlloc( size );
    if( mem == NULL ) {
        return( NULL );
    }

    if( (DWORD)size != DdeGetData( hData, mem, (DWORD)size, 0 ) ) {
        WdeMemFree( mem );
        return( NULL );
    }

    return( mem );
}

HDDEDATA WdeCreateResNameData( WResID *name, Bool is32bit )
{
    HDDEDATA    hdata;
    void        *data;
    uint_32     size;

    hdata = (HDDEDATA)NULL;

    if( WRWResID2Mem( name, &data, &size, is32bit ) ) {
        hdata = DdeCreateDataHandle( IdInst, (LPBYTE)data, size, 0,
                                     hNameItem, WdeDataClipbdFormat, 0 );
        WdeMemFree( data );
    }

    return( hdata );
}

HDDEDATA WdeCreateResData( WdeResDlgItem *ditem )
{
    HDDEDATA    hdata;
    void        *data;
    uint_32     size;

    hdata = (HDDEDATA)NULL;

    if( WdeGetItemData( ditem, &data, &size ) ) {
        hdata = DdeCreateDataHandle( IdInst, (LPBYTE)data, size, 0,
                                     hDataItem, WdeDataClipbdFormat, 0 );
        WdeMemFree( data );
    }

    return( hdata );
}

WdeResDlgItem *WdeGetDlgItem( void )
{
    WdeResInfo          *rinfo;
    WdeResDlgItem       *ditem;

    ditem = NULL;
    rinfo = WdeGetCurrentRes();
    if( rinfo && rinfo->dlg_item_list ) {
        ditem = (WdeResDlgItem *) ListElement( rinfo->dlg_item_list );
    }

    return( ditem );
}

Bool WdeUpdateDDEEditSession( void )
{
    WdeResInfo          *rinfo;
    WdeResDlgItem       *ditem;
    HDDEDATA            hdata;
    Bool                ok;

    hdata = (HDDEDATA)NULL;
    ditem = WdeGetDlgItem();
    ok = ( ( WdeClientConv != (HCONV)NULL ) && ( ditem != NULL ) );

    if( ok ) {
        hdata = WdeCreateResData( ditem );
        ok = ( hdata != (HDDEDATA)NULL );
    }

    if( ok ) {
        ok = (Bool) DdeClientTransaction( (LPBYTE)hdata, -1, WdeClientConv,
                                          hDataItem, WdeDataClipbdFormat,
                                          XTYP_POKE, TIME_OUT, NULL );
    }

    if( hdata != (HDDEDATA)NULL ) {
        DdeFreeDataHandle( hdata );
    }

    if( ok ) {
        hdata = WdeCreateResNameData( ditem->dialog_name, ditem->is32bit );
        ok = ( hdata != (HDDEDATA)NULL );
    }

    if( ok ) {
        ok = (Bool) DdeClientTransaction( (LPBYTE)hdata, -1, WdeClientConv,
                                          hNameItem, WdeDataClipbdFormat,
                                          XTYP_POKE, TIME_OUT, NULL );
    }

    if( hdata != (HDDEDATA)NULL ) {
        DdeFreeDataHandle( hdata );
    }

    if( ok ) {
        rinfo = WdeGetCurrentRes();
        WdeSetResModified( rinfo, FALSE );
    }

    return( ok );
}

Bool WdeStartDDEEditSession( void )
{
    WdeResInfo          *rinfo;
    WdeResDlgItem       *ditem;
    char                *filename;
    HDDEDATA            hData;
    void                *data;
    DWORD               ret;
    uint_32             size;
    OBJPTR              object;
    Bool                ok;

    object = NULL;
    ditem = WdeAllocResDlgItem();
    ok = ( ditem != NULL );

    if( ok ) {
        hData = DdeClientTransaction( NULL, 0, WdeClientConv,
                                      hFileItem, WdeDataClipbdFormat,
                                      XTYP_REQUEST, TIME_OUT, &ret );
        ok = ( hData != (HDDEDATA)NULL );
    }

    if( ok ) {
        filename = (char *)WdeHData2Mem( hData );
        DdeFreeDataHandle( hData );
        ok = ( filename != NULL );
    }

    if( ok ) {
        hData = DdeClientTransaction( NULL, 0, WdeClientConv,
                                      hIs32BitItem, WdeDataClipbdFormat,
                                      XTYP_REQUEST, TIME_OUT, &ret );
        if( hData != (HDDEDATA)NULL ) {
            ditem->is32bit = TRUE;
            DdeFreeDataHandle( hData );
        }
    }

    if( ok ) {
        hData = DdeClientTransaction( NULL, 0, WdeClientConv,
                                      hNameItem, WdeDataClipbdFormat,
                                      XTYP_REQUEST, TIME_OUT, &ret );
        ok = ( hData != (HDDEDATA)NULL );
    }

    if( ok ) {
        data = WdeHData2Mem( hData );
        DdeFreeDataHandle( hData );
        ok = ( data != NULL );
    }

    if( ok ) {
        ditem->dialog_name = WRMem2WResID( data, ditem->is32bit );
        ok = ( ditem->dialog_name != NULL );
        WdeMemFree( data );
    }

    if( ok ) {
        hData = DdeClientTransaction( NULL, 0, WdeClientConv,
                                      hDataItem, WdeDataClipbdFormat,
                                      XTYP_REQUEST, TIME_OUT, &ret );
        if( hData != (HDDEDATA)NULL ) {
            data = WdeHData2Mem( hData );
            size = (int)DdeGetData( hData, NULL, 0, 0 );
            DdeFreeDataHandle( hData );
            if( data ) {
                ditem->dialog_info = WdeMem2DBI( (uint_8 *)data, size,
                                                 ditem->is32bit );
                ok = ( ditem->dialog_info != NULL );
                WdeMemFree( data );
            } else {
                ok = FALSE;
            }
        }
    }

    if( ok ) {
        rinfo = WdeCreateNewResource( filename );
        ok = ( rinfo != NULL );
    }

    if( ok ) {
        if( ditem->dialog_info ) {
            ok = WdeOpenDialogFromResInfo( rinfo, ditem );
            if( ok ) {
                WdeAddResDlgItemToResInfo( rinfo, ditem );
                object = ditem->object;
            }
        } else {
            object = WdeCreateNewDialog( ditem->dialog_name, ditem->is32bit );
            if( ditem ) {
                WdeFreeResDlgItem( &ditem, TRUE );
            }
            ditem = NULL;
        }
        ok = ok && ( object != NULL );
    }

    if( ok ) {
        MakeObjectCurrent( object );
    }

    if( !ok ) {
        if( ditem ) {
            WdeFreeResDlgItem( &ditem, TRUE );
        }
        if( rinfo ) {
            WdeFreeResInfo( rinfo );
        }
    }

    if( filename ) {
        WdeMemFree( filename );
    }

    return( ok );
}

static Bool GotEndSession = FALSE;
void WdeHandlePokedData( HDDEDATA hdata )
{
    HWND        main;
    char        *cmd;
    WdeResInfo  *rinfo;

    if( hdata == (HDDEDATA)NULL ) {
        return;
    }

    cmd = (char *)WdeHData2Mem( hdata );
    if( cmd == NULL ) {
        return;
    }

    main = WdeGetMainWindowHandle();

    if( !stricmp( cmd, "show" ) ) {
        ShowWindow( main, SW_RESTORE );
        ShowWindow( main, SW_SHOWNA );
    } else if( !stricmp( cmd, "hide" ) ) {
        ShowWindow( main, SW_SHOWMINNOACTIVE );
        ShowWindow( main, SW_HIDE );
    } else if( !stricmp( cmd, "endsession" ) ) {
        if( GotEndSession == FALSE ) {
            GotEndSession = TRUE;
            rinfo = WdeGetCurrentRes();
            WdeDestroyResourceWindow( rinfo );
        }
    } else if( !stricmp( cmd, "bringtofront" ) ) {
        if( IsIconic( main ) ) {
            ShowWindow( main, SW_RESTORE );
        }
#ifdef __NT__
        SetWindowPos( main, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( main, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( main, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
        SetForegroundWindow( main );
#else
        SetActiveWindow( main );
        SetWindowPos( main, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
#endif
    }

    WdeMemFree( cmd );
}

HDDEDATA WINEXPORT DdeCallBack( WORD wType, WORD wFmt, HCONV hConv,
                               HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
                               DWORD lData1, DWORD lData2 )
{
    HWND                hmain;
    HSZPAIR             hszpair[2];
    HDDEDATA            ret;
    WdeResDlgItem       *ditem;

    _wde_touch(wFmt);
    _wde_touch(hdata);
    _wde_touch(lData1);
    _wde_touch(lData2);

    ret = (HDDEDATA)NULL;

    switch( wType ) {
        case XTYP_CONNECT_CONFIRM:
            WdeServerConv = hConv;
            break;

        case XTYP_DISCONNECT:
            WdeServerConv = (HCONV)NULL;
            WdeClientConv = (HCONV)NULL;
            hmain = WdeGetMainWindowHandle();
            SendMessage( hmain, WM_CLOSE, (WPARAM)1, 0 );
            break;

        case XTYP_CONNECT:
            if( (WdeServerConv == (HCONV)NULL) && (hsz1 == hDialogTopic) ) {
                ret = (HDDEDATA)TRUE;
            }
            break;

        case XTYP_WILDCONNECT:
            if( hsz2 != hDialogService ) {
                break;
            }
            hszpair[0].hszSvc   = hDialogService;
            hszpair[0].hszTopic = hDialogTopic;
            hszpair[1].hszSvc   = (HSZ)NULL;
            hszpair[1].hszTopic = (HSZ)NULL;
            ret = (HDDEDATA)
                DdeCreateDataHandle( IdInst, (LPBYTE)&hszpair[0],
                                     sizeof(hszpair), 0L, 0, CF_TEXT, 0 );
            break;

        case XTYP_REQUEST:
            if( wFmt == WdeDataClipbdFormat ) {
                if( hsz1 == hTopic ) {
                    ditem = WdeGetDlgItem();
                    if( hsz2 == hDataItem ) {
                        ret = WdeCreateResData( ditem );
                    } else if( hsz2 == hNameItem ) {
                        ret = WdeCreateResNameData( ditem->dialog_name, ditem->is32bit );
                    }
                }
            }
            break;

        case XTYP_POKE:
            ret = (HDDEDATA)DDE_FNOTPROCESSED;
            if( hsz1 == hDialogTopic ) {
                if( hsz2 == hDataItem ) {
                    WdeHandlePokedData( hdata );
                    ret = (HDDEDATA)DDE_FACK;
                }
            }
            break;

    }

    return( ret );
}

