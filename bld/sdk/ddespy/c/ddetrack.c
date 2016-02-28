/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DDE Spy window tracking.
*
****************************************************************************/


#include "commonui.h"
#include "wddespy.h"
#include <string.h>
#include <stdio.h>

#define MAX_TRK_STR     10

#define STR_HDR_CNT     3
#define CNV_HDR_CNT     4
#define LNK_HDR_CNT     7
#define SRV_HDR_CNT     2

typedef int (*comp_fn)( const void *, const void * );

static TrackHeaderInfo StrHdr[STR_HDR_CNT] = {
    STR_HANDLE,         PUSH_STR_HDL,   9,
    STR_COUNT,          PUSH_STR_CNT,   7,
    STR_STRING,         PUSH_STR_TEXT,  9
};

static TrackHeaderInfo ConvHdr[CNV_HDR_CNT] = {
    STR_CLIENT,         PUSH_CLIENT,    9,
    STR_SERVER,         PUSH_SERVER,    9,
    STR_SERVICE,        PUSH_SERVICE,   21,
    STR_TOPIC,          PUSH_TOPIC,     10
};

static TrackHeaderInfo LnkHdr[] = {
    STR_CLIENT,         PUSH_CLIENT,    9,
    STR_SERVER,         PUSH_SERVER,    9,
    STR_TYPE,           PUSH_TYPE,      5,
    STR_SERVICE,        PUSH_SERVICE,   11,
    STR_TOPIC,          PUSH_TOPIC,     11,
    STR_ITEM,           PUSH_ITEM,      11,
    STR_FORMAT,         PUSH_FORMAT,    11
};

static TrackHeaderInfo SrvHdr[] = {
    STR_SERVER_NAME,    PUSH_SERVER,    20,
    STR_INSTANCE,       PUSH_INST,      20
};

static void makePushWin( DDETrackInfo *info, HWND hwnd, TrackHeaderInfo *hdrinfo, WORD hdrcnt );

/*
 * SetTrackWndDefault - set the tracking window size, pos, etc. to the defaults
 */
void SetTrackWndDefault( void )
{
    int     i;

    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        Tracking[i].visible = false;
        Tracking[i].xpos = 0;
        Tracking[i].ypos = i * GetSystemMetrics( SM_CYCAPTION );
        Tracking[i].xsize = GetSystemMetrics( SM_CXSCREEN );
        Tracking[i].ysize = 150;
    }

} /* SetTrackWndDefault */

/*
 * InitTrackWnd - update the checkmarks in the Tracking menu and
 *                 and the visibility state of the tracking windows so it
 *                 corresponds to the information contained in the Tracking
 *                 array
 */
void InitTrackWnd( HWND hwnd )
{
    unsigned    i;
    HMENU       mh;

    mh = GetMenu( hwnd );
    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        if( Tracking[i].visible ) {
            ShowWindow( Tracking[i].hwnd, SW_SHOWNORMAL );
            CheckMenuItem( mh, DDE_TRK_FIRST + i, MF_BYCOMMAND | MF_CHECKED );
        } else {
            ShowWindow( Tracking[i].hwnd, SW_HIDE );
            CheckMenuItem( mh, DDE_TRK_FIRST + i, MF_BYCOMMAND | MF_UNCHECKED );
        }
        DDESetStickyState( DDE_TRK_FIRST + i, Tracking[i].visible );
    }

} /* InitTrackWnd */

/*
 * CreateTrackWnd - create the tracking windows
 *                 - the tracking windows are always present when the program
 *                   is running
 *                 - when not visible they are hidden, not destroyed
 */
BOOL CreateTrackWnd( void )
{
    WORD        type;
    int         i;

    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        type = DDE_TRK_FIRST + i;
        Tracking[i].hwnd = CreateWindow(
            TRACKING_CLASS,             /* Window class name */
            "",                         /* Window caption */
            WS_OVERLAPPED | WS_CAPTION |
            WS_SYSMENU | WS_THICKFRAME |
            WS_MAXIMIZEBOX,             /* Window style */
            Tracking[i].xpos,           /* Initial X position */
            Tracking[i].ypos,           /* Initial Y position */
            Tracking[i].xsize,          /* Initial X size */
            Tracking[i].ysize,          /* Initial Y size */
            NULL,                       /* Parent window handle */
            NULL,                       /* Window menu handle */
            Instance,                   /* Program instance handle */
            &type );                    /* Create parameters */
        if( Tracking[i].hwnd == NULL ) {
            return( FALSE );
        }
    }
    return( TRUE );

} /* CreateTrackWnd */

/*
 * FiniTrackWnd - destroy the tracking windows
 */
void FiniTrackWnd( void )
{
    int     i;

    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        DestroyWindow( Tracking[i].hwnd );
    }

} /* FiniTrackWnd */

/*
 * SetTrackFont - redraw the tracking windows after a font change
 */
void SetTrackFont( void )
{
    int                 i;
    DDETrackInfo        *info;
    HFONT               font;
    RECT                area;

    font = GetMonoFont();
    for( i = 0; i < MAX_DDE_TRK; i++ ) {
        info = (DDETrackInfo *)GET_WNDINFO( Tracking[i].hwnd );
        makePushWin( info, Tracking[i].hwnd, info->hdrinfo, info->hdrcnt );
        GetClientRect( Tracking[i].hwnd, &area );
        ResizeListBox( area.right - area.left, area.bottom - area.top, &info->list );
        SendMessage( info->list.box, WM_SETFONT, (WPARAM)font, MAKELONG( TRUE, 0 ) );
    }

} /* SetTrackFont */

/*
 * getNextPos - get a pointer to the next available pointer position
 *              in the element list for a tracking window
 */
static void *getNextPos( DDETrackInfo *listinfo )
{
    WORD                i;
    void                **data;

    data = listinfo->data;
    for( i = 0; i < listinfo->cnt; i++ ) {
        if( data[i] == NULL ) {
            return( data + i );
        }
    }
    listinfo->cnt++;
    listinfo->data = MemReAlloc( listinfo->data,
                                 listinfo->cnt * sizeof( void * ) );
    return( (void **)listinfo->data + listinfo->cnt - 1 );

} /* getNextPos */

#define SRV(x) (*(const ServerInfo * const *)(x))
/*
 * sortServerByServer
 */
static int sortServerByServer( const void *srv1, const void *srv2 )
{
    if( SRV( srv1 ) == NULL ) {
        return( 1 );
    }
    if( SRV( srv2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( SRV( srv1 )->server, SRV( srv2 )->server ) );
}

/*
 * sortServerByInst
 */
static int sortServerByInst( const void *srv1, const void *srv2 )
{
    if( SRV( srv1 ) == NULL ) {
        return( 1 );
    }
    if( SRV( srv2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( SRV( srv1 )->instname, SRV( srv2 )->instname ) );
}
#undef SRV

/*
 * displayServers - refresh the information in the servers tracking window
 */
static void displayServers( DDETrackInfo *info )
{
    unsigned    i;
    size_t      len;
    char        buf[80];
    ServerInfo  **servers;
    ServerInfo  *cur;
    comp_fn     fn;

    switch( info->sorttype ) {
    default:
    case PUSH_SERVER:
        fn = sortServerByServer;
        break;
    case PUSH_INST:
        fn = sortServerByInst;
        break;
    }
    qsort( info->data, info->cnt, sizeof( ServerInfo * ), fn );
    SendMessage( info->list.box, LB_RESETCONTENT, 0, 0L );
    servers = info->data;
    for( i = 0; i < info->cnt; i++ ) {
        cur = servers[i];
        if( cur == NULL ) {
            break;
        }
        memset( buf, ' ', 80 * sizeof( char ) );
        len = strlen( cur->server );
        strcpy( buf, cur->server );
        buf[len] = ' ';
        strcpy( buf + 20, cur->instname );
        SendMessage( info->list.box, LB_ADDSTRING, 0, (LPARAM)(LPSTR)buf );
    }

} /* displayServers */

/*
 * doServerSort - change the sort type for the server tracking window
 */
static void doServerSort( WORD type, DDETrackInfo *info )
{
    if( type == PUSH_SERVER || type == PUSH_INST ) {
        info->sorttype = type;
        displayServers( info );
    }

} /* doServerSort */

/*
 * findServer - find the information about the server with instance string inst
 *            - if we don't know about this server return NULL
 */
static ServerInfo **findServer( char *inst, DDETrackInfo *info )
{
    unsigned            i;
    ServerInfo          **list;

    list = info->data;
    for( i = 0; i < info->cnt; i++ ) {
        if( list[i] != NULL ) {
            if( !strcmp( inst, list[i]->instname ) ) {
                return( list + i );
            }
        }
    }
    return( NULL );

} /* findServer */

/*
 * TrackServerMsg - update the coversations tracking window
 */
void TrackServerMsg( MONCBSTRUCT *info )
{
    char                *inst;
    DDETrackInfo        *listinfo;
    ServerInfo          **entry;
    ServerInfo          *cur;

    inst = HSZToString( info->hsz2 );
    listinfo = (DDETrackInfo *)GET_WNDINFO( Tracking[DDEMENU_TRK_SERVER - DDE_TRK_FIRST].hwnd );
    entry = findServer( inst, listinfo );

    if( info->wType == XTYP_REGISTER ) {
        if( entry == NULL ) {
           entry = getNextPos( listinfo );
           cur = MemAlloc( sizeof( ServerInfo ) );
           *entry = cur;
           cur->instname = inst;
           cur->server = HSZToString( info->hsz1 );
        } else {
            MemFree( inst );
        }
    } else if( info->wType == XTYP_UNREGISTER ) {
        if( entry != NULL ) {
            cur = *entry;
            MemFree( cur->instname );
            MemFree( cur->server );
            *entry = NULL;
        } else {
            MemFree( inst );
        }
    }
    displayServers( listinfo );

} /* TrackServerMsg */

/*
 * addStringInfo - add information about a new string to the list of
 *                 strings displayed in the string tracking window
 */
static StringInfo *addStringInfo( MONHSZSTRUCT *info, DDETrackInfo *listinfo )
{
    StringInfo          **str;
    StringInfo          *ret;
#ifdef __NT__
    WORD                *ptr;
    DWORD               len;
    DWORD               ver;
#endif

    str = getNextPos( listinfo );
    ret = MemAlloc( sizeof( StringInfo ) );
    *str = ret;
    ret->hsz = info->hsz;
    ret->cnt = 1;
    ret->str = NULL;
#ifdef __NT__
    /* In NT 3.1, ret->str is a Unicode string. Otherwise it is ASCII. */
    ver = GetVersion();
    if( (ver & 0xFF) == 3 && (ver & 0xFF00) <= 0x0A00 ) {
        len = 0;
        ptr = (WORD *)info->str;
        while( *ptr != 0 ) {
            ptr++;
            len++;
        }
        ret->str = MemAlloc( len + 1 );
        wsprintf( ret->str, "%ls", info->str );
    }
#endif
    if( ret->str == NULL ) {
        ret->str = MemAlloc( strlen( info->str ) + 1 );
        strcpy( ret->str, info->str );
    }
    return( ret );

} /* addStringInfo */

/*
 * getStringInfo - check if information about a string is already
 *                 displayed in the strings tracking window
 */
static StringInfo **getStringInfo( HSZ hsz, DDETrackInfo *info )
{
    WORD                i;
    StringInfo          **str;

    str = info->data;
    for( i = 0; i < info->cnt; i++ ) {
        if( str[i] != NULL && str[i]->hsz == hsz ) {
            return( str + i );
        }
    }
    return( NULL );

} /* getStringInfo */

#define STR(x) (*(const StringInfo * const *)(x))
/*
 * sortStrByText
 */
static int sortStrByText( const void *str1, const void *str2 )
{
    if( STR( str1 ) == NULL ) {
        return( 1 );
    }
    if( STR( str2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( STR( str1 )->str, STR( str2 )->str ) );

} /* sortStrByText */

/*
 * sortStrByCnt
 */
static int sortStrByCnt( const void *str1, const void *str2 )
{
    if( STR( str1 ) == NULL ) {
        return( 1 );
    }
    if( STR( str2 ) == NULL ) {
        return( -1 );
    }
    return( STR( str1 )->cnt - STR( str2 )->cnt );

} /* sortStrByCnt */

/*
 * sortStrByHSZ
 */
static int sortStrByHSZ( const void *str1, const void *str2 )
{
    if( STR( str1 ) == NULL ) {
        return( 1 );
    }
    if( STR( str2 ) == NULL ) {
        return( -1 );
    }
    return( (char *)STR( str1 )->hsz - (char *)STR( str2 )->hsz );

} /* sortStrByHSZ */
#undef STR

/*
 * redispStrTrk - sort then redisplay information in the string tracking window
 */
static void redispStrTrk( DDETrackInfo *info )
{
    comp_fn     fn;
    StringInfo  **items;
    unsigned    i;
    char        buf[80];

    SendMessage( info->list.box, LB_RESETCONTENT, 0, 0L );
    switch( info->sorttype ) {
    default:
    case PUSH_STR_TEXT:
        fn = sortStrByText;
        break;
    case PUSH_STR_CNT:
        fn = sortStrByCnt;
        break;
    case PUSH_STR_HDL:
        fn = sortStrByHSZ;
        break;
    }
    qsort( info->data, info->cnt, sizeof( StringInfo * ), fn );
    items = (StringInfo **)info->data;
    for( i = 0; i < info->cnt; i++ ) {
        if( items[i] == NULL ) {
            break;
        }
        sprintf( buf, "%0*lX    %4d    %s", HSZ_FMT_LEN, items[i]->hsz,
                 items[i]->cnt, items[i]->str );
        SendMessage( info->list.box, LB_ADDSTRING, 0, (LPARAM)(LPSTR)buf );
    }

} /* redispStrTrk */

/*
 * freeStringItem - free memory allocated to a string item
 */
static void freeStringItem( StringInfo *info )
{
    MemFree( info->str );
    MemFree( info );

} /* freeStringItem */

/*
 * TrackStringMsg - update the strings tracking window
 */
void TrackStringMsg( MONHSZSTRUCT *info )
{
    DDETrackInfo        *listinfo;
    StringInfo          **pos;
    StringInfo          *str;

    listinfo = (DDETrackInfo *)GET_WNDINFO( Tracking[DDEMENU_TRK_STR - DDE_TRK_FIRST].hwnd );
    pos = getStringInfo( info->hsz, listinfo );
    if( pos == NULL ) {
        str = NULL;
    } else {
        str = *pos;
    }
    switch( info->fsAction ) {
    case MH_CREATE:
    case MH_KEEP:
        if( str != NULL ) {
            str->cnt++;
        } else {
            str = addStringInfo( info, listinfo );
        }
        break;
    case MH_CLEANUP:
    case MH_DELETE:
        if( str != NULL ) {
            if( str->cnt > 0 ) {
                str->cnt--;
            }
            if( str->cnt == 0 ) {
                freeStringItem( str );
                *pos = NULL;
            }
        }
        break;
    }
    redispStrTrk( listinfo );

} /* TrackStringMsg */

/*
 * doStrSort - handle a change in the sort type of the string tracking window
 */
static bool doStrSort( WORD type, DDETrackInfo *info )
{
    switch( type ) {
    case PUSH_STR_HDL:
    case PUSH_STR_CNT:
    case PUSH_STR_TEXT:
        info->sorttype = type;
        redispStrTrk( info );
        break;
    default:
        return( false );
    }
    return( true );

} /* doStrSort */

#define LNK(x) (*(const LinkInfo * const *)(x))
/*
 * sortLinkByClient
 */
static int sortLinkByClient( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( (char *)LNK( lnk1 )->client - (char *)LNK( lnk2 )->client );

} /* sortLinkByClient */

/*
 * sortLinkByServer
 */
static int sortLinkByServer( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( (char *)LNK( lnk1 )->server - (char *)LNK( lnk2 )->server );

} /* sortLinkByServer */

/*
 * sortLinkByServer
 */
static int sortLinkByService( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( LNK( lnk1 )->service, LNK( lnk2 )->service ) );

} /* sortLinkByServer */

/*
 * sortLinkByTopic
 */
static int sortLinkByTopic( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( LNK( lnk1 )->topic, LNK( lnk2 )->topic ) );

} /* sortLinkByTopic */

/*
 * sortLinkByItem
 */
static int sortLinkByItem( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( LNK( lnk1 )->item, LNK( lnk2 )->item ) );

} /* sortLinkByItem */

/*
 * sortLinkByType
 */
static int sortLinkByType( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( LNK( lnk1 )->type, LNK( lnk2 )->type ) );

} /* sortLinkByType */

/*
 * sortLinkByFormat
 */
static int sortLinkByFormat( const void *lnk1, const void *lnk2 )
{
    if( LNK( lnk1 ) == NULL ) {
        return( 1 );
    }
    if( LNK( lnk2 ) == NULL ) {
        return( -1 );
    }
    return( stricmp( LNK( lnk1 )->format, LNK( lnk2 )->format ) );

} /* sortLinkByFormat */
#undef LNK

/*
 * redispLinkTrk - sort then redisplay information in the link tracking
 *                 window (if islink is true) or refresh the
 *                 conversation tracking window (otherwise)
 */
static void redispLinkTrk( DDETrackInfo *info, bool islink )
{
    comp_fn     fn;
    unsigned    i;
    char        buf[100];
    LinkInfo    **items;

    SendMessage( info->list.box, LB_RESETCONTENT, 0, 0L );
    switch( info->sorttype ) {
    default:
    case PUSH_CLIENT:
         fn = sortLinkByClient;
         break;
    case PUSH_SERVER:
         fn = sortLinkByServer;
         break;
    case PUSH_SERVICE:
         fn = sortLinkByService;
         break;
    case PUSH_TOPIC:
         fn = sortLinkByTopic;
         break;
    case PUSH_TYPE:
         fn = sortLinkByType;
         break;
    case PUSH_ITEM:
         fn = sortLinkByItem;
         break;
    case PUSH_FORMAT:
         fn = sortLinkByFormat;
         break;
    }
    qsort( info->data, info->cnt, sizeof( LinkInfo * ), fn );
    items = (LinkInfo **)info->data;
    for( i = 0; i < info->cnt; i++ ) {
        if( items[i] == NULL ) {
            break;
        }
        if( islink ) {
            sprintf( buf, "%08lX %08lX %-4s %-10s %-10s %-10s %-18s",
                     items[i]->client, items[i]->server,
                     items[i]->type, items[i]->service, items[i]->topic,
                     items[i]->item, items[i]->format );
        } else {
            sprintf( buf, "%08lX %08lX %-20s %-s", items[i]->client,
                     items[i]->server, items[i]->service, items[i]->topic );
        }
        SendMessage( info->list.box, LB_ADDSTRING, 0, (LPARAM)(LPSTR)buf );
    }

} /* redispLinkTrk */

/*
 * FreeLinkInfo - free memory holding information about a link
 */
void FreeLinkInfo( LinkInfo *info )
{
    MemFree( info->service );
    MemFree( info->topic );
    if( info->item != NULL ) {
        MemFree( info->item );
    }
    if( info->type != NULL ) {
        FreeRCString( info->type );
    }
    MemFree( info );

} /* FreeLinkInfo */

/*
 * findLinkInfo - find the location of information about a given link
 *                if the link is already recorded
 */
static LinkInfo **findLinkInfo( DDETrackInfo *info, MONLINKSTRUCT *find )
{
    unsigned    i;
    LinkInfo    *cur;
    char        *service;
    char        *topic;
    char        *item;

    service = HSZToString( find->hszSvc );
    topic = HSZToString( find->hszTopic );
    item = HSZToString( find->hszItem );
    for( i = 0; i < info->cnt; i++ ) {
        cur = ((void **)info->data)[i];
        if( cur == NULL ) {
            break;
        }
        if( cur->client == find->hConvClient && cur->server == find->hConvServer &&
            !stricmp( service, cur->service ) && !stricmp( topic, cur->topic ) &&
            !stricmp( item, cur->item ) ) {
            return( (LinkInfo **)info->data + i );
        }
    }
    MemFree( service );
    MemFree( topic );
    MemFree( item );
    return( NULL );

} /* findLinkInfo */

/*
 * TrackLinkMsg - update the links tracking window
 */
void TrackLinkMsg( MONLINKSTRUCT *info )
{
    DDETrackInfo        *listinfo;
    LinkInfo            *item;
    LinkInfo            **itempos;

    listinfo = (DDETrackInfo *)GET_WNDINFO( Tracking[DDEMENU_TRK_LINK - DDE_TRK_FIRST].hwnd );
    itempos = findLinkInfo( listinfo, info );
    if( info->fEstablished ) {
        if( itempos != NULL ) {
            return;
        }
        itempos = getNextPos( listinfo );
        item = MemAlloc( sizeof( LinkInfo ) );
        *itempos = item;

        item->service = HSZToString( info->hszSvc );
        item->topic = HSZToString( info->hszTopic );
        item->item = HSZToString( info->hszItem );
        item->format = MemAlloc( 20 );
        GetFmtStr( info->wFmt, item->format );
        if( info->fNoData ) {
            item->type = AllocRCString( STR_WARM );
        } else {
            item->type = AllocRCString( STR_HOT );
        }
        item->client = info->hConvClient;
        item->server = info->hConvServer;

        if( strlen( item->service ) > MAX_TRK_STR ) {
            item->service[MAX_TRK_STR] = '\0';
        }
        if( strlen( item->topic ) > MAX_TRK_STR ) {
            item->topic[MAX_TRK_STR] = '\0';
        }
        if( strlen( item->item ) > MAX_TRK_STR ) {
            item->item[MAX_TRK_STR] = '\0';
        }
    } else {
        if( itempos != NULL ) {
            FreeLinkInfo( *itempos );
            *itempos = NULL;
        }
    }
    redispLinkTrk( listinfo, true );

} /* TrackLinkMsg */

/*
 * doLinkSort - handle a change in the sort type of the link tracking window
 */
static bool doLinkSort( WORD type, DDETrackInfo *info )
{
    switch( type ) {
    case PUSH_CLIENT:
    case PUSH_SERVER:
    case PUSH_SERVICE:
    case PUSH_TOPIC:
    case PUSH_TYPE:
    case PUSH_ITEM:
    case PUSH_FORMAT:
        info->sorttype = type;
        redispLinkTrk( info, true );
        break;
    default:
        return( false );
    }
    return( true );

} /* doLinkSort */

/*
 * findConvInfo - find the location of information about a given conversation
 *                if the conversation is already recorded
 */
static LinkInfo **findConvInfo( DDETrackInfo *info, MONCONVSTRUCT *find )
{
    unsigned    i;
    LinkInfo    *cur;
    char        *service;
    char        *topic;

    service = HSZToString( find->hszSvc );
    topic = HSZToString( find->hszTopic );
    for( i = 0; i < info->cnt; i++ ) {
        cur = ((LinkInfo **)info->data)[i];
        if( cur == NULL ) {
            break;
        }
        if( cur->client == find->hConvClient && cur->server == find->hConvServer &&
            !stricmp( service, cur->service ) && !stricmp( topic, cur->topic ) ) {
            return( (LinkInfo **)info->data + i );
        }
    }
    MemFree( service );
    MemFree( topic );
    return( NULL );

} /* findConvInfo */

/*
 * TrackConvMsg - update the coversations tracking window
 */
void TrackConvMsg( MONCONVSTRUCT *info )
{
    LinkInfo            *item;
    LinkInfo            **itempos;
    DDETrackInfo        *listinfo;

    listinfo = (DDETrackInfo *)GET_WNDINFO( Tracking[DDEMENU_TRK_CONV - DDE_TRK_FIRST].hwnd );
    itempos = findConvInfo( listinfo, info );
    if( info->fConnect ) {
        if( itempos != NULL ) {
            return;
        }
        itempos = getNextPos( listinfo );
        item = MemAlloc( sizeof( LinkInfo ) );
        *itempos = item;

        item->service = HSZToString( info->hszSvc );
        item->topic = HSZToString( info->hszTopic );
        item->client = info->hConvClient;
        item->server = info->hConvServer;
        item->type = NULL;
        item->item = NULL;
        item->format = NULL;
        if( strlen( item->service ) > MAX_TRK_STR ) {
            item->service[MAX_TRK_STR] = '\0';
        }
        if( strlen( item->topic ) > MAX_TRK_STR ) {
            item->topic[MAX_TRK_STR] = '\0';
        }
    } else {
        if( itempos != NULL ) {
            FreeLinkInfo( *itempos );
            *itempos = NULL;
        }
    }
    redispLinkTrk( listinfo, false );

} /* TrackConvMsg */

/*
 * doConvSort - change the sort type for the conversation tracking window
 */
static bool doConvSort( WORD type, DDETrackInfo *info )
{
    switch( type ) {
    case PUSH_CLIENT:
    case PUSH_SERVER:
    case PUSH_TOPIC:
    case PUSH_SERVICE:
        info->sorttype = type;
        redispLinkTrk( info, false );
        break;
    default:
        return( false );
    }
    return( true );

} /* doConvSort */

/*
 * DisplayTracking - toggle a tracking window between displayed and hidden
 */
void DisplayTracking( unsigned i )
{
    int         action;
    HMENU       mh;

    mh = GetMenu( DDEMainWnd );
    Tracking[i].visible = !Tracking[i].visible;
    if( Tracking[i].visible ) {
        CheckMenuItem( mh, DDE_TRK_FIRST + i, MF_BYCOMMAND | MF_CHECKED );
        action = SW_SHOWNORMAL;
    } else {
        CheckMenuItem( mh, DDE_TRK_FIRST + i, MF_BYCOMMAND | MF_UNCHECKED );
        action = SW_HIDE;
    }
    DDESetStickyState( DDE_TRK_FIRST + i, Tracking[i].visible  );
    ShowWindow( Tracking[i].hwnd, action );

} /* DisplayTracking */

/*
 * makePushWin - create the push window headers for a window
 */
static void makePushWin( DDETrackInfo *info, HWND hwnd,
                         TrackHeaderInfo *hdrinfo, WORD hdrcnt )
{
    WORD        i;
    SIZE        sz;
    char        test[40];
    int         left;
    HDC         dc;
    HFONT       font;
    HFONT       old_font;
    char        *win_title;

    if( info->hdr == NULL ) {
        info->hdr = MemAlloc( hdrcnt * sizeof( HWND ) );
        info->hdrcnt = hdrcnt;
        info->hdrinfo = hdrinfo;
    }
    left = 0;
    font = GetMonoFont();
    for( i = 0; i < hdrcnt; i++ ) {
        if( IsWindow( info->hdr[i] ) ) {
            DestroyWindow( info->hdr[i] );
        }
        win_title = AllocRCString( hdrinfo[i].textid );
        info->hdr[i] = CreatePushWin( hwnd, win_title, hdrinfo[i].id, font, Instance );
        FreeRCString( win_title );
        memset( test, '0', hdrinfo[i].size );
        test[hdrinfo[i].size] = '\0';
        dc = GetDC( info->hdr[i] );
        old_font = SelectObject( dc, font );
        GetTextExtentPoint( dc, test, hdrinfo[i].size, &sz );
        SelectObject( dc, old_font );
        ReleaseDC( info->hdr[i], dc );
        MoveWindow( info->hdr[i], left, 0, sz.cx, sz.cy, TRUE );
        ShowWindow( info->hdr[i], SW_SHOW );
        left += sz.cx;
    }
    info->list.ypos = sz.cy + 1;

} /* makePushWin */

/*
 * DDETrackingWndProc - handle messages from the tracking windows
 */
LRESULT CALLBACK DDETrackingWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    DDETrackInfo                *info;
    WORD                        i;
    void                        **ptr;
    RECT                        area;
    WORD                        cmd;
    char                        *wintitle;

    info = (DDETrackInfo *)GET_WNDINFO( hwnd );
    switch( msg ) {
    case WM_CREATE:
        info = MemAlloc( sizeof( DDETrackInfo ) );
        memset( info, 0, sizeof( DDETrackInfo ) );
        info->type = *(WORD *)((CREATESTRUCT *)lparam)->lpCreateParams;
        SET_WNDINFO( hwnd, (LONG_PTR)info );
        switch( info->type ) {
        case DDEMENU_TRK_CONV:
            info->sorttype = PUSH_CLIENT;
            makePushWin( info, hwnd, ConvHdr, CNV_HDR_CNT );
            wintitle = GetRCString( STR_CONVERSATIONS );
            break;
        case DDEMENU_TRK_STR:
            info->sorttype = PUSH_STR_TEXT;
            makePushWin( info, hwnd, StrHdr, STR_HDR_CNT );
            wintitle = GetRCString( STR_STRINGS );
            break;
        case DDEMENU_TRK_LINK:
            info->sorttype = PUSH_CLIENT;
            makePushWin( info, hwnd, LnkHdr, LNK_HDR_CNT );
            wintitle = GetRCString( STR_LINKS );
            break;
        case DDEMENU_TRK_SERVER:
            info->sorttype = PUSH_INST;
            makePushWin( info, hwnd, SrvHdr, SRV_HDR_CNT );
            wintitle = GetRCString( STR_REG_SERVERS );
            break;
        }
        SetWindowText( hwnd, wintitle );
        CreateListBox( hwnd, &info->list );
        info->config = &Tracking[info->type - DDE_TRK_FIRST];
        if( info->config->visible ) {
            ShowWindow( hwnd, SW_SHOWNORMAL );
        }
        break;
    case WM_MOVE:
        GetWindowRect( hwnd, &area );
        info->config->xpos = area.left;
        info->config->ypos = area.top;
        break;
    case WM_SIZE:
        ResizeListBox( LOWORD( lparam ), HIWORD( lparam ), &info->list );
        GetWindowRect( hwnd, &area );
        if( wparam != SIZE_MAXIMIZED && wparam != SIZE_MINIMIZED ) {
            info->config->xsize = area.right - area.left;
            info->config->ysize = area.bottom - area.top;
        }
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( info->type ) {
        case DDEMENU_TRK_STR:
            doStrSort( cmd, info );
            break;
        case DDEMENU_TRK_CONV:
            doConvSort( cmd, info );
            break;
        case DDEMENU_TRK_LINK:
            doLinkSort( cmd, info );
            break;
        case DDEMENU_TRK_SERVER:
            doServerSort( cmd, info );
            break;
        }
        break;
    case WM_CLOSE:
        DisplayTracking( info->type - DDE_TRK_FIRST );
        break;
    case WM_DESTROY:
        ptr = info->data;
        for( i = 0; i < info->cnt; i++ ) {
            MemFree( ptr[i] );
        }
        if( ptr != NULL ) {
            MemFree( ptr );
        }
        if( info->hdr != NULL ) {
            MemFree( info->hdr );
        }
        MemFree( info );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( TRUE );

} /* DDETrackingWndProc */
