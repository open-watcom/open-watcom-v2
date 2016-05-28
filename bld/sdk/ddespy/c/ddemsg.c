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
* Description:  Process messages from DDEML.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <stdio.h>
#include "wddespy.h"
#include "watcom.h"

#define HWND_LEN        10
#define CONV_LEN        10
#define TASK_LEN        10

/*
 * The values in this enum are used as indices into the aliasHdlTable array.
 */
enum {
    DEALIAS_HWND,
    DEALIAS_TASK,
    DEALIAS_CONV,
    DEALIAS_CLIENT_CONV,
    DEALIAS_SERVER_CONV
};

static AliasHdl *aliasHdlTable[] = {
    &HwndAlias,
    &TaskAlias,
    &ConvAlias,
    &ConvAlias,
    &ConvAlias
};


typedef struct {
    WORD        type;
    HWND        lb;
} ReplaceInfo;

static ReplaceInfo      taskReplace;
static ReplaceInfo      convReplace;
static ReplaceInfo      hwndReplace;

msglist DDEMsgs[] = {
    WM_DDE_ACK,                 (char *)(pointer_int)STR_ACK,
    WM_DDE_ADVISE,              (char *)(pointer_int)STR_ADVISE,
    WM_DDE_DATA,                (char *)(pointer_int)STR_DATA,
    WM_DDE_EXECUTE,             (char *)(pointer_int)STR_EXECUTE,
    WM_DDE_INITIATE,            (char *)(pointer_int)STR_INITIATE,
    WM_DDE_POKE,                (char *)(pointer_int)STR_POKE,
    WM_DDE_REQUEST,             (char *)(pointer_int)STR_REQUEST,
    WM_DDE_TERMINATE,           (char *)(pointer_int)STR_TERMINATE,
    WM_DDE_UNADVISE,            (char *)(pointer_int)STR_UNADVISE,
    0,                          (char *)(pointer_int)-1
};

static msglist FormatMsgs[] = {
    CF_BITMAP,                  "CF_BITMAP",
    CF_DIB,                     "CF_DIB",
    CF_DIF,                     "CF_DIF",
    CF_DSPBITMAP,               "CF_DSPBITMAP",
    CF_DSPMETAFILEPICT,         "CF_DSPMETAFILEPICT",
    CF_DSPTEXT,                 "CF_DSPTEXT",
    CF_METAFILEPICT,            "CF_METAFILEPICT",
    CF_OEMTEXT,                 "CF_OEMTEXT",
    CF_OWNERDISPLAY,            "CF_OWNERDISPLAY",
    CF_PALETTE,                 "CF_PALETTE",
    CF_PENDATA,                 "CF_PENDATA",
    CF_RIFF,                    "CF_RIFF",
    CF_SYLK,                    "CF_SYLK",
    CF_TEXT,                    "CF_TEXT",
    CF_TIFF,                    "CF_TIFF",
    CF_WAVE,                    "CF_WAVE",
    0,                          NULL
};

static msglist XTypMsgs[] = {
    XTYP_ADVSTART,              "XTYP_ADVSTART",
    XTYP_CONNECT,               "XTYP_CONNECT",
    XTYP_ADVREQ,                "XTYP_ADVREQ",
    XTYP_REQUEST,               "XTYP_REQUEST",
    XTYP_WILDCONNECT,           "XTYP_WILDCONNECT",
    XTYP_ADVDATA,               "XTYP_ADVDATA",
    XTYP_EXECUTE,               "XTYP_EXECUTE",
    XTYP_POKE,                  "XTYP_POKE",
    XTYP_ADVSTOP,               "XTYP_ADVSTOP",
    XTYP_CONNECT_CONFIRM,       "XTYP_CONNECT_CONFIRM",
    XTYP_DISCONNECT,            "XTYP_DISCONNECT",
    XTYP_ERROR,                 "XTYP_ERROR",
    XTYP_XACT_COMPLETE,         "XTYP_XACT_COMPLETE",
    XTYP_UNREGISTER,            "XTYP_UNREGISTER",
    XTYP_REGISTER,              "XTYP_REGISTER",
    0,                          NULL
};

static msglist DDEErrorMsgs[] = {
    DMLERR_ADVACKTIMEOUT,       "DMLERR_ADVACKTIMEOUT",
    DMLERR_BUSY,                "DMLERR_BUSY",
    DMLERR_DATAACKTIMEOUT,      "DMLERR_DATAACKTIMEOUT",
    DMLERR_DLL_NOT_INITIALIZED, "DMLERR_DLL_NOT_INITIALIZED",
    DMLERR_DLL_USAGE,           "DMLERR_DLL_USAGE",
    DMLERR_EXECACKTIMEOUT,      "DMLERR_EXECACKTIMEOUT",
    DMLERR_INVALIDPARAMETER,    "DMLERR_INVALIDPARAMETER",
    DMLERR_LOW_MEMORY,          "DMLERR_LOW_MEMORY",
    DMLERR_MEMORY_ERROR,        "DMLERR_MEMORY_ERROR",
    DMLERR_NO_CONV_ESTABLISHED, "DMLERR_NO_CONV_ESTABLISHED",
    DMLERR_NOTPROCESSED,        "DMLERR_NOTPROCESSED",
    DMLERR_POKEACKTIMEOUT,      "DMLERR_POKEACKTIMEOUT",
    DMLERR_POSTMSG_FAILED,      "DMLERR_POSTMSG_FAILED",
    DMLERR_REENTRANCY,          "DMLERR_REENTRANCY",
    DMLERR_SERVER_DIED,         "DMLERR_SERVER_DIED",
    DMLERR_SYS_ERROR,           "DMLERR_SYS_ERROR",
    DMLERR_UNADVACKTIMEOUT,     "DMLERR_UNADVACKTIMEOUT",
    DMLERR_UNFOUND_QUEUE_ID,    "DMLERR_UNFOUND_QUEUE_ID",
    0,                          NULL
};


/*
 * fmtAlias
 */
static char *fmtAlias( unsigned long id, char *alias, unsigned type, size_t *prefixlen )
{
    const char  *prefix;
    char        *ret;
    size_t      len;
    int         fmt_len;
    char        buf[10];

    switch( type ) {
    case DEALIAS_TASK:
        fmt_len = TASK_FMT_LEN;
        prefix = GetRCString( STR_TASK_PREFIX );
        break;
    case DEALIAS_CONV:
        fmt_len = CONV_FMT_LEN;
        prefix = GetRCString( STR_CONV_PREFIX );
        break;
    case DEALIAS_CLIENT_CONV:
        fmt_len = CONV_FMT_LEN;
        prefix = GetRCString( STR_CLICONV_PREFIX );
        break;
    case DEALIAS_SERVER_CONV:
        fmt_len = CONV_FMT_LEN;
        prefix = GetRCString( STR_SERVCONV_PREFIX );
        break;
    case DEALIAS_HWND:
        fmt_len = HWND_FMT_LEN;
        prefix = GetRCString( STR_HWND_PREFIX );
        break;
    }
    if( alias == NULL ) {
        alias = buf;
        sprintf( alias, "0x%0*lX", fmt_len, id );
    }
    len = strlen( prefix );
    if( prefixlen != NULL )
        *prefixlen = len;
    len += strlen( alias ) + 1;
    ret = MemAlloc( len );
    sprintf( ret, "%s%s", prefix, alias );
    return( ret );

} /* fmtAlias */

/*
 * deAlias - convert a value to its associated alias or
 *           format it as a number if no alias exists
 */
static char *deAlias( unsigned long id, unsigned type )
{
    char        *alias;
    char        *ret;
    AliasHdl    *alias_list;
    size_t      prefixlen;

    alias_list = aliasHdlTable[type];
    if( !ConfigInfo.alias ) {
        alias = NULL;
    } else {
        alias = LookupAlias( *alias_list, id );
    }
    ret = fmtAlias( id, alias, type, &prefixlen );
    if( alias == NULL ) {
        AddAlias( *alias_list, ret + prefixlen, id );
    }
    return( ret );

} /* deAlias */

/*
 * doReplace - replace strings in the list box
 *           - this routine will not handle the case of replacing a string with itself
 */
static void doReplace( HWND lb, WORD searchcnt, char **searchfor, char **replace )
{
    WORD        i;
    WORD        j;
    size_t      pos;
    LRESULT     ret;
    char        buf1[256];
    char        buf2[256];
    char        *inbuf;
    char        *outbuf;
    char        *tmp;
    char        *ptr;

    inbuf = buf1;
    outbuf = buf2;

    /* for each line in the listbox */
    for( i = 0;; i++ ) {
        ret = SendMessage( lb, LB_GETTEXT, i, (LPARAM)(LPSTR)inbuf );
        if( ret == LB_ERR ) {
            break;
        }

        /* search for each thing */
        for( j = 0; j < searchcnt; j++ ) {

            /* continue searching while there are more */
            ptr = strstr( inbuf, searchfor[j] );
            pos = 0;
            while( ptr != NULL ) {

                /* do the replacement */
                *ptr = '\0';
                strcpy( outbuf, inbuf );
                strcat( outbuf, replace[j] );
                ptr += strlen( searchfor[j] );
                strcat( outbuf, ptr );
                pos = ptr - inbuf;

                /* swap the buffers */
                tmp = inbuf;
                inbuf = outbuf;
                outbuf = tmp;
                ptr = strstr( inbuf + pos, searchfor[j] );
            }
        }

        /* replace the listbox line */
        SendMessage( lb, LB_INSERTSTRING, i, (LPARAM)(LPSTR)inbuf );
        SendMessage( lb, LB_DELETESTRING, i + 1, 0 );
    }

} /* doReplace */

/*
 * updateAlias
 */
static void updateAlias( unsigned long id, char *newalias, char *oldalias, void *_info )
{
    WORD        i;
    WORD        searchcnt;
    char        *searchfor[3];
    char        *replace[3];
    ReplaceInfo *info = _info;

    if( oldalias != NULL && newalias != NULL ) {
        if( !strcmp( oldalias, newalias ) ) {
            return;
        }
    }
    if( info->type == DEALIAS_CONV ) {
        searchcnt = 3;
        searchfor[0] = fmtAlias( id, oldalias, DEALIAS_CONV, NULL );
        searchfor[1] = fmtAlias( id, oldalias, DEALIAS_CLIENT_CONV, NULL );
        searchfor[2] = fmtAlias( id, oldalias, DEALIAS_SERVER_CONV, NULL );
        replace[0] = fmtAlias( id, newalias, DEALIAS_CONV, NULL );
        replace[1] = fmtAlias( id, newalias, DEALIAS_CLIENT_CONV, NULL );
        replace[2] = fmtAlias( id, newalias, DEALIAS_SERVER_CONV, NULL );
    } else {
        searchcnt = 1;
        searchfor[0] = fmtAlias( id, oldalias, info->type, NULL );
        replace[0] = fmtAlias( id, newalias, info->type, NULL );
    }
    doReplace( info->lb, searchcnt, searchfor, replace );
    for( i = 0; i < searchcnt; i++ ) {
        MemFree( searchfor[i] );
        MemFree( replace[i] );
    }

} /* updateAlias */

/*
 * refreshAnAlias
 */
static void refreshAnAlias( unsigned long id, char *text, void *info )
{
    char        *ptr;
    size_t      prefix;

    if( id == (unsigned long)-1L ) {
        return;
    }
    ptr = fmtAlias( id, NULL, ((ReplaceInfo *)info)->type, &prefix );
    if( ConfigInfo.alias ) {
        updateAlias( id, text, ptr + prefix, info );
    } else {
        updateAlias( id, ptr + prefix, text, info );
    }
    MemFree( ptr );

} /* refreshAnAlias */

/*
 * InitAliases
 */
void InitAliases( void )
{
    DDEWndInfo          *info;

    info = (DDEWndInfo *)GET_WNDINFO( DDEMainWnd );
    hwndReplace.type = DEALIAS_HWND;
    hwndReplace.lb = info->list.box;
    convReplace.type = DEALIAS_CONV;
    convReplace.lb = info->list.box;
    taskReplace.type = DEALIAS_TASK;
    taskReplace.lb = info->list.box;
    InitAliasHdl( &HwndAlias, updateAlias, &hwndReplace );
    InitAliasHdl( &ConvAlias, updateAlias, &convReplace );
    InitAliasHdl( &TaskAlias, updateAlias, &taskReplace );

} /* InitAliases */

/*
 * RefreshAliases - replace all the aliases because the user has activated
 *                  or deactivated them
 */
void RefreshAliases( void )
{
    EnumAliases( HwndAlias, refreshAnAlias, &hwndReplace );
    EnumAliases( ConvAlias, refreshAnAlias, &convReplace );
    EnumAliases( TaskAlias, refreshAnAlias, &taskReplace );

} /* RefreshAliases */

/*
 * GetFmtStr - convert a format code to an appropriate string
 */
char *GetFmtStr( UINT fmt, char *buf )
{
    char        *ret;
    const char  *fmtstr;

    ret = SrchMsg( fmt, FormatMsgs, NULL );
    if( ret == NULL ) {
        ret = buf;
        fmtstr = GetRCString( STR_UNKNOWN_FMT_ID );
        sprintf( buf, fmtstr, FMT_ID_LEN, fmt );
    } else {
        strcpy( buf, ret );
    }
    return( buf );

} /* GetFmtStr */

/*
 * HSZToString - gets the string associated with hsz and allocates a buffer
 *               to hold it
 *             - the returned string must be freed with a call to MemFree
 */
char *HSZToString( HSZ hsz )
{
    char        *ret;
    int         codepage;
    unsigned    len;

#ifdef __NT__
    codepage = CP_WINANSI;
#else
    codepage = GetKBCodePage();
#endif
    len = DdeQueryString( DDEInstId, hsz, NULL, 0, codepage );
    len++;
    ret = MemAlloc( len );
    DdeQueryString( DDEInstId, hsz, ret, len, codepage );
    return( ret );

} /* HSZToString */

/*
 * setHorzExtent - set the horizontal extent of the list box
 */
static void setHorzExtent( DDEWndInfo *info, char *text )
{
    HDC         dc;
    HFONT       font;
    SIZE        sz;

    dc = GetDC( info->list.box );
    font = GetMonoFont();
    font = SelectObject( dc, font );
    GetTextExtentPoint( dc, text, (int)strlen( text ), &sz );
    if( sz.cx > info->horz_extent ) {
        SendMessage( info->list.box, LB_SETHORIZONTALEXTENT, sz.cx, 0L );
        info->horz_extent = sz.cx;
    }
    SelectObject( dc, font );
    ReleaseDC( info->list.box, dc );

} /* setHorzExtent */

/*
 * RecordMsg
 */
void RecordMsg( char *buf )
{
    DDEWndInfo          *info;
    int                 ret;
    char                *ptr;
    char                *start;

    ptr = buf;
    start = buf;
    info = (DDEWndInfo *)GET_WNDINFO( DDEMainWnd );
    while( *ptr ) {
        if( *ptr == '\n' ) {
            *ptr = '\0';
            if( ConfigInfo.screen_out ) {
                setHorzExtent( info, start );
                SendMessage( info->list.box, LB_ADDSTRING, 0, (LPARAM)(LPSTR)start );
            }
            SpyLogOut( start );
            *ptr = '\n';
            start = ptr + 1;
        }
        ptr++;
    }
    if( start != ptr ) {
        if( ConfigInfo.screen_out ) {
            setHorzExtent( info, start );
            ret = (int)SendMessage( info->list.box, LB_ADDSTRING, 0, (LPARAM)(LPSTR)start );
        }
        SpyLogOut( start );
    }
    if( ConfigInfo.scroll && ConfigInfo.screen_out ) {
        SendMessage( info->list.box, LB_SETTOPINDEX, ret, 0L );
    }
    /* NYI do something if the list box is full */

} /* RecordMsg */


/*
 * processCBStruct - convert information in a MONCBSTRUCT to a string for display
 */
static void processCBStruct( char *buf, MONCBSTRUCT *info )
{
    char                *type;
    char                *str1;
    char                *str2;
    char                *fmt;
    char                *task;
    char                *conv;
    bool                type_not_found;
    char                fbuf[40];

    fmt = GetFmtStr( info->wFmt, fbuf );
    type = SrchMsg( info->wType, XTypMsgs, NULL );
    if( type == NULL ) {
        type = AllocRCString( STR_UNKNOWN );
        type_not_found = true;
    } else {
        type_not_found = false;
    }
    task = deAlias( (unsigned long)info->hTask, DEALIAS_TASK );
    conv = deAlias( (unsigned long)info->hConv, DEALIAS_CONV );
    switch( info->wType ) {
    case XTYP_ADVSTART:
    case XTYP_ADVSTOP:
    case XTYP_REQUEST:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_ADVSTART_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1, info->hsz2, str2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_CONNECT:
    case XTYP_WILDCONNECT:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_CONNECT_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1,
                   info->hsz2, str2, info->dwData1, info->dwData2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_ADVREQ:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_ADVREQ_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1,
                   info->hsz2, str2, info->dwData1 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_ADVDATA:
    case XTYP_POKE:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_ADVDATA_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1,
                   info->hsz2, str2, info->hData );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_EXECUTE:
        str1 = HSZToString( info->hsz1 );
        RCsprintf( buf, STR_EXECUTE_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1, info->hData );
        MemFree( str1 );
        break;
    case XTYP_CONNECT_CONFIRM:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_CONFIRM_FRM_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1,
                   info->hsz2, str2, info->dwData2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_DISCONNECT:
        RCsprintf( buf, STR_DISCONNECT_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->dwData2 );
        break;
    case XTYP_ERROR:
        RCsprintf( buf, STR_ERROR_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->dwData1 );
        break;
    case XTYP_XACT_COMPLETE:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_XACT_CPLT_FMT_STR, info->dwTime, task, type,
                   type, info->dwRet, conv, fmt, info->hsz1, str1,
                   info->hsz2, str2, info->hData, info->dwData1, info->dwData2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_UNREGISTER:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_UNREGISTER_FMT_STR, info->dwTime, task, type,
                   info->hsz1, str1, info->hsz2, str2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    case XTYP_REGISTER:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_REGISTER_FMT_STR, info->dwTime, task, type,
                   info->hsz1, str1, info->hsz2, str2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    default:
        str1 = HSZToString( info->hsz1 );
        str2 = HSZToString( info->hsz2 );
        RCsprintf( buf, STR_DEFAULT_FMT_STR, info->dwTime, task, type,
                   info->dwRet, conv, fmt, info->hsz1, str1,
                   info->hsz2, str2, info->hData, info->dwData1, info->dwData2 );
        MemFree( str1 );
        MemFree( str2 );
        break;
    }

    if( type_not_found ) {
        FreeRCString( type );
    }
    MemFree( task );
    MemFree( conv );

} /* processCBStruct */

/*
 * processConvStruct - convert information in a MONCONVSTRUCT to a string for display
 */
static void processConvStruct( char *buf, MONCONVSTRUCT *info )
{
    char                *server;
    char                *topic;
    char                *task;
    char                *serverconv;
    char                *clientconv;
    msg_id              fmtstr;

    server = HSZToString( info->hszSvc );
    topic = HSZToString( info->hszTopic );
    task = deAlias( (unsigned long)info->hTask, DEALIAS_TASK );
    clientconv = deAlias( (unsigned long)info->hConvClient, DEALIAS_CLIENT_CONV );
    serverconv = deAlias( (unsigned long)info->hConvServer, DEALIAS_SERVER_CONV );
    if( info->fConnect ) {
        fmtstr = STR_CONV_EST_FMT_STR;
    } else {
        fmtstr = STR_CONV_TERM_FMT_STR;
    }
    RCsprintf( buf, fmtstr, info->dwTime, task, info->hszSvc, server, info->hszTopic,
               topic, clientconv, serverconv );
    MemFree( server );
    MemFree( topic );
    MemFree( task );
    MemFree( clientconv );
    MemFree( serverconv );

} /* processConvStruct */

/*
 * processErrStruct - convert information in a MONERRSTRUCT to a string for display
 */
static void processErrStruct( char *buf, MONERRSTRUCT *info )
{
    char                *type;
    char                *task;

    task = deAlias( (unsigned long)info->hTask, DEALIAS_TASK );
    type = SrchMsg( info->wLastError, DDEErrorMsgs, "" );
    RCsprintf( buf, STR_ERR_STRUCT_FMT_STR, info->dwTime, task, type );
    MemFree( task );

} /* processErrStruct */

/*
 * processHSZStruct - convert information in a MONHSZSTRUCT to a string for display
 */
static bool processHSZStruct( char *buf, MONHSZSTRUCT *info )
{
    char                *task;
    msg_id              fmtid;
    char                *str;
    bool                str_alloced;
#ifdef __NT__
    DWORD               ver;
#endif

    switch( info->fsAction ) {
    case MH_CLEANUP:
    case MH_DELETE:
        fmtid = STR_HSZ_STRUCT_DEL;
        break;
    case MH_CREATE:
        fmtid = STR_HSZ_STRUCT_CREATE;
        break;
    case MH_KEEP:
        fmtid = STR_HSZ_STRUCT_KEPT;
        break;
    default:
        return( false );
    }
    task = deAlias( (unsigned long)info->hTask, DEALIAS_TASK );
    str = info->str;
    str_alloced = false;
#ifdef __NT__
    ver = GetVersion();

    /* In NT 3.1 the string is Unicode.  Otherwise it is ASCII. */
    if( (ver & 0xFF) == 3 && (ver & 0xFF00) <= 0x0A00 ) {
        str = MemAlloc( lstrlenW( (LPCWSTR)info->str ) + 1 );
        str_alloced = true;
        wsprintf( str, "%ls", info->str );
    }
#endif
    RCsprintf( buf, fmtid, info->dwTime, task, HSZ_FMT_LEN, info->hsz, str );
    if( str_alloced ) {
        MemFree( str );
    }
    MemFree( task );
    return( true );

} /* processHSZStruct */

/*
 * processLinkStruct - convert information in a MONLINKSTRUCT to a string for display
 */
static void processLinkStruct( char *buf, MONLINKSTRUCT *info )
{
    char                fbuf[40];
    char                *fmt;
    char                *service;
    char                *topic;
    char                *item;
    char                *task;
    char                *clientconv;
    char                *serverconv;
    msg_id              fmtid;

    if( info->fNoData ) {
        if( info->fEstablished ) {
            fmtid = STR_LNK_STRUCT_FMT_STR_1;
        } else {
            fmtid = STR_LNK_STRUCT_FMT_STR_3;
        }
    } else {
        if( info->fEstablished ) {
            fmtid = STR_LNK_STRUCT_FMT_STR_2;
        } else {
            fmtid = STR_LNK_STRUCT_FMT_STR_3;
        }
    }
    service = HSZToString( info->hszSvc );
    topic = HSZToString( info->hszTopic );
    item = HSZToString( info->hszItem );
    fmt = GetFmtStr( info->wFmt, fbuf );
    task = deAlias( (unsigned long)info->hTask, DEALIAS_TASK );
    clientconv = deAlias( (unsigned long)info->hConvClient, DEALIAS_CLIENT_CONV );
    serverconv = deAlias( (unsigned long)info->hConvServer, DEALIAS_SERVER_CONV );
    /* NYI what does fServer mean???? */
    RCsprintf( buf, fmtid, info->dwTime, task, info->hszSvc, service, info->hszTopic,
               topic, info->hszItem, item, fmt, serverconv, clientconv );
    MemFree( task );
    MemFree( clientconv );
    MemFree( serverconv );

} /* processLinkStruct */

/*
 * processMsgStruct - convert information in a MONMSGSTRUCT to a string for display
 */
static void processMsgStruct( char *buf, MONMSGSTRUCT *info, bool posted )
{
    char                *msg;
    char                *task;
    char                *tohwnd;
    bool                msg_not_found;
    msg_id              fmtid;

    task = deAlias( (unsigned long)info->hTask, DEALIAS_TASK );
    tohwnd = deAlias( (unsigned long)info->hwndTo, DEALIAS_HWND );
    msg = SrchMsg( info->wMsg, DDEMsgs, NULL );
    if( msg == NULL ) {
        msg = AllocRCString( STR_UNKNOWN );
        msg_not_found = true;
    } else {
        msg_not_found = false;
    }
    if( posted ) {
        fmtid = STR_MSG_STRUC_FMT_STR_2;
    } else {
        fmtid = STR_MSG_STRUC_FMT_STR_1;
    }
    RCsprintf( buf, fmtid, info->dwTime, task, msg,
               tohwnd, WPARAM_FMT_LEN, info->wParam, info->lParam );
    if( msg_not_found ) {
        FreeRCString( msg );
    }
    MemFree( task );
    MemFree( tohwnd );

} /* processMsgStruct */

/*
 * DDEMsgProc - process DDE messages received from the DDE manager
 */
HDDEDATA CALLBACK DDEMsgProc( UINT type, UINT fmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, ULONG_PTR data1, ULONG_PTR data2 )
{
    char        buf[400];
    void        *info;

    //RecordMsg( "In Message Proc..." );
    fmt = fmt;
    hsz1 = hsz1;
    hsz2 = hsz2;
    data1 = data1;
    hconv = hconv;
    if( type != XTYP_MONITOR ) {
        return( NULL );
    }
    info = DdeAccessData( hdata, NULL );
    if( info == NULL ) {
        return( NULL );
    }
    switch( data2 ) {
    case MF_HSZ_INFO:
        //RecordMsg( "MF_HSZ_INFO msg\n" );
        if( Monitoring[MON_STR_IND] ) {
            if( !processHSZStruct( buf, info ) ) {
                break;
            }
            RecordMsg( buf );
        }
        TrackStringMsg( info );
        break;
    case MF_SENDMSGS:
        //RecordMsg( "MF_SENDMSG msg\n" );
        if( Monitoring[MON_SENT_IND] ) {
            if( DoFilter( ((MONMSGSTRUCT *)info)->wMsg, FILTER_MESSAGE ) ) {
                processMsgStruct( buf, info, false );
                RecordMsg( buf );
            }
        }
        break;
    case MF_POSTMSGS:
        //RecordMsg( "MF_POSTMSG msg\n" );
        if( Monitoring[MON_POST_IND] ) {
            if( DoFilter( ((MONMSGSTRUCT *)info)->wMsg, FILTER_MESSAGE ) ) {
                processMsgStruct( buf, info, true );
                RecordMsg( buf );
            }
        }
        break;
    case MF_CALLBACKS:
        //RecordMsg( "MF_CB msg\n" );
        if( Monitoring[MON_CB_IND] ) {
            if( DoFilter( ((MONCBSTRUCT *)info)->wType, FILTER_CB ) ) {
                processCBStruct( buf, info );
                RecordMsg( buf );
            }
        }
        if( ((MONCBSTRUCT *)info)->wType == XTYP_REGISTER ||
            ((MONCBSTRUCT *)info)->wType == XTYP_UNREGISTER ) {
            TrackServerMsg( info );
        }
        break;
    case MF_ERRORS:
        //RecordMsg( "MF_ERROR msg\n" );
        if( Monitoring[MON_ERR_IND] ) {
            processErrStruct( buf, info );
            RecordMsg( buf );
        }
        break;
    case MF_LINKS:
        //RecordMsg( "MF_LINKS msg\n" );
        if( Monitoring[MON_LNK_IND] ) {
            processLinkStruct( buf, info );
            RecordMsg( buf );
        }
        TrackLinkMsg( info );
        break;
    case MF_CONV:
        //RecordMsg( "MF_CONV msg\n" );
        if( Monitoring[MON_LNK_IND] ) {
            processConvStruct( buf, info );
            RecordMsg( buf );
        }
        TrackConvMsg( info );
        break;
    default:
        //RecordMsg( "Unknown message\n" );
        break;
    }
    DdeUnaccessData( hdata );
    return( (HDDEDATA)0 );

} /* DDEMsgProc */
