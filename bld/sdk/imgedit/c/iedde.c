/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "imgedit.h"
#include "wclbdde.h"
#include "iebmpdat.h"
#include "iedde.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define NUM_FORMATS             3

#define TIME_OUT                4000

#define WRE_SERVICE_NAME        "WATCOMResourceEditor"
#define WRE_BITMAP_TOPIC        "WATCOMEditBitmaps"
#define WRE_CURSOR_TOPIC        "WATCOMEditCursors"
#define WRE_ICON_TOPIC          "WATCOMEditIcons"
#define WRE_IMAGE_DUMP          "WATCOMDumpImage"

#define BMP_SERVICE_NAME        "WATCOMBitmapEditor"
#define CUR_SERVICE_NAME        "WATCOMCursorEditor"
#define ICO_SERVICE_NAME        "WATCOMIconEditor"

#define BMP_SERVICE_TOPIC       "WATCOMBitmapEditLink"
#define CUR_SERVICE_TOPIC       "WATCOMCursorEditLink"
#define ICO_SERVICE_TOPIC       "WATCOMIconEditLink"

#define WRE_FILE_ITEM           "WATCOMResFile"
#define WRE_NAME_ITEM           "WATCOMResName"
#define WRE_DATA_ITEM           "WATCOMResData"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef enum {
    DDEBitmap,
    DDECursor,
    DDEIcon,
    DDENone
}IEEditFormat;

typedef struct IEService {
    char        *service;
    char        *topic;
    HSZ         hservice;
    HSZ         htopic;
} IEService;

typedef struct IETopic {
    char        *topic;
    HSZ         htopic;
} IETopic;

typedef struct IEClipFormat {
    char        *str;
    UINT        format;
} IEClipFormat;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT FNCALLBACK DdeCallBack;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

static bool     IEHData2Mem( HDDEDATA, void **, uint_32 * );
static bool     IEStartDDEEditSession( void );
static HDDEDATA IECreateResData( img_node *node );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

// These are the clipboard formats registered by the Image Editor
static IEClipFormat IEClipFormats[NUM_FORMATS] =
{
    { WR_CLIPBD_BITMAP, 0 },
    { WR_CLIPBD_CURSOR, 0 },
    { WR_CLIPBD_ICON,   0 }
};

// These are the services offered by the Image Editor
static IEService IEServices[NUM_FORMATS] =
{
    { BMP_SERVICE_NAME, BMP_SERVICE_TOPIC, NULL, NULL },
    { CUR_SERVICE_NAME, CUR_SERVICE_TOPIC, NULL, NULL },
    { ICO_SERVICE_NAME, ICO_SERVICE_TOPIC, NULL, NULL }
};

#if 0
// These are the services offered by the Resource Editor
// that will be used by the Image Editor
static IETopic IETopics[NUM_FORMATS] =
{
    { WRE_BITMAP_TOPIC, NULL },
    { WRE_CURSOR_TOPIC, NULL },
    { WRE_ICON_TOPIC,   NULL }
};
#endif

static IEEditFormat EditFormat = DDENone;
static DWORD        IdInst = 0;
static PFNCALLBACK  DdeProcInst = NULL;
static HSZ          hFileItem = NULL;
static HSZ          hNameItem = NULL;
static HSZ          hDataItem = NULL;
static HSZ          hService = NULL;
static HCONV        IEClientConv = NULL;
static HCONV        IEServerConv = NULL;
static bool         GotEndSession = false;

/*
 * IEDDEStart
 */
bool IEDDEStart( HINSTANCE inst )
{
    WORD        ret;
    DWORD       flags;
    int         i;
    bool        ok;

    _imged_touch( inst ); /* MakeProcInstance vanishes in NT */

    ok = ( IdInst == 0 );

    if( ok ) {

        for( i = 0; i < NUM_FORMATS; i++ ) {
            IEClipFormats[i].format = RegisterClipboardFormat( IEClipFormats[i].str );
            if( IEClipFormats[i].format == 0 ) {
                ok = false;
                break;
            }
        }

        if( ok ) {
            DdeProcInst = MakeProcInstance_DDE( DdeCallBack, inst );
            ok = ( DdeProcInst != NULL );
        }
        if( ok ) {
            flags = APPCLASS_STANDARD | APPCMD_FILTERINITS |
                    CBF_FAIL_ADVISES | CBF_FAIL_SELFCONNECTIONS |
                    CBF_SKIP_REGISTRATIONS | CBF_SKIP_UNREGISTRATIONS;

            ret = DdeInitialize( &IdInst, DdeProcInst, flags, 0 );
            ok = ( ret == DMLERR_NO_ERROR );
        }

        if( ok ) {
            for( i = 0; i < NUM_FORMATS; i++ ) {
                IEServices[i].hservice = DdeCreateStringHandle( IdInst, IEServices[i].service, CP_WINANSI );
                IEServices[i].htopic = DdeCreateStringHandle( IdInst, IEServices[i].topic, CP_WINANSI );
            }

            hFileItem = DdeCreateStringHandle( IdInst, WRE_FILE_ITEM, CP_WINANSI );
            ok = ( hFileItem != (HSZ)NULL );
        }

        if( ok ) {
            hNameItem = DdeCreateStringHandle( IdInst, WRE_NAME_ITEM, CP_WINANSI );
            ok = ( hNameItem != (HSZ)NULL );
        }

        if( ok ) {
            hDataItem = DdeCreateStringHandle( IdInst, WRE_DATA_ITEM, CP_WINANSI );
            ok = ( hDataItem != (HSZ)NULL );
        }

        if( ok ) {
            for( i = 0; i < NUM_FORMATS; i++ ) {
                if( IEServices[i].hservice != (HSZ)NULL ) {
                    DdeNameService( IdInst, IEServices[i].hservice, (HSZ)NULL, DNS_REGISTER );
                }
            }
        }

    }
    return( ok );

} /* IEDDEStart */

/*
 * IEDDEEnd
 */
void IEDDEEnd( void )
{
    int i;

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
        for( i = 0; i < NUM_FORMATS; i++ ) {
            if( IEServices[i].hservice != (HSZ)NULL ) {
                DdeFreeStringHandle( IdInst, IEServices[i].hservice );
            }
            if( IEServices[i].htopic != (HSZ)NULL ) {
                DdeFreeStringHandle( IdInst, IEServices[i].htopic );
            }
        }
        DdeUninitialize( IdInst );
        IdInst = 0;
    }
    if( DdeProcInst != NULL ) {
        FreeProcInstance_DDE( DdeProcInst );
    }

} /* IEDDEEnd */

/*
 * IEDDEDumpConversation
 */
bool IEDDEDumpConversation( HINSTANCE inst )
{
    HCONV       hconv;
    HSZ         hservice;
    HSZ         htopic;
    bool        ok;

    ok = IEDDEStart( inst );

    if( ok ) {
        hservice = DdeCreateStringHandle( IdInst, WRE_SERVICE_NAME, CP_WINANSI );
        ok = (hservice != (HSZ)NULL);
    }

    if( ok ) {
        htopic = DdeCreateStringHandle( IdInst, WRE_IMAGE_DUMP, CP_WINANSI );
        ok = (htopic != (HSZ)NULL);
    }

    if( ok ) {
        // We expect the server to reject this connect attempt.
        // If it doesn't, then we terminate the conversation.
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

    IEDDEEnd();

    if( !ok ) {
        IEDisplayErrorMsg( WIE_DDEINITTITLE, WIE_DDETERMINATIONMSG, MB_OK | MB_ICONINFORMATION );
    }

    return( ok );

} /* IEDDEDumpConversation */

/*
 * IEDDEStartConversation
 */
bool IEDDEStartConversation( void )
{
    if( IdInst == 0 ) {
        return( false );
    }

    hService = DdeCreateStringHandle( IdInst, WRE_SERVICE_NAME, CP_WINANSI );
    if( hService == (HSZ)NULL ) {
        return( false );
    }

    IEClientConv = DdeConnect( IdInst, hService, (HSZ)NULL, (LPVOID)NULL );
    if( IEClientConv == (HCONV)NULL ) {
        return( false );
    }

    if( !IEStartDDEEditSession() ) {
        return( false );
    }

    return( true );

} /* IEDDEStartConversation */

/*
 * IEDDEEndConversation
 */
void IEDDEEndConversation( void )
{
    if( IEClientConv != (HCONV)NULL ) {
        DdeDisconnect( IEClientConv );
        IEClientConv = (HCONV)NULL;
    }
    if( IEServerConv != (HCONV)NULL ) {
        DdeDisconnect( IEServerConv );
        IEServerConv = (HCONV)NULL;
    }
    if( hService != (HSZ)NULL ) {
        DdeFreeStringHandle( IdInst, hService );
        hService = (HSZ)NULL;
    }

} /* IEDDEEndConversation */

/*
 * IEHData2Mem
 */
bool IEHData2Mem( HDDEDATA hData, void **mem, uint_32 *size )
{
    bool    ok;

    ok = ( hData != NULL && mem != NULL && size != NULL );

    if( ok ) {
        *size = (uint_32)DdeGetData( hData, NULL, 0, 0 );
        ok = ( *size != 0 );

        if( ok ) {
            *mem = MemAlloc( *size );
            ok = ( *mem != NULL );
        }

        if( ok && (DWORD)*size != DdeGetData( hData, *mem, (DWORD)*size, 0 ) ) {
            MemFree( *mem );
            *mem = NULL;
            ok = false;
        }
    }
    return( ok );

} /* IEHData2Mem */

/*
 * IECreateResData
 */
HDDEDATA IECreateResData( img_node *node )
{
    HDDEDATA    hdata;
    BYTE        *data;
    size_t      size;
    bool        ok;

    data = NULL;
    hdata = NULL;
    ok = (node != NULL && EditFormat != DDENone);

    if( ok ) {
        switch( node->imgtype ) {
        case BITMAP_IMG:
            ok = SaveBitmapToData( node, &data, &size );
            break;
        case ICON_IMG:
        case CURSOR_IMG:
            ok = SaveImgToData( node, &data, &size );
            break;
        default:
            ok = false;
            break;
        }
    }

    if( ok ) {
        hdata = DdeCreateDataHandle( IdInst, (LPBYTE)data, (DWORD)size, 0, hDataItem, IEClipFormats[EditFormat].format, 0 );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( hdata );

} /* IECreateResData */

/*
 * IEGetCurrentImageNode
 */
static img_node *IEGetCurrentImageNode( void )
{
    img_node    *node;
    img_node    *root;

    node = GetCurrentNode();
    if( node == NULL ) {
        return( NULL );
    }

    root = GetImageNode( node->hwnd );
    if( root == NULL ) {
        return( NULL );
    }

    return( root );

} /* IEGetCurrentImageNode */

/*
 * IEUpdateDDEEditSession
 */
bool IEUpdateDDEEditSession( void )
{
    img_node            *node;
    HDDEDATA            hdata;
    bool                ok;

    hdata = NULL;
    node = IEGetCurrentImageNode();
    ok = (IEClientConv != (HCONV)NULL && node != NULL && EditFormat != DDENone);

    if( ok ) {
        hdata = IECreateResData( node );
        ok = (hdata != NULL);
    }

    if( ok ) {
        ok = DdeClientTransaction( (LPBYTE)hdata, (DWORD)-1L, IEClientConv, hDataItem,
                                         IEClipFormats[EditFormat].format,
                                         XTYP_POKE, TIME_OUT, NULL ) != 0;
    }

    if( hdata != NULL ) {
        DdeFreeDataHandle( hdata );
    }

    if( ok ) {
        SetIsSaved( node->hwnd, true );
    }

    return( ok );

} /* IEUpdateDDEEditSession */

/*
 * IEStartDDEEditSession
 */
bool IEStartDDEEditSession( void )
{
    char                *filename;
    HDDEDATA            hData;
    void                *data;
    DWORD               ret;
    uint_32             size;
    bool                ok;

    data = NULL;
    filename = NULL;
    ok = (EditFormat != DDENone);

    if( ok ) {
        hData = DdeClientTransaction( NULL, 0, IEClientConv, hFileItem,
                                      IEClipFormats[EditFormat].format,
                                      XTYP_REQUEST, TIME_OUT, &ret );
        ok = (hData != NULL);
    }

    if( ok ) {
        ok = IEHData2Mem( hData, (void **)&filename, &size );
        DdeFreeDataHandle( hData );
    }

    if( ok ) {
        hData = DdeClientTransaction( NULL, 0, IEClientConv, hDataItem,
                                      IEClipFormats[EditFormat].format,
                                      XTYP_REQUEST, TIME_OUT, &ret );
    }

    if( ok ) {
        if( hData != NULL ) {
            ok = IEHData2Mem( hData, &data, &size );
            DdeFreeDataHandle( hData );
        }
    }

    if( ok ) {
        switch( EditFormat ) {
        case DDEBitmap:
            if( data != NULL ) {
                ok = ReadBitmapFromData( data, filename, NULL, NULL );
            } else {
                ok = NewImage( BITMAP_IMG, NULL );
            }
            break;
        case DDECursor:
            if( data != NULL ) {
                ok = ReadCursorFromData( data, filename, NULL, NULL );
            } else {
                ok = NewImage( CURSOR_IMG, NULL );
            }
            break;
        case DDEIcon:
            if( data != NULL ) {
                ok = ReadIconFromData( data, filename, NULL, NULL );
            } else {
                ok = NewImage( ICON_IMG, NULL );
            }
            break;
        default:
            ok = false;
            break;
        }
    }

    if( filename != NULL ) {
        MemFree( filename );
    }

    if( data != NULL ) {
        MemFree( data );
    }

    return( ok );

} /* IEStartDDEEditSession */

/*
 * IEHandlePokedData
 */
static void IEHandlePokedData( HDDEDATA hdata )
{
    void        *cmd;
    uint_32     size;

    if( hdata == NULL ) {
        return;
    }

    cmd = NULL;
    if( !IEHData2Mem( hdata, &cmd, &size ) || cmd == NULL ) {
        return;
    }

    if( stricmp( cmd, "show" ) == 0 ) {
        ShowWindow( HMainWindow, SW_RESTORE );
        ShowWindow( HMainWindow, SW_SHOWNA );
    } else if( stricmp( cmd, "hide" ) == 0 ) {
        ShowWindow( HMainWindow, SW_SHOWMINNOACTIVE );
        ShowWindow( HMainWindow, SW_HIDE );
    } else if( stricmp( cmd, "endsession" ) == 0 ) {
        if( !GotEndSession ) {
            GotEndSession = true;
            CloseAllImages();
        }
    } else if( stricmp( cmd, "bringtofront" ) == 0 ) {
        if( IsIconic( HMainWindow ) ) {
            ShowWindow( HMainWindow, SW_RESTORE );
        }
#ifdef __NT__
        SetWindowPos( HMainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( HMainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( HMainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetForegroundWindow( HMainWindow );
#else
        SetActiveWindow( HMainWindow );
        SetWindowPos( HMainWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#endif
    }

    MemFree( cmd );

} /* IEHandlePokedData */

/*
 * DdeCallBack
 */
HDDEDATA CALLBACK DdeCallBack( UINT wType, UINT wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2,
                               HDDEDATA hdata, ULONG_PTR lData1, ULONG_PTR lData2 )
{
    img_node            *node;
    HSZPAIR             hszpair[2];
    HDDEDATA            ret;
    int                 i;

    _imged_touch( wFmt );
    _imged_touch( hdata );
    _imged_touch( lData1 );
    _imged_touch( lData2 );

    ret = NULL;

    switch( wType ) {
    case XTYP_CONNECT_CONFIRM:
        IEServerConv = hConv;
        break;

    case XTYP_DISCONNECT:
        IEServerConv = (HCONV)NULL;
        IEClientConv = (HCONV)NULL;
        SendMessage( HMainWindow, WM_CLOSE, (WPARAM)1, 0 );
        break;

    case XTYP_CONNECT:
        for( i = 0; i < NUM_FORMATS; i++ ) {
            if( IEServices[i].htopic == hsz1 ) {
                EditFormat = (IEEditFormat)i;
                ret = (HDDEDATA)TRUE;
            }
        }
        break;

    case XTYP_WILDCONNECT:
        for( i = 0; i < NUM_FORMATS; i++ ) {
            if( IEServices[i].hservice == hsz2 ) {
                break;
            }
        }
        if( i == NUM_FORMATS ) {
            break;
        }
        hszpair[0].hszSvc = IEServices[i].hservice;
        hszpair[0].hszTopic = IEServices[i].htopic;
        hszpair[1].hszSvc = (HSZ)NULL;
        hszpair[1].hszTopic = (HSZ)NULL;
        ret = DdeCreateDataHandle( IdInst, (LPBYTE)&hszpair[0], (DWORD)sizeof( hszpair ), 0L, 0, CF_TEXT, 0 );
        break;

    case XTYP_REQUEST:
        if( wFmt == IEClipFormats[EditFormat].format ) {
            if( hsz1 == IEServices[EditFormat].htopic ) {
                node = IEGetCurrentImageNode();
                if( hsz2 == hDataItem ) {
                    ret = IECreateResData( node );
                }
            }
        }
        break;

    case XTYP_POKE:
        ret = (HDDEDATA)DDE_FNOTPROCESSED;
        if( hsz1 == IEServices[EditFormat].htopic ) {
            if( hsz2 == hDataItem ) {
                IEHandlePokedData( hdata );
                ret = (HDDEDATA)DDE_FACK;
            }
        }
        break;
    }

    return( ret );

} /* DdeCallback */
