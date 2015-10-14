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


#include "commonui.h"
#include <ddeml.h>

#include "watcom.h"
#include "wreglbl.h"
#include "wresall.h"
#include "wreres.h"
#include "wregcres.h"
#include "wresrvr.h"
#include "wredlg.h"
#include "wredde.h"
#include "wreimg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define NUM_SERVERS     4
#define NUM_TOPICS      4
#define TIME_OUT        4000
#define LONG_TIME_OUT   0x7fffffff

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT HDDEDATA CALLBACK DdeCallBack( UINT wType, UINT wFmt, HCONV hConv,
                HSZ hsz1, HSZ hsz2, HDDEDATA hdata, ULONG_PTR lData1, ULONG_PTR lData2 );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WREServer {
    char        *service;
    char        *topic;
    HSZ         hservice;
    HSZ         htopic;
} WREServer;

typedef struct WRETopic {
    char        *topic;
    HSZ         htopic;
} WRETopic;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WREServer EditServers[NUM_SERVERS] = {
    { WDE_SERVICE_NAME, WDE_SERVICE_TOPIC, NULL, NULL },
    { BMP_SERVICE_NAME, BMP_SERVICE_TOPIC, NULL, NULL },
    { CUR_SERVICE_NAME, CUR_SERVICE_TOPIC, NULL, NULL },
    { ICO_SERVICE_NAME, ICO_SERVICE_TOPIC, NULL, NULL }
};

static WRETopic Topics[NUM_TOPICS] = {
    { WRE_DIALOG_TOPIC, NULL },
    { WRE_BITMAP_TOPIC, NULL },
    { WRE_CURSOR_TOPIC, NULL },
    { WRE_ICON_TOPIC,   NULL }
};

static DWORD    IdInst                  = 0;
static FARPROC  DdeProc                 = NULL;
static HSZ      hServiceName            = NULL;
static HSZ      hFileItem               = NULL;
static HSZ      hIs32BitItem            = NULL;
static HSZ      hNameItem               = NULL;
static HSZ      hDataItem               = NULL;
static HSZ      hDialogDump             = NULL;
static HSZ      hImageDump              = NULL;
static WRESPT   PendingService          = NoServicePending;

WRESPT WREGetPendingService( void )
{
    return( PendingService );
}

void WRESetPendingService( WRESPT s )
{
    PendingService = s;
}

bool WREHData2Mem( HDDEDATA hData, void **data, uint_32 *size )
{
    DWORD   dde_size;

    if( data == NULL || size == NULL || hData == (HDDEDATA)NULL ) {
        return( FALSE );
    }

    *size = dde_size = DdeGetData( hData, NULL, 0, 0 );
    if( dde_size == 0 ) {
        return( FALSE );
    }

    *data = WRMemAlloc( dde_size );
    if( *data == NULL ) {
        return( FALSE );
    }

    if( dde_size != DdeGetData( hData, *data, dde_size, 0 ) ) {
        WRMemFree( *data );
        return( FALSE );
    }

    return( TRUE );
}

bool WREDDEStart( HINSTANCE inst )
{
    UINT        ret;
    DWORD       flags;
    int         i;

    _wre_touch( inst ); /* MakeProcInstance vanishes in NT */

    if( IdInst != 0 ) {
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

    for( i = 0; i < NUM_SERVERS; i++ ) {
        EditServers[i].htopic = DdeCreateStringHandle( IdInst, EditServers[i].topic, CP_WINANSI );
        if( EditServers[i].htopic == (HSZ)NULL ) {
            return( FALSE );
        }
        EditServers[i].hservice = DdeCreateStringHandle( IdInst, EditServers[i].service, CP_WINANSI );
        if( EditServers[i].hservice == (HSZ)NULL ) {
            return( FALSE );
        }
    }

    for( i = 0; i < NUM_TOPICS; i++ ) {
        Topics[i].htopic = DdeCreateStringHandle( IdInst, Topics[i].topic, CP_WINANSI );
        if( Topics[i].htopic == (HSZ)NULL ) {
            return( FALSE );
        }
    }

    hDialogDump = DdeCreateStringHandle( IdInst, WRE_DIALOG_DUMP, CP_WINANSI );
    if( hDialogDump == (HSZ)NULL ) {
        return( FALSE );
    }

    hImageDump = DdeCreateStringHandle( IdInst, WRE_IMAGE_DUMP, CP_WINANSI );
    if( hImageDump == (HSZ)NULL ) {
        return( FALSE );
    }

    hServiceName = DdeCreateStringHandle( IdInst, WRE_SERVICE_NAME, CP_WINANSI );
    if( hServiceName == (HSZ)NULL ) {
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

    hDataItem = DdeCreateStringHandle( IdInst, WRE_DATA_ITEM, CP_WINANSI );
    if( hDataItem == (HSZ)NULL ) {
        return( FALSE );
    }

    hNameItem = DdeCreateStringHandle( IdInst, WRE_NAME_ITEM, CP_WINANSI );
    if( hNameItem == (HSZ)NULL ) {
        return( FALSE );
    }

    DdeNameService( IdInst, hServiceName, (HSZ)NULL, DNS_REGISTER );

    return( TRUE );
}

void WREDDEEnd( void )
{
    int         i;

    if( IdInst != 0 ) {
        DdeNameService( IdInst, (HSZ)NULL, (HSZ)NULL, DNS_UNREGISTER );
        for( i = 0; i < NUM_TOPICS; i++ ) {
            if( Topics[i].htopic != (HSZ)NULL ) {
                DdeFreeStringHandle( IdInst, Topics[i].htopic );
            }
        }
        for( i = 0; i < NUM_SERVERS; i++ ) {
            if( EditServers[i].htopic != (HSZ)NULL ) {
                DdeFreeStringHandle( IdInst, EditServers[i].htopic );
            }
            if( EditServers[i].hservice != (HSZ)NULL ) {
                DdeFreeStringHandle( IdInst, EditServers[i].hservice );
            }
        }
        if( hDialogDump != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hDialogDump );
        }
        if( hImageDump != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hImageDump );
        }
        if( hFileItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hFileItem );
        }
        if( hIs32BitItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hIs32BitItem );
        }
        if( hDataItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hDataItem );
        }
        if( hNameItem != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hNameItem );
        }
        if( hServiceName != (HSZ)NULL ) {
            DdeFreeStringHandle( IdInst, hServiceName );
        }
        DdeUninitialize( IdInst );
    }
    if( DdeProc != (FARPROC)NULL ) {
        FreeProcInstance( DdeProc );
    }
}

bool WREPokeData( HCONV conv, void *data, int size, bool retry )
{
    DWORD       result;
    UINT        err;
    bool        timeout;
    bool        ret;
    UINT        tries;

    if( conv == (HCONV)NULL || data == NULL || size == 0 ) {
        return( FALSE );
    }

    if( retry ) {
        tries = 8;
    } else {
        tries = 0;
    }

    for( ;; ) {
        ret = ( DdeClientTransaction( (LPBYTE)data, size, conv, hDataItem, CF_TEXT, XTYP_POKE, LONG_TIME_OUT, &result ) != 0 );
        if( !ret && tries-- != 0 ) {
            err = DdeGetLastError( IdInst );
            timeout = ((err & DMLERR_POKEACKTIMEOUT) != 0);
            if( !timeout ) {
                break;
            }
        } else {
            break;
        }
    }

    return( ret );
}

HDDEDATA CALLBACK DdeCallBack( UINT wType, UINT wFmt, HCONV hConv,
                                HSZ hsz1, HSZ hsz2, HDDEDATA hdata,
                                ULONG_PTR lData1, ULONG_PTR lData2 )
{
    HDDEDATA    ret;
    HSZPAIR     hszpair[2];
    HSZ         htopic;
    HCONV       htconv;
    void        *data;
    uint_32     size;
    bool        ok;

    _wre_touch( hdata );
    _wre_touch( lData1 );
    _wre_touch( lData2 );

    ret = (HDDEDATA)FALSE;

    switch( wType ) {
    case XTYP_CONNECT:
    case XTYP_WILDCONNECT:
        if( PendingService == NoServicePending ) {
            htopic = (HSZ)NULL;
        } else {
            htopic = (HSZ)Topics[PendingService].htopic;
        }
        break;
    }

    switch( wType ) {
    case XTYP_CONNECT_CONFIRM:
        htconv = (HCONV)NULL;
        if( PendingService != NoServicePending ) {
                htconv = DdeConnect( IdInst, EditServers[PendingService].hservice,
                                     EditServers[PendingService].htopic, (LPVOID)NULL );
        }
        if( htconv != (HCONV)NULL ) {
            if( PendingService == DialogService ) {
                ok = WRECommitDialogSession( hConv, htconv );
            } else {
                ok = WRECommitImageSession( hConv, htconv );
            }
            if( !ok ) {
                DdeDisconnect( htconv );
                DdeDisconnect( hConv );
            }
        } else {
            DdeDisconnect( hConv );
        }
        break;

    case XTYP_DISCONNECT:
        if( !WREEndEditImageResource( hConv ) ) {
            WREEndEditDialogResource( hConv );
        }
        break;

    case XTYP_CONNECT:
        if( hsz1 == hDialogDump ) {
            WREDumpPendingDialogSession();
        } else if( hsz1 == hImageDump ) {
            WREDumpPendingImageSession();
        } else if( hsz1 == htopic ) {
            ret = (HDDEDATA)TRUE;
        }
        break;

    case XTYP_WILDCONNECT:
        if( hsz2 != hServiceName ) {
            break;
        }
        if( htopic == (HSZ)NULL ) {
            break;
        }
        hszpair[0].hszSvc = hServiceName;
        hszpair[0].hszTopic = htopic;
        hszpair[1].hszSvc = (HSZ)NULL;
        hszpair[1].hszTopic = (HSZ)NULL;
        ret = (HDDEDATA)DdeCreateDataHandle( IdInst, (LPBYTE)&hszpair[0], sizeof( hszpair ),
                                             0L, 0, CF_TEXT, 0 );
        break;

    case XTYP_REQUEST:
        data = NULL;
        size = 0;
        ok = false;
        if( hsz1 == Topics[DialogService].htopic ) {
            if( hsz2 == hFileItem ) {
                ok = WREGetDlgSessionFileName( hConv, &data, &size );
            } else if( hsz2 == hDataItem ) {
                ok = WREGetDlgSessionData( hConv, &data, &size );
            } else if( hsz2 == hNameItem ) {
                ok = WREGetDlgSessionResName( hConv, &data, &size );
            } else if( hsz2 == hIs32BitItem ) {
                ok = WREGetDlgSessionIs32Bit( hConv, &data, &size );
            }
        } else if( hsz1 == Topics[BitmapService].htopic ||
                   hsz1 == Topics[CursorService].htopic ||
                   hsz1 == Topics[IconService].htopic ) {
            if( hsz2 == hFileItem ) {
                ok = WREGetImageSessionFileName( hConv, &data, &size );
            } else if( hsz2 == hDataItem ) {
                ok = WREGetImageSessionData( hConv, &data, &size );
            } else if( hsz2 == hNameItem ) {
                ok = WREGetImageSessionResName( hConv, &data, &size );
            }
        }
        if( data != NULL ) {
            if( ok ) {
                ret = DdeCreateDataHandle( IdInst, (LPBYTE)data, size, 0, hsz2, wFmt, 0 );
            }
            WRMemFree( data );
        }
        break;

    case XTYP_POKE:
        data = NULL;
        size = 0;
        ok = false;
        ret = (HDDEDATA)DDE_FNOTPROCESSED;
        if( hsz1 == Topics[DialogService].htopic ) {
            if( hsz2 == hDataItem ) {
                ok = WRESetDlgSessionResData( hConv, hdata );
            } else if( hsz2 == hNameItem ) {
                ok = WRESetDlgSessionResName( hConv, hdata );
            }
        } else if( hsz1 == Topics[BitmapService].htopic ||
                   hsz1 == Topics[CursorService].htopic ||
                   hsz1 == Topics[IconService].htopic ) {
            if( hsz2 == hDataItem ) {
                ok = WRESetImageSessionResData( hConv, hdata );
            } else if( hsz2 == hNameItem ) {
                ok = WRESetImageSessionResName( hConv, hdata );
            }
        }
        ret = (HDDEDATA)DDE_FACK;
        break;
    }

    return( ret );
}
